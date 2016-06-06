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

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>

#ifdef DEBUG_IMPL_DEBUG

static void print_debug_msg(
    const char* file, int line,
    const char* func, bool nl,
    const char* fmt, va_list arg)
{
    static const char* const fmts[] = {
        "!!! %s:%d:%s: %s\n",
        "!!! %s:%d:%s: %s"
    };

    char buf[1024];
    const char* f;

    vsnprintf(buf, sizeof buf - 1, fmt, arg);
    buf[sizeof buf - 1] = 0;

    f = strrchr(file, '/');
    if (f == NULL)
        f = file;
    else
        f ++;

    fprintf(stderr, fmts[!nl], f, line, func, buf);
}

void print_debug_header(
    const char* file, int line,
    const char* func, const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    print_debug_msg(file, line, func, false, fmt, arg);
    va_end(arg);
}

void print_debug_fmt(const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    vfprintf(stderr, fmt, arg);
    va_end(arg);
}

void print_debug_str(const char* str)
{
    fputs(str, stderr);
}

void print_debug(
    const char* file, int line,
    const char* func, const char* fmt, ...)
{
    va_list arg;

    va_start(arg, fmt);
    print_debug_msg(file, line, func, true, fmt, arg);
    va_end(arg);
}

#endif // DEBUG_IMPL_DEBUG


