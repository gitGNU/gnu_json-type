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

#ifndef __DEBUG_IMPL_H
#define __DEBUG_IMPL_H

#include <stdarg.h>

#ifndef DEBUG_IMPL_DEBUG

#define PRINT_DEBUG_BEGIN(M, ...) \
    do {

#define PRINT_DEBUG_BEGIN_UNCOND(M, ...) \
    do {

#define PRINT_DEBUG_END() \
    } while (0)

#define PRINT_DEBUG_FMT(M, ...)

#define PRINT_DEBUG(M, ...) \
    do {} while (0)

#else // DEBUG_IMPL_DEBUG

#define DEBUG_IMPL_PRINTF(F) \
    __attribute__((format(printf, F, F + 1)))

#ifndef PRINT_DEBUG_COND
#define PRINT_DEBUG_COND true
#endif

#define PRINT_DEBUG_BEGIN(M, ...)             \
    do {                                      \
        if (PRINT_DEBUG_COND) {               \
            print_debug_header(               \
                __FILE__, __LINE__,           \
                __func__, M, ## __VA_ARGS__); \

#define PRINT_DEBUG_END()                     \
            fputc('\n', stderr);              \
        }                                     \
    }                                         \
    while (0)

#define PRINT_DEBUG_BEGIN_UNCOND(M, ...)      \
    do {                                      \
        print_debug_header(                   \
            __FILE__, __LINE__,               \
            __func__, M, ## __VA_ARGS__);     \

#define PRINT_DEBUG_END_UNCOND()              \
        fputc('\n', stderr);                  \
    }                                         \
    while (0)

#define PRINT_DEBUG_STR(M) \
        print_debug_str(M);

#define PRINT_DEBUG_FMT(M, ...) \
        print_debug_fmt(M, ## __VA_ARGS__);

#define PRINT_DEBUG(M, ...)                   \
    do {                                      \
        if (PRINT_DEBUG_COND)                 \
            print_debug(                      \
                __FILE__, __LINE__,           \
                __func__, M, ## __VA_ARGS__); \
    }                                         \
    while (0)

#define PRINT_DEBUG_UNCOND(M, ...)            \
    do {                                      \
        print_debug(                          \
            __FILE__, __LINE__,               \
            __func__, M, ## __VA_ARGS__);     \
    }                                         \
    while (0)

#define PRINT_DEBUG_EX(C, M, ...)             \
    do {                                      \
        if (C)                                \
            print_debug(                      \
                __FILE__, __LINE__,           \
                __func__, M, ## __VA_ARGS__); \
    }                                         \
    while (0)

void print_debug_header(
    const char* file, int line,
    const char* func, const char* fmt, ...)
    DEBUG_IMPL_PRINTF(4);

void print_debug_fmt(const char* fmt, ...)
    DEBUG_IMPL_PRINTF(1);

void print_debug_str(const char* str);

void print_debug(
    const char* file, int line,
    const char* func, const char* fmt, ...)
    DEBUG_IMPL_PRINTF(4);

#endif // DEBUG_IMPL_DEBUG

#endif/*__DEBUG_IMPL_H*/


