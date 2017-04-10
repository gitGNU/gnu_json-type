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
#include <sys/time.h>

#include "common.h"
#include "ptr-traits.h"

#include "test-common.h"

const char program[] = STRINGIFY(PROGRAM);
const char verdate[] = "0.1 -- 2017-04-05 16:49"; // $ date +'%F %R'

const char help[] = 
"usage: %s [ACTION|OPTION]...\n"
"where the actions are:\n"
"  -T|--gen-type        generate json type\n"
"  -V|--gen-value       generate JSON value (default)\n"
"and the options are:\n"
"  -d|--depth=NUM       depth of generated JSON objects\n"
"                         (default value: 2)\n"
"  -k|--n-key=NUM       number of keys on first generated\n"
"                         level of JSON objects (default\n"
"                         value: 4)\n"
"  -n|--div=NUM         divider for the number of keys at\n"
"                         each level of the generated JSON\n"
"                         objects (default value: 4)\n"
"  -p|--plain-types     when action is `-T|--gen-type' do\n"
"                         generate the leaf type objects\n"
"                         to be \"plain\" type objects\n"
"                         instead of \"number\"'s\n"
"  -r|--randomize       when action is `-V|--gen-value' do\n"
"                         randomize the order of arguments\n"
"                         of the generated object\n"
"  -t|--json-type=TYPE  the kind of json types generated:\n"
"     --object            either 'o[bject[' or 'd[ict]';\n"
"     --dict              the default is 'object'\n"
"     --dump-options    print options and exit\n"
"     --version         print version numbers and exit\n"
"  -?|--help            display this help info and exit\n";

enum options_action_t
{
    options_gen_type_action,
    options_gen_value_action,
};

enum options_json_type_t
{
    options_json_object_type,
    options_json_dict_type,
};

struct options_t
{
    enum options_action_t
                 action;
    enum options_json_type_t
                 json_type;
    bits_t       plain_types: 1;
    bits_t       randomize: 1;
    size_t       depth;
    size_t       nkey;
    size_t       div;
    size_t       argc;
    char* const *argv;
};

static void options_done(struct options_t* opts, size_t n_arg)
{
    ASSERT_SIZE_SUB_NO_OVERFLOW(opts->argc, n_arg);
    opts->argc -= n_arg;
    opts->argv += n_arg;
}

static void options_dump(const struct options_t* opts)
{
    static const char* const noyes[] = {
        [0] "no", [1] "yes"
    };
#define CASE2(n0, n1) \
    [options_ ## n0 ## _ ## n1 ## _action] = #n0 "-" #n1
    static const char* const actions[] = {
        CASE2(gen, type),
        CASE2(gen, value),
    };
#define CASE(n) \
    [options_json_ ## n ## _type] = #n
    static const char* const json_types[] = {
        CASE(object),
        CASE(dict),
    };
    char* const* p;
    size_t i;

#define NAME(x)  OPTIONS_NAME(x)
#define NOYES(x) OPTIONS_NOYES(x)

    fprintf(stdout,
        "action:      %s\n"
        "json-type:   %s\n"
        "plain-types: %s\n"
        "randomize:   %s\n"
        "depth:       %zu\n"
        "nkey:        %zu\n"
        "div:         %zu\n"
        "argc:        %zu\n",
        NAME(action),
        NAME(json_type),
        NOYES(plain_types),
        NOYES(randomize),
        opts->depth,
        opts->nkey,
        opts->div,
        opts->argc);

    for (i = 0, p = opts->argv; i < opts->argc; i ++, p ++)
        fprintf(stdout, "argv[%zu]:     %s\n", i, *p);
}

enum {
    // stev: actions:
    options_gen_type_act    = 'T',
    options_gen_value_act   = 'V',

    // stev: options:
    options_depth_opt       = 'd',
    options_nkey_opt        = 'k',
    options_div_opt         = 'n',
    options_plain_types_opt = 'p',
    options_randomize_opt   = 'r',
    options_json_type_opt   = 't',

    options_object_opt      = 256,
    options_dict_opt,
};

// $ . ~/lookup-gen/commands.sh
// $ print() { printf '%s\n' "$@"; }
// $ adjust-func() { ssed -R '1s/^/static /;1s/\&/*/;s/(,\s+)/\1enum /;s/(?=t =)/*/;1s/(?<=\()/\n\t/;s/([a-z0-9_]+)(_type)_t::([a-z]+)/\1_\3\2/'; }

// $ print 'o object' 'd dict'|gen-func -f options_lookup_json_type -r options_json_type_t|adjust-func

static bool options_lookup_json_type(
    const char* n, enum options_json_type_t* t)
{
    // pattern: d[ict]|o[bject]
    switch (*n ++) {
    case 'd':
        if (*n == 0) {
            *t = options_json_dict_type;
            return true;
        }
        if (*n ++ == 'i' &&
            *n ++ == 'c' &&
            *n ++ == 't' &&
            *n == 0) {
            *t = options_json_dict_type;
            return true;
        }
        return false;
    case 'o':
        if (*n == 0) {
            *t = options_json_object_type;
            return true;
        }
        if (*n ++ == 'b' &&
            *n ++ == 'j' &&
            *n ++ == 'e' &&
            *n ++ == 'c' &&
            *n ++ == 't' &&
            *n == 0) {
            *t = options_json_object_type;
            return true;
        }
    }
    return false;
}

static enum options_json_type_t
    options_parse_json_type_optarg(
        const char* opt_name, const char* opt_arg)
{
    enum options_json_type_t r;

    ASSERT(opt_arg != NULL);
    if (!options_lookup_json_type(opt_arg, &r))
        options_invalid_opt_arg(opt_name, opt_arg);

    return r;
}

static bool options_parse(struct options_t* opts,
    int opt, const char* opt_arg)
{
    switch (opt) {
    case options_gen_type_act:
        opts->action = options_gen_type_action;
        break;
    case options_gen_value_act:
        opts->action = options_gen_value_action;
        break;
    case options_json_type_opt:
        opts->json_type = options_parse_json_type_optarg(
            "json-type", opt_arg);
        break;
    case options_object_opt:
        opts->json_type = options_json_object_type;
        break;
    case options_dict_opt:
        opts->json_type = options_json_dict_type;
        break;
    case options_plain_types_opt:
        opts->plain_types = true;
        break;
    case options_randomize_opt:
        opts->randomize = true;
        break;
    case options_depth_opt:
        opts->depth = options_parse_size_optarg(
            "depth", opt_arg, 1, 0);
        break;
    case options_nkey_opt:
        opts->nkey = options_parse_size_optarg(
            "nkey", opt_arg, 1, 0);
        break;
    case options_div_opt:
        opts->div = options_parse_size_optarg(
            "div", opt_arg, 1, 0);
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
        .action      = options_gen_value_action,
        .json_type   = options_json_object_type,
        .plain_types = false,
        .randomize   = false,
        .depth       = 2,
        .nkey        = 4,
        .div         = 4,
    };

    static struct option longs[] = {
        { "gen-type",    0, 0, options_gen_type_act },
        { "gen-value",   0, 0, options_gen_value_act },
        { "plain-types", 0, 0, options_plain_types_opt },
        { "randomize",   0, 0, options_randomize_opt },
        { "json-type",   1, 0, options_json_type_opt },
        { "object",      0, 0, options_object_opt },
        { "dict",        0, 0, options_dict_opt },
        { "depth",       1, 0, options_depth_opt },
        { "nkey",        1, 0, options_nkey_opt },
        { "div",         1, 0, options_div_opt },
    };
    static const char shorts[] = "TV" "d:k:n:prt:";

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

// stev: TODO: promote SIZE_MUL_DOUBLE
// to the global scope of 'int-traits.h'
#define SIZE_MUL_DOUBLE(x, d)            \
    ({                                   \
        double __r;                      \
        STATIC(TYPEOF_IS_SIZET(x));      \
        STATIC(TYPEOF_IS(d, double));    \
        __r = (double) (x) * (d);        \
        VERIFY(__r < (double) SIZE_MAX); \
        (size_t) __r;                    \
    })

// stev: Knuth, TAOCP, vol 2, 3rd edition,
// 3.4.2 Random Sampling and Shuffling, p. 145
// Algorithm P (Shuffling)

static void shuffle(size_t keys[], size_t n)
{
    struct timeval t;
    size_t j;

    if (n <= 1)
        return;

    if (gettimeofday(&t, NULL) < 0)
        t.tv_usec = 0;
    srand48(INT_AS_SIZE(t.tv_usec));

    for (j = n - 1; j > 0; j --) {
        size_t k, t;
        double u;

        // knuth: generate a random number 'u',
        // uniformly distributed between 0 and 1
        u = drand48();
        ASSERT(u >= 0.0 && u < 1.0);

        // stev: make 'k' be a random integer,
        // between 0 and 'j - 1'
        k = SIZE_MUL_DOUBLE(j, u);
        VERIFY(k < j);

        // stev: swap 'keys[k]' and 'keys[j]'
        t = keys[k];
        keys[k] = keys[j];
        keys[j] = t;
    }
}

struct gen_t
{
    FILE*        output;
    enum options_json_type_t
                 json_type;
    bits_t       plain_types: 1;
    bits_t       randomize: 1;
    size_t       nkey_div;
};

static void gen_init(struct gen_t* gen,
    const struct options_t* opt)
{
    memset(gen, 0, sizeof(*gen));

    ASSERT(opt->div > 0);

    gen->output = stdout;
    gen->json_type = opt->json_type;
    gen->plain_types = opt->plain_types;
    gen->randomize = opt->randomize;
    gen->nkey_div = opt->div;
}

#define CASE(n) [options_json_ ## n ## _type] = #n
static const char* const gen_json_types[] = {
    CASE(object),
    CASE(dict)
};

#define SIZE_DIV(x, y)              \
    ({                              \
        STATIC(TYPEOF_IS_SIZET(x)); \
        STATIC(TYPEOF_IS_SIZET(y)); \
        ASSERT(y > 0);              \
        (x) / (y);                  \
    })

#define print_char(c) \
    fputc(c, gen->output)
#define print_str(s) \
    fputs(s, gen->output)
#define print_fmt(f, ...) \
    fprintf(gen->output, f, ## __VA_ARGS__)

static void gen_type_gen(
    const struct gen_t* gen,
    size_t nkey, size_t depth)
{
    size_t i;

    print_fmt("{\"type\":\"%s\",\"args\":[",
        ARRAY_CHAR_NULL_ELEM(gen_json_types,
            gen->json_type));

    for (i = 0; i < nkey; i ++) {
        print_fmt("{\"name\":\"%zu\",\"type\":", i);

        if (depth > 1)
            gen_type_gen(
                gen, SIZE_DIV(nkey, gen->nkey_div),
                depth - 1);
        else
        if (gen->plain_types)
            print_fmt("{\"plain\":%zu}", i);
        else
            print_str("\"number\"");

        print_char('}');

        if (i < nkey - 1)
            print_char(',');
    }

    print_str("]}");
}

static void gen_keys_gen(
    const struct gen_t* gen,
    size_t keys[], size_t n)
{
    size_t i;

    for (i = 0; i < n; i ++)
        keys[i] = i;

    if (gen->randomize)
        shuffle(keys, n);
}

static void gen_value_gen(
    const struct gen_t* gen,
    size_t nkey, size_t depth)
{
    size_t s[nkey];
    size_t i;

    gen_keys_gen(gen, s, nkey);

    print_char('{');

    for (i = 0; i < nkey; i ++) {
        print_fmt("\"%zu\":", s[i]);

        if (depth > 1)
            gen_value_gen(
                gen, SIZE_DIV(nkey, gen->nkey_div),
                depth - 1);
        else
            print_fmt("%zu", s[i]);

        if (i < nkey - 1)
            print_char(',');
    }

    print_char('}');
}

#undef print_fmt
#undef print_str
#undef print_char

int main(int argc, char* argv[])
{
    typedef void (*gen_action_func_t)(
        const struct gen_t*, size_t, size_t);

    static const gen_action_func_t actions[] = {
        [options_gen_type_action]  = gen_type_gen,
        [options_gen_value_action] = gen_value_gen,
    };

    const struct options_t* opt = options(argc, argv);
    gen_action_func_t act = ARRAY_NULL_ELEM(
        actions, opt->action);
    struct gen_t gen;

    gen_init(&gen, opt);

    ASSERT(act != NULL);
    ASSERT(opt->nkey > 0);
    ASSERT(opt->depth > 0);
    act(&gen, opt->nkey, opt->depth);

    return 0;
}


