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

#ifndef __JSON_INTF_H
#define __JSON_INTF_H

#include "lib/json.h"

struct json_intf_t;

struct json_intf_t* json_intf_create_obj(
    const struct json_handler_t* handler, void* obj,
    const struct json_obj_sizes_t* sizes);
struct json_intf_t* json_intf_create_ast(
    const struct json_ast_sizes_t* sizes);
struct json_intf_t* json_intf_create_type_from_def(
    const uchar_t* type_def, const char* type_name,
    const struct json_handler_t* handler, void* obj,
    const struct json_type_sizes_t* sizes);
struct json_intf_t* json_intf_create_type_from_lib(
    const char* type_lib, const char* type_name,
    const struct json_handler_t* handler, void* obj,
    const struct json_type_sizes_t* sizes);
struct json_intf_t* json_intf_create_text_type_lib(
    const struct json_type_lib_sizes_t* sizes);
struct json_intf_t* json_intf_create_sobj_type_lib(
    const char* lib_name, const struct json_type_lib_sizes_t* sizes);

void json_intf_destroy(struct json_intf_t*);

void json_intf_config_set_param(
    struct json_intf_t*, enum json_config_param_t param, bool val);

enum json_parse_status_t json_intf_parse(struct json_intf_t*,
    const uchar_t*, size_t);
enum json_parse_status_t json_intf_parse_done(struct json_intf_t*);

bool json_intf_get_is_error(struct json_intf_t*);
struct json_error_pos_t json_intf_get_error_pos(struct json_intf_t*);
const struct json_file_info_t* json_intf_get_error_file(
    struct json_intf_t*);
void json_intf_print_error_desc(struct json_intf_t*, FILE*);
#ifdef JSON_DEBUG
void json_intf_print_error_debug(struct json_intf_t*, FILE*);
#endif

struct json_obj_t* json_intf_get_as_obj(
    struct json_intf_t*);

struct json_ast_t* json_intf_get_as_ast(
    struct json_intf_t*);

struct json_type_t* json_intf_get_as_type(
    struct json_intf_t*);

struct json_type_lib_t* json_intf_get_as_type_lib(
    struct json_intf_t*);

#endif/*__JSON_INTF_H*/


