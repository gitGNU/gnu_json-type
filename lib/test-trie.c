// Copyright (C) 2016  Stefan Vargyas
// 
// This file is part of Json-Type.
// 
// Json-Type is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Json-Type is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Json-Type.  If not, see <http://www.gnu.org/licenses/>.

#include "config.h"

#define _GNU_SOURCE
#include <limits.h>
#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#include "common.h"
#include "ptr-traits.h"
#include "pool-alloc.h"
#include "pretty-print.h"

static const char program[] = STRINGIFY(PROGRAM);
static const char verdate[] = "0.2 -- 2016-05-02 09:20"; // $ date +'%F %R'

static const char stdin_name[] = "<stdin>";

static void error(const char* fmt, ...)
    PRINTF(1)
    NORETURN;

static void error(const char* fmt, ...)
{
    va_list arg;

    char b[256];

    va_start(arg, fmt);
    vsnprintf(b, sizeof b - 1, fmt, arg);
    va_end(arg);

    b[sizeof b - 1] = 0;

    fprintf(
        stderr, "%s: error: %s\n",
        program, b);

    exit(1);
}

enum options_action_t
{
    options_print_trie_action,
    options_print_depths_action
};

struct options_t
{
    enum options_action_t action;
    size_t                pool_size;
    size_t                lvl_iter_depth;
    size_t                sib_iter_depth;

    size_t                argc;
    char* const          *argv;
};

static void version()
{
    fprintf(stdout, "%s: version %s\n", program, verdate);
}

static void usage()
{
    fprintf(stdout,
        "usage: %s [ACTION|OPTION]...\n"
        "where actions are specified as:\n"
        "  -T|--[print-]trie        print out the built trie\n"
        "  -D|--[print-]depths      print out iterator max depths of\n"
        "                             the root of the built trie\n"
        "the options are:\n"
        "  -p|--pool-size NUM[KM]   trie memory pool size (default 128K)\n"
        "  -l|--lvl-iter-depth NUM  use NUM as lvl-iterator depth (default: 0)\n"
        "  -s|--sib-iter-depth NUM  use NUM as sib-iterator depth (default: 0)\n"
        "     --dump-options        print options and exit\n"
        "     --version             print version numbers and exit\n"
        "  -?|--help                display this help info and exit\n",
        program);
}

#define KB(X) (SZ(X) * SZ(1024))
#define MB(X) (KB(X) * SZ(1024))

struct su_size_t
{
    size_t sz;
    const char* su;
};

static struct su_size_t su_size(size_t sz)
{
    struct su_size_t r;

    r.sz = sz;
    r.su = "";
    if (r.sz  % MB(1) == 0) {
        r.sz /= MB(1);
        r.su = "M";
    }
    else
    if (r.sz  % KB(1) == 0) {
        r.sz /= KB(1);
        r.su = "K";
    }
    return r;
}

static void dump_options(const struct options_t* opts)
{
#define CASE2(n0, n1) \
    [options_ ## n0 ## _ ## n1 ## _action] = #n0 "-" #n1
    static const char* const actions[] = {
        CASE2(print, trie),
        CASE2(print, depths),
    };
    struct su_size_t pool_su = su_size(opts->pool_size);
    char* const* p;
    size_t i;

    fprintf(stdout,
        "action:         %s\n"
        "pool-size:      %zu%s\n"
        "lvl-iter-depth: %zu\n"
        "sib-iter-depth: %zu\n"
        "argc:           %zu\n",
#define NAME0(X, T)                  \
    ({                               \
        size_t __v = opts->X;        \
        ARRAY_NON_NULL_ELEM(T, __v); \
    })
#define NAME(X)  (NAME0(X, X ## s))
#define NNUL(X)  (opts->X ? opts->X : "-")
#define NOYES(X) (NAME0(X, noyes))
        NAME(action),
        pool_su.sz,
        pool_su.su,
        opts->lvl_iter_depth,
        opts->sib_iter_depth,
        opts->argc);

    for (i = 0, p = opts->argv; i < opts->argc; i ++, p ++)
        fprintf(stdout, "argv[%zu]:        %s\n", i, *p);
}

static void invalid_opt_arg(const char* opt_name, const char* opt_arg)
{
    error("invalid argument for '%s' option: '%s'", opt_name, opt_arg);
}

static void illegal_opt_arg(const char* opt_name, const char* opt_arg)
{
    error("illegal argument for '%s' option: '%s'", opt_name, opt_arg);
}

static void missing_opt_arg_str(const char* opt_name)
{
    error("argument for option '%s' not found", opt_name);
}

static void missing_opt_arg_ch(char opt_name)
{
    error("argument for option '-%c' not found", opt_name);
}

static void not_allowed_opt_arg(const char* opt_name)
{
    error("option '%s' does not allow an argument", opt_name);
}

static void invalid_opt_str(const char* opt_name)
{
    error("invalid command line option '%s'", opt_name);
}

static void invalid_opt_ch(char opt_name)
{
    error("invalid command line option '-%c'", opt_name);
}

static size_t parse_num(
    const char* p, const char** q, int b)
{
    if (!isdigit(*p)) {
        *q = p;
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    STATIC(SIZE_MAX == ULONG_MAX);
    return strtoul(p, (char**) q, b);
}

static size_t parse_size_optarg(
    const char* opt_name, const char* opt_arg)
{
    const char *p, *q = NULL;
    size_t n, v, d;

    if (!(n = strlen(opt_arg)))
        invalid_opt_arg(opt_name, opt_arg);
    v = parse_num(p = opt_arg, &q, 10);
    d = PTR_OFFSET(q, p, n);
    if (errno || (d != n))
        invalid_opt_arg(opt_name, opt_arg);
#if 0
    if (v == 0)
        illegal_opt_arg(opt_name, opt_arg);
#endif
    return v;
}

static size_t parse_su_size_optarg(
    const char* opt_name, const char* opt_arg)
{
    const char *p, *q = NULL;
    size_t n, v, d;

    if (!(n = strlen(opt_arg)))
        invalid_opt_arg(opt_name, opt_arg);
    v = parse_num(p = opt_arg, &q, 10);
    d = PTR_OFFSET(q, p, n);
    if (errno ||
        (d == 0) ||
        (d < n - 1) ||
        (d == n - 1 && *q != 'k' && *q != 'K' &&
            *q != 'm' && *q != 'M'))
        invalid_opt_arg(opt_name, opt_arg);
    switch (*q) {
    case '\0':
        break;
    case 'k':
    case 'K':
        STATIC(KB(1) <= SIZE_MAX);
        if (v > SIZE_MAX / KB(1))
            illegal_opt_arg(opt_name, opt_arg);
        v *= KB(1);
        break;
    case 'm':
    case 'M':
        STATIC(MB(1) <= SIZE_MAX);
        if (v > SIZE_MAX / MB(1))
            illegal_opt_arg(opt_name, opt_arg);
        v *= MB(1);
        break;
    }
    STATIC(MB(2) < SIZE_MAX);
    if (v == 0 || v > MB(2))
        illegal_opt_arg(opt_name, opt_arg);
    return v;
}

static const struct options_t* options(
    int argc, char* argv[])
{
    static struct options_t opts = {
        .action         = options_print_trie_action,
        .pool_size      = KB(128),
        .lvl_iter_depth = 0,
        .sib_iter_depth = 0,
        .argc           = 0,
        .argv           = NULL
    };

    enum {
        // stev: actions:
        print_trie_act     = 'T',
        print_depths_act   = 'D',

        // stev: options:
        pool_size_opt      = 'p',
        lvl_iter_depth_opt = 'l',
        sib_iter_depth_opt = 's',
        help_opt           = '?',

        dump_opts_opt      = 128,
        version_opt,
    };

    static struct option long_opts[] = {
        { "print-trie",     0,       0, print_trie_act },
        { "trie",           0,       0, print_trie_act },
        { "print-depths",   0,       0, print_depths_act },
        { "depths",         0,       0, print_depths_act },
        { "pool-size",      1,       0, pool_size_opt },
        { "lvl-iter-depth", 1,       0, lvl_iter_depth_opt },
        { "sib-iter-depth", 1,       0, lvl_iter_depth_opt },
        { "dump-options",   0,       0, dump_opts_opt },
        { "version",        0,       0, version_opt },
        { "help",           0, &optopt, help_opt },
        { 0,                0,       0, 0 }
    };
    static const char short_opts[] = ":" "DT" "l:p:s:";

    struct bits_opts_t {
        bits_t dump: 1;
        bits_t usage: 1;
        bits_t version: 1;
    };
    struct bits_opts_t bits = {
        .dump    = false,
        .usage   = false,
        .version = false
    };

    int opt;

#define argv_optind()                      \
    ({                                     \
        size_t i = INT_AS_SIZE(optind);    \
        ASSERT_SIZE_DEC_NO_OVERFLOW(i);    \
        ASSERT(i - 1 < INT_AS_SIZE(argc)); \
        argv[i - 1];                       \
    })

#define optopt_char()            \
    ({                           \
        ASSERT(isascii(optopt)); \
        (char) optopt;           \
    })

    opterr = 0;
    optind = 1;
    while ((opt = getopt_long(argc, argv, short_opts,
        long_opts, 0)) != EOF) {
        switch (opt) {
        case print_trie_act:
            opts.action = options_print_trie_action;
            break;
        case print_depths_act:
            opts.action = options_print_depths_action;
            break;
        case pool_size_opt:
            opts.pool_size = 
                parse_su_size_optarg("pool-size", optarg);
            break;
        case lvl_iter_depth_opt:
            opts.lvl_iter_depth = 
                parse_size_optarg("lvl-iter-depth", optarg);
            break;
        case sib_iter_depth_opt:
            opts.sib_iter_depth = 
                parse_size_optarg("sib-iter-depth", optarg);
            break;
        case dump_opts_opt:
            bits.dump = true;
            break;
        case version_opt:
            bits.version = true;
            break;
        case 0:
            bits.usage = true;
            break;
        case ':': {
            const char* opt = argv_optind();
            if (opt[0] == '-' && opt[1] == '-')
                missing_opt_arg_str(opt);
            else
                missing_opt_arg_ch(optopt_char());
            break;
        }
        case '?':
        default:
            if (optopt == 0)
                invalid_opt_str(argv_optind());
            else
            if (optopt != '?') {
                char* opt = argv_optind();
                if (opt[0] == '-' && opt[1] == '-') {
                    char* end = strchr(opt, '=');
                    if (end) *end = '\0';
                    not_allowed_opt_arg(opt);
                }
                else
                    invalid_opt_ch(optopt_char());
            }
            else
                bits.usage = true;
            break;
        }
    }

    ASSERT(argc >= optind);

    argc -= optind;
    argv += optind;

    opts.argc = argc;
    opts.argv = argv;

    if (bits.version)
        version();
    if (bits.dump)
        dump_options(&opts);
    if (bits.usage)
        usage();

    if (bits.dump ||
        bits.version ||
        bits.usage)
        exit(0);

    return &opts;
}

static void test_print_string_len(const char* str,
    size_t len, FILE* file)
{
    pretty_print_string(
        file, (const uchar_t*) str, len,
        pretty_print_string_quotes);
}

static void test_print_string(const char* str, FILE* file)
{
    test_print_string_len(str, strlen(str), file);
}

static void test_print_char(char ch, FILE* file)
{
    test_print_string_len(&ch, 1, file);
}

static void test_print_ch(char ch, FILE* file)
{
    pretty_print_char(
        file, ch, pretty_print_char_quotes);
}

struct test_t;

#define TEST_TRIE_SYM_IS_NULL(x) (x == '\0')

#define TEST_TRIE_SYM_CMP(x, y)     \
    (                               \
        STATIC(TYPEOF_IS(x, char)), \
        STATIC(TYPEOF_IS(y, char)), \
          x < y                     \
        ? trie_sym_cmp_lt           \
        : x > y                     \
        ? trie_sym_cmp_gt           \
        : trie_sym_cmp_eq           \
    )

#define TRIE_NAME           test
#define TRIE_SYM_TYPE       char
#define TRIE_NULL_SYM()     ('\0')
#define TRIE_SYM_IS_NULL    TEST_TRIE_SYM_IS_NULL
#define TRIE_SYM_CMP        TEST_TRIE_SYM_CMP
#define TRIE_SYM_PRINT      test_print_char
#define TRIE_VAL_TYPE       const char*
#define TRIE_VAL_PRINT      test_print_string
#define TRIE_KEY_TYPE       const char*
#define TRIE_KEY_INC(k)     ((k) ++)
#define TRIE_KEY_DEREF(k)   ((*k))
#define TRIE_ALLOC_OBJ_TYPE struct test_t

#define TRIE_DEBUG_LVL_ITERATOR
#define TRIE_DEBUG_FILE stdout

#define TRIE_NEED_PRINT
#define TRIE_NEED_SIB_ITERATOR
#define TRIE_NEED_LVL_ITERATOR
#define TRIE_NEED_NODE_GET_ITER_MAX_DEPTH
#define TRIE_NEED_NODE_GET_SIB_ITER_MAX_DEPTH
#define TRIE_NEED_NODE_GET_LVL_ITER_MAX_DEPTH
#define TRIE_NEED_INSERT_KEY
#include "trie-impl.h"
#undef  TRIE_NEED_INSERT_KEY
#undef  TRIE_NEED_NODE_GET_LVL_ITER_MAX_DEPTH
#undef  TRIE_NEED_NODE_GET_SIB_ITER_MAX_DEPTH
#undef  TRIE_NEED_NODE_GET_ITER_MAX_DEPTH
#undef  TRIE_NEED_LVL_ITERATOR
#undef  TRIE_NEED_SIB_ITERATOR
#undef  TRIE_NEED_PRINT

#define TEST_TRIE_NODE_AS_VAL(p) \
    TRIE_NODE_AS_VAL( \
        test, p, \
        TEST_TRIE_SYM_IS_NULL)
#define TEST_TRIE_NODE_AS_VAL_REF(p)  \
    ({                                \
        const char* const* __r;       \
        __r = TRIE_NODE_AS_VAL_REF(   \
            test, p,                  \
            TEST_TRIE_SYM_IS_NULL);   \
        CONST_CAST(__r, const char*); \
    })

struct test_t
{
    struct pool_alloc_t pool;
    struct test_trie_t  trie;
};

static void* test_allocate(
    struct test_t* test, size_t size, size_t align)
{
    void* n;

    if ((n = pool_alloc_allocate(
            &test->pool, size, align)) == NULL)
        fatal_error("pool alloc failed");

    memset(n, 0, size);

    return n;
}

#define TEST_ALLOCATE_ARRAY(t, n)                    \
    ({                                               \
        STATIC(TYPEOF_IS_SIZET(n));                  \
        ASSERT_SIZE_MUL_NO_OVERFLOW((n), sizeof(t)); \
        test_allocate(test, (n) * sizeof(t),         \
            MEM_ALIGNOF(t[0]));                      \
    })

static const char* test_new_string(
    struct test_t* test, const char* str, size_t len)
{
    char* s;

    ASSERT_SIZE_INC_NO_OVERFLOW(len);
    s = TEST_ALLOCATE_ARRAY(char, len + 1);
    memcpy(s, str, len);
    s[len] = 0;

    return s;
}

#define TEST_ALLOCATE_STRUCT(t)     \
    (                               \
        test_allocate(              \
            test, sizeof(struct t), \
            MEM_ALIGNOF(struct t))  \
    )

static struct test_trie_node_t* test_new_trie_node(
    struct test_t* test)
{
    return TEST_ALLOCATE_STRUCT(test_trie_node_t);
}

static void test_init(struct test_t* test, size_t size)
{
    pool_alloc_init(&test->pool, size);
    test_trie_init(&test->trie, test_new_trie_node, test);
}

static void test_done(struct test_t* test)
{
    pool_alloc_done(&test->pool);
}

static void test_load_trie(
    struct test_t* test, FILE* file, const char* name)
{
    char* b = NULL;
    size_t n = 0, s = 0;
    ssize_t r;

    while ((r = getline(&b, &n, file)) >= 0) {
        const struct test_trie_node_t* t = NULL;
        size_t l = INT_AS_SIZE(r);

        s ++;

        ASSERT(b != NULL);
        ASSERT(n > 0);

        ASSERT(l > 0);
        ASSERT(l < n);

        if (b[l - 1] == '\n')
            b[-- l] = '\0';

        if (!test_trie_insert_key(&test->trie, b, &t)) {
            fprintf(stderr,
                "%s: error: %s:%zu: duplicated input string: ",
                program, name, s);
            test_print_string_len(b, l, stderr);
            fputc('\n', stderr);
        }
        else {
            ASSERT(t != NULL);
            *TEST_TRIE_NODE_AS_VAL_REF(t) = 
                test_new_string(test, b, l);
        }
    }

    if (b != NULL) {
        ASSERT(n > 0);
        free(b);
    }
}

static void test_print_trie(
    const struct test_t* test,
    const struct options_t* opt,
    FILE* file)
{
    struct test_trie_lvl_iterator_t l;
    struct test_trie_sib_iterator_t s;
    const struct test_trie_node_t *t, *u;

    fputs("trie=", file);
    test_trie_print(&test->trie, file);
    fputc('\n', file);

    test_trie_lvl_iterator_init(
        &l, &test->trie, opt->lvl_iter_depth);

    while (!test_trie_lvl_iterator_at_end(&l)) {
        t = test_trie_lvl_iterator_deref(&l);
        ASSERT(t != NULL);

        fputs("node=", file);
        test_trie_print_node(t, file);
        fputc('\n', file);

        test_trie_sib_iterator_init(
            &s, t, opt->sib_iter_depth);

        while (!test_trie_sib_iterator_at_end(&s)) {
            u = test_trie_sib_iterator_deref(&s);
            ASSERT(u != NULL);

            fputs("char=", file);
            test_print_ch(u->sym, file);
            fputc('\n', file);

            test_trie_sib_iterator_inc(&s);
        }

        test_trie_sib_iterator_done(&s);
        test_trie_lvl_iterator_inc(&l);
    }

    test_trie_lvl_iterator_done(&l);
}

static void test_print_depths(
    const struct test_t* test,
    const struct options_t* opt UNUSED,
    FILE* file)
{
    const struct test_trie_node_t* n;

    n = test->trie.root;
    ASSERT(n != NULL);

    fprintf(file,
        "iter:     %zu\n"
        "sib-iter: %zu\n"
        "lvl-iter: %zu\n",
        test_trie_node_get_iter_max_depth(n),
        test_trie_node_get_sib_iter_max_depth(n),
        test_trie_node_get_lvl_iter_max_depth(n));
}

typedef void (*test_action_func_t)(
    const struct test_t*,
    const struct options_t* opt,
    FILE*);

static void exec_test(
    test_action_func_t test_act,
    const struct options_t* opt)
{
    struct test_t test;

    test_init(&test, opt->pool_size);

    test_load_trie(&test, stdin, stdin_name);
    test_act(&test, opt, stdout);

    test_done(&test);
}

int main(int argc, char* argv[])
{
    static const test_action_func_t actions[] = {
        [options_print_trie_action]   = test_print_trie,
        [options_print_depths_action] = test_print_depths,
    };
    const struct options_t* opt = options(argc, argv);
    test_action_func_t act = ARRAY_NULL_ELEM(
        actions, opt->action);

    ASSERT(act != NULL);
    exec_test(act, opt);

    return 0;
}


