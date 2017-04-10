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

#ifndef __TEST_COMMON_H
#define __TEST_COMMON_H

#include <getopt.h>
#include <stdbool.h>

extern const char program[];
extern const char verdate[];
extern const char help[];

extern const char stdin_name[];

#define KB(X) (SZ(X) * SZ(1024))
#define MB(X) (KB(X) * SZ(1024))

struct su_size_t
{
    size_t sz;
    const char* su;
};

struct su_size_t su_size(size_t sz);

typedef bool (*options_done_func_t)(void*,
    size_t);
typedef bool (*options_parse_func_t)(void*,
    int, const char*);
typedef void (*options_dump_func_t)(void*);

struct options_funcs_t
{
    options_done_func_t  done;
    options_parse_func_t parse;
    options_dump_func_t  dump;
};

size_t options_parse_size_optarg(
    const char* opt_name, const char* opt_arg,
    size_t min, size_t max);

size_t options_parse_su_size_optarg(
    const char* opt_name, const char* opt_arg,
    size_t min, size_t max);

void options_invalid_opt_arg(
    const char* opt_name, const char* opt_arg);
void options_illegal_opt_arg(
    const char* opt_name, const char* opt_arg);

void options_parse_args(void* opts,
    const struct options_funcs_t* funcs,
    const char* shorts, size_t n_shorts,
    struct option* longs, size_t n_longs,
    int argc, char* argv[]);

#define OPTIONS_NAME_(x, t)          \
    ({                               \
        size_t __v = opts->x;        \
        ARRAY_NON_NULL_ELEM(t, __v); \
    })
#define OPTIONS_NAME(x)  OPTIONS_NAME_(x, x ## s)
#define OPTIONS_NNUL(x)  (opts->x ? opts->x : "-")
#define OPTIONS_NOYES(x) OPTIONS_NAME_(x, noyes)

#endif /* __TEST_COMMON_H */


