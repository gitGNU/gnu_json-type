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

#include <sys/time.h>
#include <stdbool.h>

#ifndef __GNUC__
#error we need a GCC compiler
#endif

#define UNUSED    __attribute__((unused))
#define PRINTF(F) __attribute__((format(printf, F, F + 1)))
#define NORETURN  __attribute__((noreturn))

#define WEAK      __attribute__((weak))

#define STRINGIFY_(s) #s
#define STRINGIFY(s)  STRINGIFY_(s)

// stev: important requirement: VERIFY evaluates E only once!

#define VERIFY(E)             \
    do {                      \
        if (!(E))             \
            UNEXPECT_ERR(#E); \
    }                         \
    while (0)

// stev: important requirement: ENSURE evaluates E only once!

#define ENSURE(E, M, ...)                     \
    do {                                      \
        if (!(E))                             \
            ensure_failed(__FILE__, __LINE__, \
                __func__, M, ## __VA_ARGS__); \
    }                                         \
    while (0)

#define UNEXPECT_ERR(M, ...)               \
    do {                                   \
        unexpect_error(__FILE__, __LINE__, \
            __func__, M, ## __VA_ARGS__);  \
    }                                      \
    while (0)

#define UNEXPECT_VAR(F, N) UNEXPECT_ERR(#N "=" F, N)

#define INVALID_ARG(F, N)                    \
    do {                                     \
        invalid_argument(__FILE__, __LINE__, \
            __func__, #N, F, N);             \
    }                                        \
    while (0)

#define NOT_YET_IMPL()                   \
    do {                                 \
        not_yet_impl(__FILE__, __LINE__, \
            __func__);                   \
    }                                    \
    while (0)

#ifdef JSON_DEBUG
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

void fatal_error(const char* fmt, ...)
    PRINTF(1)
    NORETURN;

void ensure_failed(
    const char* file, int line, const char* func, const char* msg, ...)
    PRINTF(4)
    NORETURN;

void assert_failed(
    const char* file, int line, const char* func, const char* expr)
    NORETURN;

void unexpect_error(
    const char* file, int line, const char* func, const char* msg, ...)
    PRINTF(4)
    NORETURN;

void invalid_argument(
    const char* file, int line, const char* func,
    const char* name, const char* msg, ...)
    PRINTF(5)
    NORETURN;

void not_yet_impl(
    const char* file, int line, const char* func)
    NORETURN;

struct json_lib_version_t
{
    unsigned short major;
    unsigned short minor;
    unsigned short patch;
};

struct json_lib_version_t json_lib_version(
    size_t);

bool json_timeval_get_time(
    struct timeval*);

size_t json_timeval_subtract(
    const struct timeval*,
    const struct timeval*);

size_t json_timeval_to_usecs(
    const struct timeval*);

#endif/*__COMMON_H*/

