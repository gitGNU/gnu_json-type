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

#ifndef __JSON_H
#define __JSON_H

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#ifndef JSON_DEBUG
#define JSON_DEBUG
#endif

#define JSON_VERSION_MAJOR 0
#define JSON_VERSION_MINOR 8
#define JSON_VERSION_PATCH 0

#define JSON_VERSION                 \
    (                                \
        JSON_VERSION_MAJOR * 10000 + \
        JSON_VERSION_MINOR * 100 +   \
        JSON_VERSION_PATCH           \
    )

#ifndef __GNUC__
#error we need a GCC compiler
#elif __GNUC__ >= 4
#define JSON_API __attribute__ ((visibility("default")))
#else
#define JSON_API
#endif

#ifndef JSON_UNSIGNED_CHAR_DEFINED
#define JSON_UNSIGNED_CHAR_DEFINED
typedef unsigned char uchar_t;
#endif

JSON_API size_t json_version();
JSON_API const char* json_build_datetime();

#ifdef JSON_DEBUG

enum json_debug_class_t
{
    json_debug_obj_class,
    json_debug_ast_class,
    json_debug_type_class,
    json_debug_type_lib_class,
    json_debug_type_ruler_class,
};

JSON_API size_t json_debug_get_level(
    enum json_debug_class_t class);

JSON_API void json_debug_set_level(
    enum json_debug_class_t class,
    size_t level);

#endif

typedef bool (*json_null_func_t)(void* this);
typedef bool (*json_boolean_func_t)(void* this, bool val);
typedef bool (*json_number_func_t)(void* this, 
    const uchar_t* val, size_t len);
typedef bool (*json_string_func_t)(void* this,
    const uchar_t* val, size_t len);

typedef bool (*json_object_start_func_t)(void* this);
typedef bool (*json_object_key_func_t)(void* this,
    const uchar_t* key, size_t len);
typedef bool (*json_object_sep_func_t)(void* this);
typedef bool (*json_object_end_func_t)(void* this);

typedef bool (*json_array_start_func_t)(void* this);
typedef bool (*json_array_sep_func_t)(void* this);
typedef bool (*json_array_end_func_t)(void* this);

typedef bool (*json_value_sep_func_t)(void* this);

struct json_handler_t
{
    json_null_func_t         null_func;
    json_boolean_func_t      boolean_func;
    json_number_func_t       number_func;
    json_string_func_t       string_func;
    json_object_start_func_t object_start_func;
    json_object_key_func_t   object_key_func;
    json_object_sep_func_t   object_sep_func;
    json_object_end_func_t   object_end_func;
    json_array_start_func_t  array_start_func;
    json_array_sep_func_t    array_sep_func;
    json_array_end_func_t    array_end_func;
    json_value_sep_func_t    value_sep_func;
};

struct json_obj_sizes_t
{
    size_t buf_max;
    size_t buf_init;
    size_t stack_max;
    size_t stack_init;
};

struct json_obj_t;

JSON_API struct json_obj_t* json_obj_create(
    const struct json_handler_t* handler, void* obj,
    const struct json_obj_sizes_t* sizes);

JSON_API void json_obj_destroy(struct json_obj_t* obj);

enum json_config_param_t {
    json_allow_literal_value_config,
    json_allow_multi_objects_config,
    json_disallow_non_ascii_config,
    json_validate_utf8_config,
};

JSON_API bool json_config_get_param(
    struct json_obj_t* obj, enum json_config_param_t param);

JSON_API void json_config_set_param(
    struct json_obj_t* obj, enum json_config_param_t param, bool val);

enum json_parse_status_t {
    json_parse_status_ok,
    json_parse_status_error,
    json_parse_status_canceled
};

#ifdef JSON_DEBUG
JSON_API const char* json_parse_status_get_name(enum json_parse_status_t);
#endif

JSON_API enum json_parse_status_t json_parse(
    struct json_obj_t* obj, const uchar_t* buf, size_t len);
JSON_API enum json_parse_status_t json_parse_done(
    struct json_obj_t* obj);
JSON_API size_t json_parse_get_config(
    struct json_obj_t* obj);

struct json_error_pos_t
{
    size_t line;
    size_t col;
};

#define json_text_pos_t json_error_pos_t

struct json_file_info_t
{
    const char* name;
    const uchar_t* buf;
    size_t size;
};

JSON_API bool json_get_is_error(
    struct json_obj_t*);
JSON_API struct json_error_pos_t json_get_error_pos(
    struct json_obj_t*);
JSON_API void json_print_error_desc(
    struct json_obj_t*, FILE*);
JSON_API const struct json_file_info_t* json_get_error_file(
    struct json_obj_t*);
#ifdef JSON_DEBUG
JSON_API void json_print_error_debug(
    struct json_obj_t*, FILE*);
#endif

struct json_ast_null_node_t
{};

struct json_ast_boolean_node_t
{
    bool val;
};

struct json_ast_buf_t
{
    const uchar_t* ptr;
    size_t size;
};

struct json_ast_number_node_t
{
    struct json_ast_buf_t buf;
};

struct json_ast_string_node_t
{
    struct json_ast_buf_t buf;
};

struct json_ast_object_arg_t
{
    struct json_ast_string_node_t* key;
    struct json_ast_node_t*        val;
};

struct json_ast_object_node_t
{
    struct json_ast_object_arg_t* args;
    size_t size;
};

struct json_ast_array_node_t
{
    struct json_ast_node_t** args;
    size_t size;
};

struct json_ast_node_t;

typedef void* (*json_ast_node_null_func_t)(void* this,
    const struct json_ast_null_node_t* node);
typedef void* (*json_ast_node_boolean_func_t)(void* this,
    const struct json_ast_boolean_node_t* node);
typedef void* (*json_ast_node_number_func_t)(void* this, 
    const struct json_ast_number_node_t* node);
typedef void* (*json_ast_node_string_func_t)(void* this,
    const struct json_ast_string_node_t* node);
typedef void* (*json_ast_node_object_func_t)(void* this,
    const struct json_ast_object_node_t* node);
typedef void* (*json_ast_node_array_func_t)(void* this,
    const struct json_ast_array_node_t* node);

struct json_ast_visitor_t
{
    json_ast_node_null_func_t    null_func;
    json_ast_node_boolean_func_t boolean_func;
    json_ast_node_number_func_t  number_func;
    json_ast_node_string_func_t  string_func;
    json_ast_node_object_func_t  object_func;
    json_ast_node_array_func_t   array_func;
};

struct json_ast_sizes_t
{
    struct json_obj_sizes_t obj;
    size_t pool_size;
};

struct json_ast_t;

JSON_API struct json_ast_t* json_ast_create(
    const struct json_ast_sizes_t* sizes);

JSON_API void json_ast_destroy(struct json_ast_t* ast);

JSON_API bool json_ast_config_get_param(
    struct json_ast_t* ast, enum json_config_param_t param);

JSON_API void json_ast_config_set_param(
    struct json_ast_t* ast, enum json_config_param_t param, bool val);

JSON_API enum json_parse_status_t json_ast_parse(
    struct json_ast_t* ast, const uchar_t* buf, size_t len);

JSON_API enum json_parse_status_t json_ast_parse_done(
    struct json_ast_t* ast);

JSON_API const struct json_ast_node_t* json_ast_get_root(
    const struct json_ast_t* ast);

JSON_API void* json_ast_visit(
    const struct json_ast_node_t* node,
    const struct json_ast_visitor_t* vis,
    void* obj);

JSON_API bool json_ast_get_is_error(
    struct json_ast_t*);
JSON_API struct json_error_pos_t json_ast_get_error_pos(
    struct json_ast_t*);
JSON_API const struct json_file_info_t* json_ast_get_error_file(
    struct json_ast_t*);
JSON_API void json_ast_print_error_desc(
    struct json_ast_t*, FILE*);
#ifdef JSON_DEBUG
JSON_API void json_ast_print_error_debug(
    struct json_ast_t*, FILE*);
#endif

struct json_type_lib_sizes_t
{
    struct json_ast_sizes_t ast;
    size_t pool_size;
    size_t text_max_size;
    size_t ptr_space_size;
};

struct json_type_sizes_t
{
    struct json_obj_sizes_t obj;
    struct json_type_lib_sizes_t lib;
    size_t var_stack_max;
    size_t var_stack_init;
};

struct json_type_t;

JSON_API struct json_type_t* json_type_create_from_def(
    const uchar_t* type_def, const char* type_name,
    const struct json_handler_t* handler, void* obj,
    const struct json_type_sizes_t* sizes);

JSON_API struct json_type_t* json_type_create_from_lib(
    const char* type_lib, const char* type_name,
    const struct json_handler_t* handler, void* obj,
    const struct json_type_sizes_t* sizes);

JSON_API void json_type_destroy(struct json_type_t* type);

JSON_API bool json_type_config_get_param(
    struct json_type_t* type, enum json_config_param_t param);

JSON_API void json_type_config_set_param(
    struct json_type_t* type, enum json_config_param_t param, bool val);

JSON_API enum json_parse_status_t json_type_parse(
    struct json_type_t* type, const uchar_t* buf, size_t len);

JSON_API enum json_parse_status_t json_type_parse_done(
    struct json_type_t* type);

JSON_API bool json_type_get_is_error(
    struct json_type_t*);
JSON_API struct json_error_pos_t json_type_get_error_pos(
    struct json_type_t*);
JSON_API const struct json_file_info_t* json_type_get_error_file(
    struct json_type_t*);
JSON_API void json_type_print_error_desc(
    struct json_type_t*, FILE*);
#ifdef JSON_DEBUG
JSON_API void json_type_print_error_debug(
    struct json_type_t*, FILE*);
#endif

struct json_type_lib_t;

JSON_API struct json_type_lib_t* json_type_lib_create_from_source_text(
    const struct json_type_lib_sizes_t* sizes);
JSON_API struct json_type_lib_t* json_type_lib_create_from_shared_obj(
    const char* lib_name, const struct json_type_lib_sizes_t* sizes);
JSON_API void json_type_lib_destroy(struct json_type_lib_t*);

JSON_API enum json_parse_status_t json_type_lib_parse(
    struct json_type_lib_t* lib, const uchar_t* buf, size_t len);

JSON_API enum json_parse_status_t json_type_lib_parse_done(
    struct json_type_lib_t* lib);

JSON_API bool json_type_lib_get_is_error(
    struct json_type_lib_t*);
JSON_API struct json_error_pos_t json_type_lib_get_error_pos(
    struct json_type_lib_t*);
JSON_API const struct json_file_info_t* json_type_lib_get_error_file(
    struct json_type_lib_t*);
JSON_API void json_type_lib_print_error_desc(
    struct json_type_lib_t*, FILE*);
#ifdef JSON_DEBUG
JSON_API void json_type_lib_print_error_debug(
    struct json_type_lib_t*, FILE*);
#endif

JSON_API bool json_type_lib_validate(struct json_type_lib_t*);
JSON_API bool json_type_lib_print(struct json_type_lib_t*);
JSON_API bool json_type_lib_print_attr(struct json_type_lib_t*);
JSON_API bool json_type_lib_check_attr(struct json_type_lib_t*);
JSON_API bool json_type_lib_gen_def(struct json_type_lib_t*);

enum json_file_error_type_t
{
    json_file_none_error,
    json_file_open_error,
    json_file_stat_error,
    json_file_read_error,
    json_file_close_error,
};

struct json_file_error_t
{
    enum json_file_error_type_t type;
    int sys;
};

JSON_API bool json_file_is_shared_obj(
    const char* file_name, struct json_file_error_t* error);
JSON_API const char* json_file_error_type_get_desc(
    enum json_file_error_type_t);

#endif/*__JSON_H*/


