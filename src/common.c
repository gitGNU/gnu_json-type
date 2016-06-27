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
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <ctype.h>

#include "lib/json.h"
#include "lib/pretty-print.h"

#include "common.h"

#ifdef DEBUG
#define PRINT_DEBUG_COND true
#include "debug.h"
#endif

#define STRINGIFY_(s) #s
#define STRINGIFY(s)  STRINGIFY_(s)

static const char program[] = STRINGIFY(PROGRAM);
static const char library[] = STRINGIFY(LIBRARY);
static const char verdate[] = "0.8 -- 2016-06-27 20:05"; // $ date +'%F %R'

static const char license[] =
"Copyright (C) 2016  Stefan Vargyas.\n"
"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n"
"This is free software: you are free to change and redistribute it.\n"
"There is NO WARRANTY, to the extent permitted by law.\n";

const char stdin_name[] = "<stdin>";

typedef char fmt_buf_t[512];

enum error_type_t { FATAL, WARNING, ERROR };

static void verror(
    enum error_type_t err, const char* fmt, bool nl,
    va_list arg)
{
    static const char* const errors[] = {
        [FATAL]   = "fatal error",
        [WARNING] = "warning",
        [ERROR]   = "error",
    };
    static const char* const fmts[] = {
        "%s: %s: %s\n",
        "%s: %s: %s"
    };
    fmt_buf_t b;

    vsnprintf(b, sizeof b - 1, fmt, arg);
    b[sizeof b - 1] = 0;

    fprintf(stderr, fmts[!nl], program, errors[err], b);
}

void fatal_error(const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    verror(FATAL, fmt, true, arg);
    va_end(arg);

    exit(127);
}

void assert_failed(
    const char* file, int line, const char* func,
    const char* expr)
{
    fatal_error(
        "assertion failed: %s:%d:%s: %s",
        file, line, func, expr);
}

void unexpect_error(
    const char* file, int line, const char* func,
    const char* msg, ...)
{
    va_list arg;
    fmt_buf_t b;

    va_start(arg, msg);
    vsnprintf(b, sizeof b - 1, msg, arg);
    va_end(arg);

    b[sizeof b - 1] = 0;

    fatal_error(
        "unexpected error: %s:%d:%s: %s",
        file, line, func, b);
}

void warning(const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    verror(WARNING, fmt, true, arg);
    va_end(arg);
}

void error(const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    verror(ERROR, fmt, true, arg);
    va_end(arg);

    exit(1);
}

static void error_fmt(bool nl, const char* fmt, ...)
    PRINTF(2);

static void error_fmt(bool nl, const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    verror(ERROR, fmt, nl, arg);
    va_end(arg);
}

void oom_error(const char* fmt, ...)
{
    va_list arg;
    fmt_buf_t b;

    va_start(arg, fmt);
    vsnprintf(b, sizeof b - 1, fmt, arg);
    va_end(arg);

    b[sizeof b - 1] = 0;

    fatal_error("out of memory: %s", b);
}

void sys_error(const char* fmt, ...)
{
    int sys = errno;
    va_list arg;
    fmt_buf_t b;

    va_start(arg, fmt);
    vsnprintf(b, sizeof b - 1, fmt, arg);
    va_end(arg);

    b[sizeof b - 1] = 0;

    error("%s: %s", b, strerror(sys));
}

void pos_error_header(
    const char* file, size_t line, size_t col)
{
    error_fmt(
        false,
        line && col ? "%s:%zu:%zu: " : "%s: ",
        file ? file : stdin_name,
        line, col);
}

void pos_error(
    const char* file, size_t line, size_t col,
    const char* fmt, ...)
{
    va_list arg;

    pos_error_header(file, line, col);

    va_start(arg, fmt);
    vfprintf(stderr, fmt, arg);
    va_end(arg);

    fputc('\n', stderr);
}

bool mem_find(
    const uchar_t* buf, size_t len, char ch,
    size_t* pos)
{
    const uchar_t* p;

    if ((p = memchr(buf, ch, len)) != NULL) {
        *pos = PTR_OFFSET(p, buf, len);
        return true;
    }
    return false;
}

bool mem_rfind(
    const uchar_t* buf, size_t len, char ch,
    size_t* pos)
{
    const uchar_t* p;

    if ((p = memrchr(buf, ch, len)) != NULL) {
        *pos = PTR_OFFSET(p, buf, len);
        return true;
    }
    return false;
}

size_t mem_count(
    const uchar_t* buf, size_t len, char ch)
{
    const uchar_t *b = buf, *p;
    size_t l = len, r = 0;

    while ((p = memchr(b, ch, l)) != NULL) {
        size_t d = PTR_OFFSET(p, b, l);

        b += ++ d;
        l -= d;

        ASSERT(PTR_IS_IN_BUF(b, l, buf, len));

        r ++;
    }

    return r;
}

bool mem_find_nth(
    const uchar_t* buf, size_t len, char ch,
    size_t n, size_t* pos)
{
    const uchar_t *b = buf, *p;
    size_t l = len, d;

    ASSERT(n > 0);
    while ((p = memchr(b, ch, l)) != NULL) {
        if (-- n == 0) {
            *pos = PTR_OFFSET(p, buf, len);
            return true;
        }
        d = PTR_OFFSET(p, b, l);

        b += ++ d;
        l -= d;

        ASSERT(PTR_IS_IN_BUF(b, l, buf, len));
    }

    return false;
}

struct lib_version_t
{
    unsigned short major;
    unsigned short minor;
    unsigned short patch;
    const char*    build;
};

static struct lib_version_t lib_version()
{
    enum { M = 10000, m = 100 };
    struct lib_version_t r;
    const char* b;
    size_t v;

    v = json_version();
    ASSERT(v > 0 && v < M * 10);

    r.major = v / M;
    v %= M;
    r.minor = v / m;
    r.patch = v % m;

    b = json_build_datetime();
    ASSERT(b != NULL);
    r.build = b;

    return r;
}

static void lib_version_check()
{
    struct lib_version_t lib;

    lib = lib_version();
    if (lib.major == JSON_VERSION_MAJOR &&
        lib.minor == JSON_VERSION_MINOR)
        return;

    fatal_error(
        "json library is not of expected "
        "version %d.%d but %d.%d.%d",
        JSON_VERSION_MAJOR,
        JSON_VERSION_MINOR,
        lib.major, lib.minor,
        lib.patch);
}

#if JSON_VERSION <= 0 || JSON_VERSION >= 100000
#error "JSON_VERSION has an invalid value"
#endif

static void version()
{
    struct lib_version_t v;

    v = lib_version();
    fprintf(stdout, 
        "%s: version %s\n"
        "%s: %s: version: %u.%u.%u\n"
        "%s: %s: build: %s\n"
        "%s: %s: JSON_DEBUG is %sabled\n\n%s",
        program, verdate,
        program, library, v.major, v.minor, v.patch,
        program, library, v.build,
        program, library,
#ifdef JSON_DEBUG
        "en",
#else
        "dis",
#endif
        license
    );
}

#define KB(X) (SZ(X) * SZ(1024))
#define MB(X) (KB(X) * SZ(1024))

#define PARSE_BUF_SIZE_OPTARG   parse_su_size_optarg
#define PARSE_STACK_SIZE_OPTARG parse_su_size_optarg
#define PARSE_POOL_SIZE_OPTARG  parse_su_size_optarg
#define PARSE_WIDTH_SIZE_OPTARG parse_size_optarg

#define PARSE_SIZE_OPTARG(name, type, min, max) \
    PARSE_ ## type ## _SIZE_OPTARG(             \
        "sizes-" name, optarg, min, max)

#define SU_SIZE_(n) \
    struct su_size_t n ## _su = su_size(opts->n);

#define SU_BUF_SIZE      SU_SIZE_
#define SU_STACK_SIZE    SU_SIZE_
#define SU_POOL_SIZE     SU_SIZE_
#define SU_WIDTH_SIZE(n)

#define SU_SIZE(id, type) SU_ ## type ## _SIZE(sizes_ ## id)

#define FMT_SU_SIZE    "%zu%s"
#define FMT_BUF_SIZE   FMT_SU_SIZE
#define FMT_STACK_SIZE FMT_SU_SIZE
#define FMT_POOL_SIZE  FMT_SU_SIZE
#define FMT_WIDTH_SIZE "%zu"

#define FMT_SIZE(type) FMT_ ## type ## _SIZE

#define ARG_SU_SIZE(n)    n ## _su.sz, n ## _su.su
#define ARG_BUF_SIZE      ARG_SU_SIZE
#define ARG_STACK_SIZE    ARG_SU_SIZE
#define ARG_POOL_SIZE     ARG_SU_SIZE
#define ARG_WIDTH_SIZE(n) opts->n

#define ARG_SIZE(id, type) ARG_ ## type ## _SIZE(sizes_ ## id),

#define HELP_(h)
#define HELP__(h) h "\n"

#define HELP_b HELP__
#define HELP_e HELP__

#define HELP_LETTER(help, letter) \
    HELP_ ## letter(help)

#define STRUCT_(o, n)
#define STRUCT__(o, n) { o, 1, 0, sizes_ ## n ## _opt },

#define STRUCT_b STRUCT__
#define STRUCT_e STRUCT__

#define STRUCT_LETTER(name, id, letter) \
    STRUCT_ ## letter(name, id)

#define ARG_(l)
#define ARG__(l) #l ":"

#define ARG_b ARG__
#define ARG_e ARG__

#define ARG_LETTER(letter) ARG_ ## letter(letter)

#define CH_b 'b'
#define CH_e 'e'
#define CH(l) CH_ ## l

#define LETTER_(n, l)
#define LETTER__(n, l) n = CH(l),

#define LETTER_b LETTER__
#define LETTER_e LETTER__

#define LETTER(n, l) LETTER_ ## l(sizes_ ## n, l)

#define OPT_LETTER(id, letter) \
    LETTER(id ## _opt, letter)

#define NON_LETTER_(n) n,
#define NON_LETTER_b(n)
#define NON_LETTER_e(n)

#define NON_LETTER(n, l) NON_LETTER_ ## l(sizes_ ## n)

#define OPT_NON_LETTER(id, letter) \
    NON_LETTER(id ## _opt, letter)

static void usage()
{
    fprintf(stdout,
        "usage: %s [ACTION|OPTION]... [FILE]\n"
        "where actions are specified as:\n"
        "  -O|--parse-only              run 'json-parser': only parse the input or\n"
        "  -E|--echo-parse                parse and echo out verbatim the input\n"
        "  -P|--pretty-print            run 'json-printer': reprint prettily or\n"
        "  -R|--terse-print               tersely the input JSON objects; otherwise\n"
        "  -Y|--type-print                build the AST of the JSON input and print\n"
        "  -A|--ast-print                 out either its type object or the AST or\n"
        "  -F|--from-ast-print            reprint prettily from AST the JSON input\n"
        "  -T|--typelib-func [NAME]     run 'json-typelib' on the input typelib for\n"
        "                                 the named function:\n"
        "                                   v|validate    check the validity of input;\n"
        "                                   p|print       print out the constructed type;\n"
        "                                   a|attr        print out the constructed type\n"
        "                                                 along with generated attributes;\n"
        "                                   c|check-attr  check the constructed type for to\n"
        "                                                 validate the generated attributes\n"
        "                                   A|print-check-attr\n"
        "                                                 print out the constructed type\n"
        "                                                 along with generated attributes,\n"
        "                                                 then validate these attributes\n"
        "                                   d|gen-def     generate the type def code for\n"
        "                                                 the constructed type\n"
        "                                 the default is validate\n"
        "  -J|--json2                   run 'json2' (default)\n"
//!!!2JSON		"  -2|--2json                 run '2json'\n"
        "the options are:\n"
        "  -d|--type-def TYPE           use the specified type definition or library for\n"
        "  -t|--type-lib FILE[:NAME]      type checking the JSON input or, otherwise, do\n"
        "  -N|--type-name [NAME]          not check the input for types at all (default);\n"
        "     --no-type-check             the optional name specifies which type to use\n"
        "                                 in case the def/library is made of an array\n"
        "                                 of named types instead of just a sole type;\n"
        "                                 type checking doesn't apply yet to the AST\n"
        "                                 options `-Y|--type-print', `-A|--ast-print'\n"
        "                                 and `-F|--from-ast-print'\n"
        "  -i|--input-file FILE         take input from the named file (default: '-')\n"
#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def) \
        HELP_LETTER(help, letter)
#include "opt-sizes.def"
        "  -l|--[no-]liter[al]-value    allow literal values on input or otherwise\n"
        "                                 do not (default not)\n"
        "  -u|--[no-]valid[ate]-utf8    validate non-ascii chars in strings for UTF8\n"
        "                                 encoding or otherwise not (default not)\n"
        "  -a|--[no-]ascii-only         enforce JSON strings to contain ASCII chars\n"
        "                                 only or otherwise do not (default not)\n"
        "  -m|--[no-]multi-object       allow multiple JSON objects on input or\n"
        "                                 otherwise do not (default not)\n"
        "  -o|--[no-]print-dots         put indenting dots in structure print outs\n"
        "                                 or otherwise do not (default not)\n"
        "     --[no-]newline-sep        separate the top values on terse JSON print outs\n"
        "                                 by newline or otherwise do not (default not)\n"
        "     --[no-]error              print out error reports on stderr or otherwise\n"
        "                                 do not (default do); note that reporting the\n"
        "                                 fatal errors on stderr is never suppressed\n"
        "     --sizes-NAME VALUE        assign the named sizes parameter the given value;\n"
        "                                 use `--help-sizes' option to obtain the list of\n"
        "                                 all the sizes parameters along with the minimum,\n"
        "                                 the maximum and the default values and a short\n"
        "                                 description attached\n"
#ifdef DEBUG
        "     --debug-CLASS [LEVEL]     do print some debugging output for the named\n"
        "                                 class, which can be any of: base, printer,\n"
        "                                 obj, ast, type, type-lib or type-ruler;\n"
        "                                 the level can be [0-9], 1 being the default\n"
        "     --no-debug                do not print debugging output at all (default)\n"
#endif
        "     --dump-options            print options and exit\n"
        "  -V|--[no-]verbose            be verbose or otherwise do not (default not)\n"
        "     --help-sizes              print info about the sizes parameters and exit\n"
        "  -v|--version                 print version numbers and exit\n"
        "  -?|--help                    display this help info and exit\n",
        program);
}

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

static size_t null_size_to_string(
    char* buf, size_t len)
{
    ASSERT(len >= 2);
    strncpy(buf, "-", 2);
    return 1;
} 

static size_t su_size_to_string(
    char* buf, size_t len, size_t val, bool null)
{
    if (val == 0 && null)
        return null_size_to_string(buf, len);
    else {
        struct su_size_t s;
        size_t r;
        int n;

        s = su_size(val);
        n = snprintf(buf, len, "%zu%s", s.sz, s.su);
        r = INT_AS_SIZE(n);

        ASSERT(n >= 0);
        ASSERT(r < len);

        return r;
    }
}

static size_t size_to_string(
    char* buf, size_t len, size_t val, bool null)
{
    if (val == 0 && null)
        return null_size_to_string(buf, len);
    else {
        size_t r;
        int n;

        n = snprintf(buf, len, "%zu", val);
        r = INT_AS_SIZE(n);

        ASSERT(n >= 0);
        ASSERT(r < len);

        return r;
    }
}

static void usage_sizes()
{
    enum sizes_param_type_t {
        NONE, BUF, STACK, POOL, WIDTH
    };
    typedef size_t (*to_string_func_t)(
        char*, size_t, size_t, bool);
    static const to_string_func_t to_strings[] = {
        [BUF]   = su_size_to_string,
        [STACK] = su_size_to_string,
        [POOL]  = su_size_to_string,
        [WIDTH] = size_to_string,
    };
    struct sizes_param_t
    {
        enum sizes_param_type_t
                    type;
        const char* name;
        const char* desc;
        size_t      min;
        size_t      max;
        size_t      def;
    };
    static const struct sizes_param_t params[] = {
        { NONE, "name", "description", 0, 0, 0 },
#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def) \
        { type, name, desc, min, max, def },
#include "opt-sizes.def"
    };
    const size_t m = TYPE_DIGITS10(size_t) + 1;
    struct sizes_values_t
    {
        char min[m];
        char max[m];
        char def[m];
    };
    const size_t n = ARRAY_SIZE(params);
    struct sizes_values_t values[n];
    struct sizes_max_widths_t
    {
        size_t name;
        size_t min;
        size_t max;
        size_t def;
    };
    struct sizes_max_widths_t w;
    const struct sizes_param_t *p, *e;
    struct sizes_values_t *v, *f;

    memset(values, 0, sizeof(values));
    memset(&w, 0, sizeof(w));

    for (p = params,
         v = values,
         e = params + n,
         f = values + n;
         p < e && v < f;
         p ++,
         v ++) {
        size_t l;

        if (v == values) {
            ASSERT(m >= 8);

            strncpy(v->min, "min", 4);
            strncpy(v->max, "max", 4);
            strncpy(v->def, "default", 8);
        }
        else {
            to_string_func_t f =
                ARRAY_NULL_ELEM(to_strings, p->type);
            ASSERT(f != NULL);

            f(v->min, m, p->min, true);
            f(v->max, m, p->max, true);
            f(v->def, m, p->def, false);
        }

        l = strlen(p->name);
        if (w.name < l)
            w.name = l;

        l = strlen(v->min);
        if (w.min < l)
            w.min = l;

        l = strlen(v->max);
        if (w.max < l)
            w.max = l;

        l = strlen(v->def);
        if (w.def < l)
            w.def = l;
    }

    for (p = params,
         v = values,
         e = params + n,
         f = values + n;
         p < e && v < f;
         p ++,
         v ++) {
        fprintf(stdout,
            "%-*s  %*s  %*s  %*s  %s\n",
            SIZE_AS_INT(w.name), p->name,
            SIZE_AS_INT(w.min), v->min,
            SIZE_AS_INT(w.max), v->max,
            SIZE_AS_INT(w.def), v->def,
            p->desc);
    }
}

static void dump_options(const struct options_t* opts)
{
    static const char* const objects[] = {
        [OBJ_JSON_PARSER]  = "json-parser",
        [OBJ_JSON_PRINTER] = "json-printer",
        [OBJ_JSON_TYPELIB] = "json-typelib",
        [OBJ_JSON2]        = "json2"
//!!!2JSON		[OBJ_2JSON]        = "2json"
    };
#undef  CASE
#define CASE(n) [options_ ## n ## _parse_type] = #n "-parse"
    static const char* const parse_types[] = {
        CASE(echo),
        CASE(quiet),
    };
#undef  CASE
#define CASE2(n, s) [options_ ## n ## _print_type] = s "-print"
#define CASE(n)     CASE2(n, #n)
    static const char* const print_types[] = {
        CASE2(from_ast, "from-ast"),
        CASE(pretty),
        CASE(terse),
        CASE(type),
        CASE(ast)
    };
#undef  CASE
#undef  CASE2
#define CASE2(n, s) [options_ ## n ## _typelib_func] = s
#define CASE(n)     CASE2(n, #n)
    static const char* const typelib_funcs[] = {
        CASE(validate),
        CASE(print),
        CASE(attr),
        CASE(check),
        CASE2(print_check, "print-check"),
        CASE(def),
    };
#undef  CASE
#define CASE(n) [options_type_check_ ## n ## _type] = #n
    static const char* const type_checks[] = {
        CASE(none),
        CASE(def),
        CASE(lib)
    };
    static const char* const noyes[] = {
        [0] = "no",
        [1] = "yes"
    };
#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def) \
    SU_SIZE(id, type)
#include "opt-sizes.def"
    char* const* p;
    size_t i;

    fprintf(stdout,
        "input:                         %s\n"
        "object:                        %s\n"
        "parse-type:                    %s\n"
        "print-type:                    %s\n"
        "typelib-func:                  %s\n"
        "type-check:                    %s\n"
        "type-def:                      %s\n"
        "type-lib:                      %s\n"
        "type-name:                     %s\n"
        "quote-cntrl:                   %s\n"
        "liter-value:                   %s\n"
        "valid-utf8:                    %s\n"
        "ascii-only:                    %s\n"
        "multi-obj:                     %s\n"
        "print-dots:                    %s\n"
        "newline-sep:                   %s\n"
        "no-error:                      %s\n"
        "verbose:                       %s\n"
#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def) \
        "sizes-" name ":" pad " " FMT_SIZE(type) "\n"
#include "opt-sizes.def"
#ifdef DEBUG
        "debug-base:                    %d\n"
        "debug-printer:                 %d\n"
        "debug-obj:                     %d\n"
        "debug-ast:                     %d\n"
        "debug-type:                    %d\n"
        "debug-type-lib:                %d\n"
        "debug-type-ruler:              %d\n"
#endif
        "argc:                          %zu\n",
        opts->input ? opts->input : stdin_name,
#define NAME0(X, T)                  \
    ({                               \
        size_t __v = opts->X;        \
        ARRAY_NON_NULL_ELEM(T, __v); \
    })
#define NAME(X)  (NAME0(X, X ## s))
#define NNUL(X)  (opts->X ? opts->X : "-")
#define NOYES(X) (NAME0(X, noyes))
        NAME(object),
        NAME(parse_type),
        NAME(print_type),
        NAME(typelib_func),
        NAME(type_check),
        NNUL(type_def),
        NNUL(type_lib),
        NNUL(type_name),
        NOYES(quote_cntrl),
        NOYES(liter_value),
        NOYES(valid_utf8),
        NOYES(ascii_only),
        NOYES(multi_obj),
        NOYES(print_dots),
        NOYES(newline_sep),
        NOYES(no_error),
        NOYES(verbose),
#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def) \
        ARG_SIZE(id, type)
#include "opt-sizes.def"
#ifdef DEBUG
        opts->debug_base,
        opts->debug_printer,
        opts->debug_obj,
        opts->debug_ast,
        opts->debug_type,
        opts->debug_type_lib,
        opts->debug_type_ruler,
#endif
#undef  NOYES
#undef  NNUL
#undef  NAME
#undef  NAME0
        opts->argc);

    for (i = 0,
         p = opts->argv;
         i < opts->argc;
         i ++,
         p ++) {
        const size_t m = TYPE_DIGITS10(size_t) + 1;
        const size_t w = 24;
        char b[m];
        size_t n;

        n = size_to_string(b, m, i, false);
        ASSERT(n < m);

        fprintf(stdout,
            "argv[%s]:%*s%s\n",
            b, w > n ? SIZE_AS_INT(w - n) : 0, "", *p);
    }
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

static const char* parse_typelib_optarg(
    const char* opt_name, char* opt_arg,
    const char** type_name)
{
    char* p = opt_arg;
    if ((*p == '\0') || (*p ++ == '-' && *p == '\0'))
        illegal_opt_arg(opt_name, opt_arg);

    if ((p = strrchr(opt_arg, ':'))) {
        *p ++ = 0;
        *type_name = p;
    }

    return opt_arg;
}

// $ . ~/lookup-gen/commands.sh
// $ print() { printf '%s\n' "$@"; }
// $ print 'v validate =options_validate_typelib_func' 'p print =options_print_typelib_func' 'a attr =options_attr_typelib_func' 'c check check-attr =options_check_typelib_func' 'A print-check print-check-attr =options_print_check_typelib_func' 'd gen-def =options_def_typelib_func'|gen-func -f lookup_typelib_func_name|ssed -R '1s/^/static /;1s/\s*result_t\&/\n\tenum options_typelib_func_t*/;s/(?=t =)/*/;s/result_t:://'

static bool lookup_typelib_func_name(const char* n,
    enum options_typelib_func_t* t)
{
    // pattern: A|a[ttr]|c[heck[-attr]]|d|gen-def|p[rint[-check[-attr]]]|v[alidate]
    switch (*n ++) {
    case 'A':
        if (*n == 0) {
            *t = options_print_check_typelib_func;
            return true;
        }
        return false;
    case 'a':
        if (*n == 0) {
            *t = options_attr_typelib_func;
            return true;
        }
        if (*n ++ == 't' &&
            *n ++ == 't' &&
            *n ++ == 'r' &&
            *n == 0) {
            *t = options_attr_typelib_func;
            return true;
        }
        return false;
    case 'c':
        if (*n == 0) {
            *t = options_check_typelib_func;
            return true;
        }
        if (*n ++ == 'h' &&
            *n ++ == 'e' &&
            *n ++ == 'c' &&
            *n ++ == 'k') {
            if (*n == 0) {
                *t = options_check_typelib_func;
                return true;
            }
            if (*n ++ == '-' &&
                *n ++ == 'a' &&
                *n ++ == 't' &&
                *n ++ == 't' &&
                *n ++ == 'r' &&
                *n == 0) {
                *t = options_check_typelib_func;
                return true;
            }
        }
        return false;
    case 'd':
        if (*n == 0) {
            *t = options_def_typelib_func;
            return true;
        }
        return false;
    case 'g':
        if (*n ++ == 'e' &&
            *n ++ == 'n' &&
            *n ++ == '-' &&
            *n ++ == 'd' &&
            *n ++ == 'e' &&
            *n ++ == 'f' &&
            *n == 0) {
            *t = options_def_typelib_func;
            return true;
        }
        return false;
    case 'p':
        if (*n == 0) {
            *t = options_print_typelib_func;
            return true;
        }
        if (*n ++ == 'r' &&
            *n ++ == 'i' &&
            *n ++ == 'n' &&
            *n ++ == 't') {
            if (*n == 0) {
                *t = options_print_typelib_func;
                return true;
            }
            if (*n ++ == '-' &&
                *n ++ == 'c' &&
                *n ++ == 'h' &&
                *n ++ == 'e' &&
                *n ++ == 'c' &&
                *n ++ == 'k') {
                if (*n == 0) {
                    *t = options_print_check_typelib_func;
                    return true;
                }
                if (*n ++ == '-' &&
                    *n ++ == 'a' &&
                    *n ++ == 't' &&
                    *n ++ == 't' &&
                    *n ++ == 'r' &&
                    *n == 0) {
                    *t = options_print_check_typelib_func;
                    return true;
                }
            }
        }
        return false;
    case 'v':
        if (*n == 0) {
            *t = options_validate_typelib_func;
            return true;
        }
        if (*n ++ == 'a' &&
            *n ++ == 'l' &&
            *n ++ == 'i' &&
            *n ++ == 'd' &&
            *n ++ == 'a' &&
            *n ++ == 't' &&
            *n ++ == 'e' &&
            *n == 0) {
            *t = options_validate_typelib_func;
            return true;
        }
    }
    return false;
}

static enum options_typelib_func_t parse_typelib_func_optarg(
    const char* opt_name, const char* opt_arg)
{
    enum options_typelib_func_t r;

    if (opt_arg == NULL)
        return options_validate_typelib_func;
    if (!lookup_typelib_func_name(opt_arg, &r))
        invalid_opt_arg(opt_name, opt_arg);

    return r;
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
    const char* opt_name, const char* opt_arg,
    size_t min, size_t max)
{
    const char *p, *q = NULL;
    size_t n, v, d;

    if (!(n = strlen(opt_arg)))
        invalid_opt_arg(opt_name, opt_arg);
    v = parse_num(p = opt_arg, &q, 10);
    d = PTR_OFFSET(q, p, n);
    if (errno || (d != n))
        invalid_opt_arg(opt_name, opt_arg);
    if ((min > 0 && v < min) ||
        (max > 0 && v > max))
        illegal_opt_arg(opt_name, opt_arg);
    return v;
}

static size_t parse_su_size_optarg(
    const char* opt_name, const char* opt_arg,
    size_t min, size_t max)
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
        if (!SIZE_MUL_NO_OVERFLOW(v, KB(1)))
            illegal_opt_arg(opt_name, opt_arg);
        v *= KB(1);
        break;
    case 'm':
    case 'M':
        STATIC(MB(1) <= SIZE_MAX);
        if (!SIZE_MUL_NO_OVERFLOW(v, MB(1)))
            illegal_opt_arg(opt_name, opt_arg);
        v *= MB(1);
        break;
    }
    if ((min > 0 && v < min) ||
        (max > 0 && v > max))
        illegal_opt_arg(opt_name, opt_arg);
    return v;
}

#ifdef DEBUG
static size_t parse_debug_optarg(
    const char* opt_name, const char* opt_arg)
{
    if (opt_arg == NULL)
        return 1;
    else {
        if (!isdigit(opt_arg[0]) || opt_arg[1] != '\0')
            invalid_opt_arg(opt_name, opt_arg);
        return *opt_arg - '0';
    }
}
#endif

#if defined(DEBUG) && defined(DEBUG_GETOPT_LONG)

static void print_getopt_long_debug(int opt)
{
    fprintf(stderr, "optind=%d optarg=", optind);
    if (optarg)
        pretty_print_string(stderr, (const uchar_t*) optarg,
            strlen(optarg), pretty_print_string_quotes);
    else
        fputs("(nil)", stderr);
    fprintf(stderr, " opt=%d opt=", opt);
    pretty_print_char(stderr, opt, pretty_print_char_quotes);
    fprintf(stderr, " optopt=%d optopt=", optopt);
    pretty_print_char(stderr, optopt, pretty_print_char_quotes);
}

#define PRINT_GETOPT_LONG_DEBUG()                  \
    do {                                           \
        PRINT_DEBUG_BEGIN_UNCOND("getopt_long: "); \
        print_getopt_long_debug(opt);              \
        PRINT_DEBUG_END_UNCOND();                  \
    } while (0)

#endif

const struct options_t* options(int argc, char* argv[])
{
    static struct options_t opts = {
        .input            = NULL,
        .object           = OBJ_JSON2,
        .parse_type       = options_quiet_parse_type,
        .print_type       = options_pretty_print_type,
        .typelib_func     = options_validate_typelib_func,
        .type_check       = options_type_check_none_type,
        .type_def         = NULL,
        .type_lib         = NULL,
        .type_name        = NULL,
        .quote_cntrl      = false,
        .liter_value      = false,
        .valid_utf8       = false,
        .ascii_only       = false,
        .multi_obj        = false,
        .print_dots       = false,
        .newline_sep      = false,
        .no_error         = false,
        .verbose          = false,
#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def) \
        .sizes_ ## id = def,
#include "opt-sizes.def"
#ifdef DEBUG
        .debug_base       = 0,
        .debug_printer    = 0,
        .debug_obj        = 0,
        .debug_ast        = 0,
        .debug_type       = 0,
        .debug_type_lib   = 0,
        .debug_type_ruler = 0,
#endif
        .argc             = 0,
        .argv             = NULL
    };

    enum {
        // stev: actions:
        quiet_parser_act     = 'O',
        echo_parser_act      = 'E',
        from_ast_printer_act = 'F',
        pretty_printer_act   = 'P',
        terse_printer_act    = 'R',
        type_printer_act     = 'Y',
        ast_printer_act      = 'A',
        json2_act            = 'J',
        //!!!2JON two_json_act = '2',
        typelib_func_act     = 'T',

        // stev: options:
        type_def_opt         = 'd',
        type_lib_opt         = 't',
        type_name_opt        = 'N',
        input_file_opt       = 'i',
        quote_cntrl_opt      = 'q',
        liter_value_opt      = 'l',
        valid_utf8_opt       = 'u',
        ascii_only_opt       = 'a',
        multi_obj_opt        = 'm',
        print_dots_opt       = 'o',
        version_opt          = 'v',
        verbose_opt          = 'V',
        help_opt             = '?',

#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def) \
        OPT_LETTER(id, letter)
#include "opt-sizes.def"

        newline_sep_opt      = 128,
        no_error_opt,
        dump_opts_opt,

#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def) \
        OPT_NON_LETTER(id, letter)
#include "opt-sizes.def"

        no_type_check_opt,
        no_quote_cntrl_opt,
        no_liter_value_opt,
        no_valid_utf8_opt,
        no_ascii_only_opt,
        no_multi_obj_opt,
        no_print_dots_opt,
        no_newline_sep_opt,
        no_no_error_opt,
        no_verbose_opt,

#ifdef DEBUG
        no_debug_opt,

        debug_base_opt,
        debug_printer_opt,
        debug_obj_opt,
        debug_ast_opt,
        debug_type_opt,
        debug_type_lib_opt,
        debug_type_ruler_opt,
#endif
        help_sizes_opt,
    };

    static struct option long_opts[] = {
        { "parse-only",             0,       0, quiet_parser_act },
        { "echo-parse",             0,       0, echo_parser_act },
        { "from-ast-print",         0,       0, from_ast_printer_act },
        { "pretty-print",           0,       0, pretty_printer_act },
        { "terse-print",            0,       0, terse_printer_act },
        { "type-print",             0,       0, type_printer_act },
        { "ast-print",              0,       0, ast_printer_act },
        { "typelib-func",           2,       0, typelib_func_act },
        { "json2",                  0,       0, json2_act },
//!!!2JSON		{ "2json",                  0,       0, two_json_act },

        { "type-def",               1,       0, type_def_opt },
        { "type-lib",               1,       0, type_lib_opt },
        { "type-name",              2,       0, type_name_opt },
        { "no-type-check",          0,       0, no_type_check_opt },
        { "input-file",             1,       0, input_file_opt },
        { "quote-control-chars",    0,       0, quote_cntrl_opt },
        { "no-quote-control-chars", 0,       0, no_quote_cntrl_opt },
        { "liter-value",            0,       0, liter_value_opt },
        { "no-liter-value",         0,       0, no_liter_value_opt },
        { "literal-value",          0,       0, liter_value_opt },
        { "no-literal-value",       0,       0, no_liter_value_opt },
        { "valid-utf8",             0,       0, valid_utf8_opt },
        { "no-valid-utf8",          0,       0, no_valid_utf8_opt },
        { "validate-utf8",          0,       0, valid_utf8_opt },
        { "no-validate-utf8",       0,       0, no_valid_utf8_opt },
        { "ascii-only",             0,       0, ascii_only_opt },
        { "no-ascii-only",          0,       0, no_ascii_only_opt },
        { "multi-object",           0,       0, multi_obj_opt },
        { "no-multi-object",        0,       0, no_multi_obj_opt },
        { "print-dots",             0,       0, print_dots_opt },
        { "no-print-dots",          0,       0, no_print_dots_opt },
        { "dots",                   0,       0, print_dots_opt },
        { "no-dots",                0,       0, no_print_dots_opt },
        { "newline-sep",            0,       0, newline_sep_opt },
        { "no-newline-sep",         0,       0, no_newline_sep_opt },
        { "dump-options",           0,       0, dump_opts_opt },
        { "version",                0,       0, version_opt },
        { "error",                  0,       0, no_no_error_opt },
        { "no-error",               0,       0, no_error_opt },
        { "verbose",                0,       0, verbose_opt },
        { "no-verbose",             0,       0, no_verbose_opt },
#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def)    \
        { "sizes-" name,            1,       0, sizes_ ## id ## _opt }, \
        STRUCT_LETTER(name, id, letter)
#include "opt-sizes.def"
#ifdef DEBUG
        { "debug-base",             2,       0, debug_base_opt },
        { "debug-printer",          2,       0, debug_printer_opt },
        { "debug-obj",              2,       0, debug_obj_opt },
        { "debug-ast",              2,       0, debug_ast_opt },
        { "debug-type",             2,       0, debug_type_opt },
        { "debug-type-lib",         2,       0, debug_type_lib_opt },
        { "debug-type-ruler",       2,       0, debug_type_ruler_opt },
        { "no-debug",               0,       0, no_debug_opt },
#endif
        { "help-sizes",             0,       0, help_sizes_opt },
        { "help",                   0, &optopt, help_opt },
        { 0,                        0,       0, 0 }
    };
    static const char short_opts[] =
        ":"
        "AEFJOPRT::Y"
#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def) \
        ARG_LETTER(letter)
#include "opt-sizes.def"
        "ad:i:lmN::oqt:uvV";

    struct bits_opts_t {
        bits_t dump: 1;
        bits_t usage: 1;
        bits_t usage_sizes: 1;
        bits_t version: 1;
    };
    struct bits_opts_t bits = {
        .dump        = false,
        .usage       = false,
        .usage_sizes = false,
        .version     = false,
    };

    int opt;

    lib_version_check();

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

#if defined(DEBUG) && defined(DEBUG_GETOPT_LONG)
        PRINT_GETOPT_LONG_DEBUG();
#endif

        switch (opt) {
        case no_type_check_opt:
            opts.type_check =
                options_type_check_none_type;
            break;
        case type_def_opt:
            opts.type_check =
                options_type_check_def_type;
            opts.type_def = optarg;
            break;
        case type_lib_opt:
            opts.type_check =
                options_type_check_lib_type;
            opts.type_lib =
                parse_typelib_optarg("type-lib", optarg,
                    &opts.type_name);
            break;
        case type_name_opt:
            opts.type_name = optarg;
            break;
        case quote_cntrl_opt:
            opts.quote_cntrl = true;
            break;
        case no_quote_cntrl_opt:
            opts.quote_cntrl = false;
            break;
        case liter_value_opt:
            opts.liter_value = true;
            break;
        case no_liter_value_opt:
            opts.liter_value = false;
            break;
        case valid_utf8_opt:
            opts.valid_utf8 = true;
            break;
        case no_valid_utf8_opt:
            opts.valid_utf8 = false;
            break;
        case ascii_only_opt:
            opts.ascii_only = true;
            break;
        case no_ascii_only_opt:
            opts.ascii_only = false;
            break;
        case multi_obj_opt:
            opts.multi_obj = true;
            break;
        case no_multi_obj_opt:
            opts.multi_obj = false;
            break;
        case print_dots_opt:
            opts.print_dots = true;
            break;
        case no_print_dots_opt:
            opts.print_dots = false;
            break;
        case newline_sep_opt:
            opts.newline_sep = true;
            break;
        case no_newline_sep_opt:
            opts.newline_sep = false;
            break;
        case dump_opts_opt:
            bits.dump = true;
            break;
#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def) \
        case sizes_ ## id ## _opt: opts.sizes_ ## id =               \
            PARSE_SIZE_OPTARG(name, type, min, max);                 \
            break; 
#include "opt-sizes.def"

#ifdef DEBUG
#define CASE_DEBUG_(t, n)                              \
        case debug_ ## t ## _opt: {                    \
                size_t __n;                            \
                __n = parse_debug_optarg(n, optarg);   \
                ASSERT(SIZE_IS_BITS(__n, debug_bits)); \
                opts.debug_ ## t = __n;                \
            break;                                     \
        }
#define CASE_DEBUG(t) CASE_DEBUG_(t, #t)
#define CASE_DEBUG2   CASE_DEBUG_

        CASE_DEBUG(base)
        CASE_DEBUG(printer)
        CASE_DEBUG(obj)
        CASE_DEBUG(ast)
        CASE_DEBUG(type)
        CASE_DEBUG2(type_lib, "type-lib")
        CASE_DEBUG2(type_ruler, "type-ruler")

        case no_debug_opt:
            opts.debug_base = 0;
            opts.debug_printer = 0;
            opts.debug_obj = 0;
            opts.debug_ast = 0;
            opts.debug_type = 0;
            opts.debug_type_lib = 0;
            opts.debug_type_ruler = 0;
            break;
#endif
        case input_file_opt:
            opts.input = optarg;
            break;
        case version_opt:
            bits.version = true;
            break;
        case no_error_opt:
            opts.no_error = true;
            break;
        case no_no_error_opt:
            opts.no_error = false;
            break;
        case verbose_opt:
            opts.verbose = true;
            break;
        case no_verbose_opt:
            opts.verbose = false;
            break;

#undef  CASE
#define CASE(n)                                       \
        case n ## r_act:                              \
            opts.object = OBJ_JSON_PARSER;            \
            opts.parse_type = options_ ## n ## _type; \
            break;

        CASE(quiet_parse)
        CASE(echo_parse)

        case json2_act:
            opts.object = OBJ_JSON2;
            break;
        //!!!2JSONcase two_json_act:
        //!!!2JSON	opts.object = OBJ_2JSON;
        //!!!2JSON	break;

#undef  CASE
#define CASE(n)                                       \
        case n ## er_act:                             \
            opts.object = OBJ_JSON_PRINTER;           \
            opts.print_type = options_ ## n ## _type; \
            break;

        CASE(from_ast_print)
        CASE(pretty_print)
        CASE(terse_print)
        CASE(type_print)
        CASE(ast_print)

        case typelib_func_act:
            opts.object = OBJ_JSON_TYPELIB;
            opts.typelib_func = 
                parse_typelib_func_optarg("typelib-func", optarg);
            break;
        case help_sizes_opt:
            bits.usage_sizes = true;
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

    if (argc > 0) {
        opts.input = *argv;
        argc --;
        argv ++;
    }

    opts.argc = argc;
    opts.argv = argv;

    if (opts.input && !strcmp(opts.input, "-"))
        opts.input = NULL;

    if (bits.version)
        version();
    if (bits.dump)
        dump_options(&opts);
    if (bits.usage_sizes)
        usage_sizes();
    if (bits.usage)
        usage();

    if (bits.dump ||
        bits.version ||
        bits.usage_sizes ||
        bits.usage)
        exit(0);

    ASSERT_SIZE_DEC_NO_OVERFLOW(
        opts.sizes_error_buf_max);
    ASSERT_SIZE_MUL_NO_OVERFLOW(
        opts.sizes_error_ctxt_size, SZ(2));
    if (2 * opts.sizes_error_ctxt_size >
            opts.sizes_error_buf_max - 1)
        error("invalid pair of parameters "
            "'error-context-size' and 'error-buf-max': "
            "2 * %zu + 1 > %zu",
            opts.sizes_error_ctxt_size,
            opts.sizes_error_buf_max);

    if (opts.object == OBJ_JSON_TYPELIB &&
        opts.multi_obj)
        warning("`-m|--multi-object' is ignored when "
            "given along with `-T|--typelib-func'");

    return &opts;
}


