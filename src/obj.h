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

#ifndef __OBJ_H
#define __OBJ_H

#include <stdio.h>
#include <stddef.h>

#include "ptr-traits.h"

#define print_fmt(S, ...) fprintf(OBJ_SUPER(this)->output, S, ## __VA_ARGS__)
#define print_str(S)      fputs(S, OBJ_SUPER(this)->output)
#define print_chr(C)      fputc(C, OBJ_SUPER(this)->output)

struct obj_type_t;
struct options_t;

typedef size_t obj_size_t;

typedef void (*obj_init_func_t)(
    void*, const struct obj_type_t*, const struct options_t*);
typedef void (*obj_done_func_t)(void*);
typedef int (*obj_run_func_t)(void*);

struct obj_type_t
{
    obj_size_t      size;
    obj_size_t      align;
    obj_init_func_t init;
    obj_done_func_t done;
    obj_run_func_t  run;
};

enum obj_name_t  {
    OBJ_JSON_PARSER,
    OBJ_JSON_PRINTER,
    OBJ_JSON_TYPELIB,
    OBJ_JSON2,
//!!!2JSON	OBJ_2JSON
};

const struct obj_type_t* get_obj_type(enum obj_name_t);

#define OBJ_TYPE() \
const struct obj_type_t* type;

#define OBJ_ROOT() \
OBJ_TYPE();

#define OBJ_BASE(NAME) \
struct NAME base; \
OBJ_TYPE();

#define OBJ_DECL(OBJ, TYPE)                    \
char OBJ ## __[TYPE->size + TYPE->align];      \
char* OBJ = PTR_ALIGN(OBJ ## __, TYPE->align);

#define OBJ_INIT(THIS, TYPE) \
THIS->type = TYPE;

#define OBJ_SUPER(obj) \
(&((obj)->base))

#define OBJ_IMPL(NAME, TYPE, INIT, DONE, RUN)  \
const struct obj_type_t NAME = {               \
    .size  = sizeof(struct TYPE),              \
    .align = MEM_ALIGNOF(struct TYPE),         \
    .init  = (obj_init_func_t) INIT,           \
    .done  = (obj_done_func_t) DONE,           \
    .run   = (obj_run_func_t)  RUN,            \
};

struct obj_t
{
    OBJ_ROOT();

    FILE* output;
};

void obj_init(struct obj_t*, const struct obj_type_t*);
void obj_done(struct obj_t*);

#endif/*__OBJ_H */

