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
#include <stdlib.h>
#include <stdarg.h>

#ifndef JSON_NO_LIB_MAIN
#include "json.h"
#endif

#include "int-traits.h"
#include "common.h"

static const char program[] = STRINGIFY(PROGRAM);

#ifndef JSON_NO_LIB_MAIN
static const char license[] =
"Copyright (C) 2016  Stefan Vargyas.\n"
"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n"
"This is free software: you are free to change and redistribute it.\n"
"There is NO WARRANTY, to the extent permitted by law.\n";
#endif

typedef char fmt_buf_t[512];

void fatal_error(const char* fmt, ...)
{
    va_list arg;

    fmt_buf_t b;

    va_start(arg, fmt);
    vsnprintf(b, sizeof b - 1, fmt, arg);
    va_end(arg);

    b[sizeof b - 1] = 0;

    fprintf(
        stderr, "%s: fatal error: %s\n",
        program, b);

    exit(127);
}

void assert_failed(
    const char* file, int line,
    const char* func, const char* expr)
{
    fatal_error(
        "assertion failed: %s:%d:%s: %s",
        file, line, func, expr);
}

void ensure_failed(
    const char* file, int line,
    const char* func, const char* msg, ...)
{
    va_list arg;
    fmt_buf_t b;

    va_start(arg, msg);
    vsnprintf(b, sizeof b - 1, msg, arg);
    va_end(arg);

    b[sizeof b - 1] = 0;

    fatal_error(
        "%s:%d:%s: %s",
        file, line, func, b);
}

void unexpect_error(
    const char* file, int line,
    const char* func, const char* msg, ...)
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

void invalid_argument(
    const char* file, int line, const char* func,
    const char* name, const char* msg, ...)
{
    va_list arg;
    fmt_buf_t b;

    va_start(arg, msg);
    vsnprintf(b, sizeof b - 1, msg, arg);
    va_end(arg);

    b[sizeof b - 1] = 0;

    fatal_error(
        "invalid argument: %s:%d:%s: %s='%s'",
        file, line, func, name, b);
}

void not_yet_impl(
    const char* file, int line, const char* func)
{
    fatal_error(
        "%s:%d:%s: function not yet implemented",
        file, line, func);
}

struct json_lib_version_t json_lib_version(
    size_t ver)
{
    enum { M = 10000, m = 100 };
    struct json_lib_version_t r;

    ASSERT(ver > 0 && ver < M * 10);

    r.major = ver / M;
    ver %= M;
    r.minor = ver / m;
    r.patch = ver % m;

    return r;
}

bool json_timeval_get_time(
    struct timeval* a)
{
    timerclear(a);
    return gettimeofday(a, NULL) == 0;
}

size_t json_timeval_subtract(
    const struct timeval* a,
    const struct timeval* b)
{
    struct timeval r;

    timerclear(&r);
    timersub(a, b, &r);

    return json_timeval_to_usecs(&r);
}

size_t json_timeval_to_usecs(
    const struct timeval* a)
{
    const size_t limit_usecs = 1000000;
    size_t u, s;

    u = INT_AS_SIZE(a->tv_usec);
    s = INT_AS_SIZE(a->tv_sec);

    ASSERT_SIZE_MUL_NO_OVERFLOW(s, limit_usecs);
    s *= limit_usecs;

    ASSERT_SIZE_ADD_NO_OVERFLOW(s, u);
    return s + u;
}

#ifndef JSON_NO_LIB_MAIN

int main()
{
    const char* b;
    struct json_lib_version_t v;

    v = json_lib_version(JSON_VERSION);
    b = json_build_datetime();
    ASSERT(b != NULL);

    fprintf(stdout,
        "%s: version: %u.%u.%u\n"
        "%s: build: %s\n"
        "%s: JSON_DEBUG is %sabled\n\n%s",
        program, v.major, v.minor, v.patch,
        program, b,
        program,
#ifdef JSON_DEBUG
        "en",
#else
        "dis",
#endif
        license
    );

    return 0;
}

#endif // JSON_NO_LIB_MAIN


