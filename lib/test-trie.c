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

#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "common.h"
#include "ptr-traits.h"
#include "pool-alloc.h"
#include "pretty-print.h"

#include "test-common.h"

const char program[] = STRINGIFY(PROGRAM);
const char verdate[] = "0.4 -- 2017-04-05 16:12"; // $ date +'%F %R'

const char help[] = 
"usage: %s [ACTION|OPTION]...\n"
"where actions are specified as:\n"
"  -T|--[print-]trie        print out the built trie (default)\n"
"  -D|--[print-]depths      print out iterator max depths of\n"
"                             the root of the built trie\n"
"  -R|--rebalance-trie      print out the built trie upon\n"
"                             rebalancing it\n"
"the options are:\n"
"  -p|--pool-size NUM[KM]   trie memory pool size (default 128K)\n"
"  -l|--lvl-iter-depth NUM  use NUM as lvl-iterator depth (default: 0)\n"
"  -s|--sib-iter-depth NUM  use NUM as sib-iterator depth (default: 0)\n"
"     --dump-options        print options and exit\n"
"     --version             print version numbers and exit\n"
"  -?|--help                display this help info and exit\n";

enum options_action_t
{
    options_print_trie_action,
    options_print_depths_action,
    options_rebalance_trie_action,
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

static void options_done(struct options_t* opts, size_t n_arg)
{
    ASSERT_SIZE_SUB_NO_OVERFLOW(opts->argc, n_arg);
    opts->argc -= n_arg;
    opts->argv += n_arg;
}

static void options_dump(const struct options_t* opts)
{
#define CASE2(n0, n1) \
    [options_ ## n0 ## _ ## n1 ## _action] = #n0 "-" #n1
    static const char* const actions[] = {
        CASE2(print, trie),
        CASE2(print, depths),
        CASE2(rebalance, trie),
    };
    struct su_size_t pool_su = su_size(opts->pool_size);
    char* const* p;
    size_t i;

#define NAME(x) OPTIONS_NAME(x)

    fprintf(stdout,
        "action:         %s\n"
        "pool-size:      %zu%s\n"
        "lvl-iter-depth: %zu\n"
        "sib-iter-depth: %zu\n"
        "argc:           %zu\n",
        NAME(action),
        pool_su.sz,
        pool_su.su,
        opts->lvl_iter_depth,
        opts->sib_iter_depth,
        opts->argc);

    for (i = 0, p = opts->argv; i < opts->argc; i ++, p ++)
        fprintf(stdout, "argv[%zu]:        %s\n", i, *p);
}

enum {
    // stev: actions:
    options_print_trie_act     = 'T',
    options_print_depths_act   = 'D',
    options_rebalance_trie_act = 'R',

    // stev: options:
    options_pool_size_opt      = 'p',
    options_lvl_iter_depth_opt = 'l',
    options_sib_iter_depth_opt = 's',
};

static bool options_parse(struct options_t* opts,
    int opt, const char* opt_arg)
{
    switch (opt) {
    case options_print_trie_act:
        opts->action = options_print_trie_action;
        break;
    case options_print_depths_act:
        opts->action = options_print_depths_action;
        break;
    case options_rebalance_trie_act:
        opts->action = options_rebalance_trie_action;
        break;
    case options_pool_size_opt:
        opts->pool_size = 
            options_parse_su_size_optarg("pool-size",
                opt_arg, 1, MB(2));
        break;
    case options_lvl_iter_depth_opt:
        opts->lvl_iter_depth = 
            options_parse_size_optarg("lvl-iter-depth",
                opt_arg, 0, 0);
        break;
    case options_sib_iter_depth_opt:
        opts->sib_iter_depth = 
            options_parse_size_optarg("sib-iter-depth",
                opt_arg, 0, 0);
        break;
    default:
        return false;
    }
    return true;
}

static const struct options_t* options(
    int argc, char* argv[])
{
    static struct options_t opts = {
        .action         = options_print_trie_action,
        .pool_size      = KB(128),
        .lvl_iter_depth = 0,
        .sib_iter_depth = 0,
    };

    static struct option longs[] = {
        { "print-trie",     0, 0, options_print_trie_act },
        { "trie",           0, 0, options_print_trie_act },
        { "print-depths",   0, 0, options_print_depths_act },
        { "rebalance-trie", 0, 0, options_rebalance_trie_act },
        { "depths",         0, 0, options_print_depths_act },
        { "pool-size",      1, 0, options_pool_size_opt },
        { "lvl-iter-depth", 1, 0, options_lvl_iter_depth_opt },
        { "sib-iter-depth", 1, 0, options_lvl_iter_depth_opt },
    };
    static const char shorts[] = "DRT" "l:p:s:";

    static struct options_funcs_t funcs = {
        .done  = (options_done_func_t)  options_done,
        .parse = (options_parse_func_t) options_parse,
        .dump  = (options_dump_func_t)  options_dump,
    };

    opts.argc = INT_AS_SIZE(argc);
    opts.argv = argv;

    options_parse_args(
        &opts, &funcs,
        shorts, ARRAY_SIZE(shorts) - 1,
        longs, ARRAY_SIZE(longs),
        argc, argv);

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
#define TRIE_NEED_REBALANCE
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
#undef  TRIE_NEED_REBALANCE
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

static void test_rebalance_trie(
    struct test_t* test,
    const struct options_t* opt UNUSED,
    FILE* file)
{
    test_trie_print(&test->trie, file);
    fputc('\n', file);

    test_trie_rebalance(&test->trie);

    test_trie_print(&test->trie, file);
    fputc('\n', file);
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
        [options_print_trie_action]     = test_print_trie,
        [options_print_depths_action]   = test_print_depths,
        [options_rebalance_trie_action] = (test_action_func_t)
            test_rebalance_trie,
    };
    const struct options_t* opt = options(argc, argv);
    test_action_func_t act = ARRAY_NULL_ELEM(
        actions, opt->action);

    ASSERT(act != NULL);
    exec_test(act, opt);

    return 0;
}


