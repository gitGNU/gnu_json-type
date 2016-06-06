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

#ifndef __JSON_BASE_H
#define __JSON_BASE_H

#include "lib/json.h"

#include "int-traits.h"
#include "common.h"

struct mem_buf_t;

struct json_intf_t;

struct obj_json_base_t
{
    const char* input;
    int         descr;

    size_t err_max;
    size_t err_init;
    size_t buf_size;
    size_t err_ctxt;

    bits_t verbose: 1;
    bits_t no_error: 1;
    bits_t ascii_only: 1;
#ifdef DEBUG
    bits_t debug: debug_bits;
#endif
    bits_t fixed_size_buf: 1;
    bits_t echo_input: 1;

    struct json_intf_t* json;
    struct mem_buf_t*   buf;
};

void obj_json_base_init_obj(
    struct obj_json_base_t* this,
    const struct options_t* opts,
    const struct json_handler_t* handler,
    void* obj);

void obj_json_base_init_ast(
    struct obj_json_base_t* this,
    const struct options_t* opts);

void obj_json_base_init_type(
    struct obj_json_base_t* this,
    const struct options_t* opts,
    const struct json_handler_t* handler,
    void* obj);

void obj_json_base_init_text_type_lib(
    struct obj_json_base_t* this,
    const struct options_t* opts);

void obj_json_base_init_sobj_type_lib(
    struct obj_json_base_t* this,
    const struct options_t* opts);

void obj_json_base_done(struct obj_json_base_t* this);

int obj_json_base_run(struct obj_json_base_t* this);

bool obj_json_base_check_error(struct obj_json_base_t* this);

struct json_obj_t* obj_json_base_get_obj(
    struct obj_json_base_t* this);

struct json_ast_t* obj_json_base_get_ast(
    struct obj_json_base_t* this);

struct json_type_t* obj_json_base_get_type(
    struct obj_json_base_t* this);

struct json_type_lib_t* obj_json_base_get_type_lib(
    struct obj_json_base_t* this);

#define JSON_BASE(obj) (&(obj)->json_base)

#endif/*__JSON_BASE_H*/

