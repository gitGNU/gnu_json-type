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

#ifndef __COMMON_H
#define __COMMON_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

#include "lib/mem-buf.h"

#include "obj.h"

#define DEBUG

#ifndef __GNUC__
#error we need a GCC compiler
#endif

#define UNUSED    __attribute__((unused))
#define PRINTF(F) __attribute__((format(printf, F, F + 1)))
#define NORETURN  __attribute__((noreturn))

// stev: important requirement: VERIFY evaluates E only once!

#define VERIFY(E)             \
    do {                      \
        if (!(E))             \
            UNEXPECT_ERR(#E); \
    }                         \
    while (0)

#define UNEXPECT_ERR(M, ...)               \
    do {                                   \
        unexpect_error(__FILE__, __LINE__, \
            __func__, M, ## __VA_ARGS__);  \
    }                                      \
    while (0)

#define OOM_ERROR(M, ...)             \
    do {                              \
        oom_error(M, ## __VA_ARGS__); \
    }                                 \
    while (0)

#ifdef DEBUG
# define ASSERT(E)                            \
    do {                                      \
        if (!(E))                             \
            assert_failed(__FILE__, __LINE__, \
                __func__, #E);                \
    }                                         \
    while (0)
#else
# define ASSERT(E) \
    do {} while (0)
#endif

#define STATIC(E)                                   \
    ({                                              \
        extern int __attribute__                    \
            ((error("assertion failed: '" #E "'"))) \
            static_assert();                        \
        (void) ((E) ? 0 : static_assert());         \
    })

extern const char stdin_name[];

void fatal_error(const char* fmt, ...)
    PRINTF(1)
    NORETURN;

void assert_failed(
    const char* file, int line,
    const char* func, const char* expr)
    NORETURN;

void unexpect_error(
    const char* file, int line,
    const char* func, const char* msg, ...)
    PRINTF(4)
    NORETURN;

void warning(const char*, ...)
    PRINTF(1);
void error(const char*, ...)
    PRINTF(1)
    NORETURN;

void oom_error(const char*, ...)
    PRINTF(1)
    NORETURN;
void sys_error(const char*, ...)
    PRINTF(1)
    NORETURN;

void pos_error_header(
    const char* file, size_t line, size_t col);

void pos_error(
    const char* file, size_t line, size_t col,
    const char* fmt, ...)
    PRINTF(4);

bool mem_find(const uchar_t* buf, size_t len, char ch, size_t* pos);
bool mem_rfind(const uchar_t* buf, size_t len, char ch, size_t* pos);
size_t mem_count(const uchar_t* buf, size_t len, char ch);

bool mem_find_nth(
    const uchar_t* buf, size_t len, char ch, size_t n, size_t* pos);

enum { debug_bits = 4 };

enum options_parse_type_t
{
    options_quiet_parse_type,
    options_echo_parse_type,
};

enum options_print_type_t
{
    options_from_ast_print_type,
    options_pretty_print_type,
    options_terse_print_type,
    options_type_print_type,
    options_ast_print_type,
};

enum options_typelib_func_t
{
    options_validate_typelib_func,
    options_print_typelib_func,
    options_attr_typelib_func,
    options_check_typelib_func,
    options_print_check_typelib_func,
    options_def_typelib_func,
};

enum options_type_check_type_t
{
    options_type_check_none_type,
    options_type_check_def_type,
    options_type_check_lib_type,
};

struct options_t
{
    const char       *input;
    enum obj_name_t   object;
    enum options_parse_type_t
                      parse_type;
    enum options_print_type_t
                      print_type;
    enum options_typelib_func_t
                      typelib_func;
    enum options_type_check_type_t
                      type_check;
    const char       *type_def;
    const char       *type_lib;
    const char       *type_name;

    bits_t            quote_cntrl: 1;
    bits_t            liter_value: 1;
    bits_t            valid_utf8 : 1;
    bits_t            ascii_only: 1;
    bits_t            multi_obj: 1;
    bits_t            print_dots: 1;
    bits_t            newline_sep: 1;
    bits_t            no_error: 1;
    bits_t            verbose: 1;

#undef  CASE
#define CASE(type, id, name, pad, letter, desc, help, min, max, def) \
    size_t sizes_ ## id;
#include "opt-sizes.def"

#ifdef DEBUG
    bits_t            debug_base:       debug_bits;
    bits_t            debug_printer:    debug_bits;
    bits_t            debug_obj:        debug_bits;
    bits_t            debug_ast:        debug_bits;
    bits_t            debug_type:       debug_bits;
    bits_t            debug_type_lib:   debug_bits;
    bits_t            debug_type_ruler: debug_bits;
#endif

    size_t            argc;
    char* const      *argv;
};

const struct options_t* options(int argc, char* argv[]);

#endif/*__COMMON_H */

