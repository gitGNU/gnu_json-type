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
#include <getopt.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

#include "common.h"
#include "ptr-traits.h"

#include "test-common.h"

const char stdin_name[] = "<stdin>";

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

static void version()
{
    fprintf(stdout, "%s: version %s\n", program, verdate);
}

static void usage()
{
    fprintf(stdout, help, program);
}

struct su_size_t su_size(size_t sz)
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

void options_invalid_opt_arg(const char* opt_name, const char* opt_arg)
{
    error("invalid argument for '%s' option: '%s'", opt_name, opt_arg);
}

void options_illegal_opt_arg(const char* opt_name, const char* opt_arg)
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

#if SIZE_MAX < ULONG_MAX
static size_t strtosz(
    const char* ptr, char** end, int base)
{
    unsigned long r;

    errno = 0;
    r = strtoul(ptr, end, base);

    if (errno == 0 && r > SIZE_MAX)
        errno = ERANGE;

    return r;
}
#define STR_TO_SIZE strtosz
#elif SIZE_MAX == ULONG_MAX
#define STR_TO_SIZE strtoul
#elif SIZE_MAX == UULONG_MAX
#define STR_TO_SIZE strtoull
#else
#error unexpected SIZE_MAX > UULONG_MAX
#endif

static size_t parse_num(
    const char* p, const char** q, int b)
{
    if (!isdigit(*p)) {
        *q = p;
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    return STR_TO_SIZE(p, (char**) q, b);
}

size_t options_parse_size_optarg(
    const char* opt_name, const char* opt_arg,
    size_t min, size_t max)
{
    const char *p, *q = NULL;
    size_t n, v, d;

    if (!(n = strlen(opt_arg)))
        options_invalid_opt_arg(opt_name, opt_arg);
    v = parse_num(p = opt_arg, &q, 10);
    d = PTR_OFFSET(q, p, n);
    if (errno || (d != n))
        options_invalid_opt_arg(opt_name, opt_arg);
    if ((min > 0 && v < min) || (max > 0 && v > max))
        options_illegal_opt_arg(opt_name, opt_arg);
    return v;
}

size_t options_parse_su_size_optarg(
    const char* opt_name, const char* opt_arg,
    size_t min, size_t max)
{
    const char *p, *q = NULL;
    size_t n, v, d;

    if (!(n = strlen(opt_arg)))
        options_invalid_opt_arg(opt_name, opt_arg);
    v = parse_num(p = opt_arg, &q, 10);
    d = PTR_OFFSET(q, p, n);
    if (errno ||
        (d == 0) ||
        (d < n - 1) ||
        (d == n - 1 && *q != 'k' && *q != 'K' &&
            *q != 'm' && *q != 'M'))
        options_invalid_opt_arg(opt_name, opt_arg);
    switch (*q) {
    case '\0':
        break;
    case 'k':
    case 'K':
        STATIC(KB(1) <= SIZE_MAX);
        if (!SIZE_MUL_NO_OVERFLOW(v, KB(1)))
            options_illegal_opt_arg(opt_name, opt_arg);
        v *= KB(1);
        break;
    case 'm':
    case 'M':
        STATIC(MB(1) <= SIZE_MAX);
        if (!SIZE_MUL_NO_OVERFLOW(v, MB(1)))
            options_illegal_opt_arg(opt_name, opt_arg);
        v *= MB(1);
        break;
    }
    if ((min > 0 && v < min) || (max > 0 && v > max))
        options_illegal_opt_arg(opt_name, opt_arg);
    return v;
}

void options_parse_args(void* opts,
    const struct options_funcs_t* funcs,
    const char* shorts, size_t n_shorts,
    struct option* longs, size_t n_longs,
    int argc, char* argv[])
{
    struct bits_opts_t
    {
        bits_t dump: 1;
        bits_t usage: 1;
        bits_t version: 1;
    };
    struct bits_opts_t bits = {
        .dump    = false,
        .usage   = false,
        .version = false
    };
    enum {
        help_opt      = '?',
        dump_opts_opt = 128,
        version_opt,
    };
    static struct option AL[] = {
        { "dump-options", 0,       0, dump_opts_opt },
        { "version",      0,       0, version_opt },
        { "help",         0, &optopt, help_opt },
        { 0,              0,       0, 0 }
    };
    static const char AS[] = ":";

    const size_t L = ARRAY_SIZE(AL);
    const size_t S = ARRAY_SIZE(AS);
    const size_t n = sizeof(struct option);
    struct option OL[n_longs + L];
    char OS[n_shorts + S];
    struct option* l;
    char* s;
    int opt;

    ASSERT_SIZE_MUL_NO_OVERFLOW(n, n_longs);
    memcpy(l = OL, longs, n * n_longs);

    ASSERT_SIZE_MUL_NO_OVERFLOW(n, L);
    memcpy(l += n_longs, AL, n * L);

    ASSERT_SIZE_DEC_NO_OVERFLOW(S);
    memcpy(s = OS, AS, S - 1);
    memcpy(s += S - 1, shorts, n_shorts);
    s[n_shorts] = '\0';

#define call_func(f, ...)               \
    ({                                  \
        ASSERT(opts);                   \
        ASSERT(funcs->f);               \
        funcs->f(opts, ## __VA_ARGS__); \
    })

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
    while ((opt = getopt_long(argc, argv, OS, OL, 0)) != EOF) {
        switch (opt) {
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
        inv_opt:
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
        default:
            if (!call_func(parse, opt, optarg))
                goto inv_opt;
            break;
        }
    }

    call_func(done, INT_AS_SIZE(optind));

    if (bits.version)
        version();
    if (bits.dump)
        call_func(dump);
    if (bits.usage)
        usage();

    if (bits.dump ||
        bits.version ||
        bits.usage)
        exit(0);
}


