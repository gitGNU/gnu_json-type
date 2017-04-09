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

#ifndef __ERROR_H
#define __ERROR_H

enum json_error_type_t {
    json_error_none,
    json_error_lex,
    json_error_parse
};

enum json_lex_error_t {
    json_lex_error_ok,
    json_lex_error_invalid_char,
    json_lex_error_invalid_literal,
    json_lex_error_invalid_number_literal,
    json_lex_error_invalid_string_literal,
    json_lex_error_invalid_utf8_encoding,
};

enum json_parse_error_t {
    json_parse_error_ok,
    json_parse_error_literal_value,
    json_parse_error_multi_objects,
    json_parse_error_unexpected_token,
};

union json_error_what_t
{
    enum json_lex_error_t   lex;
    enum json_parse_error_t parse;
};

struct json_error_info_t
{
    enum json_error_type_t  type;
    union json_error_what_t what;
    struct json_error_pos_t pos;
};

enum json_type_lib_error_t
{
    json_type_lib_error_none,
    json_type_lib_error_oper,
    json_type_lib_error_sys,
    json_type_lib_error_lib,
    json_type_lib_error_ast,
    json_type_lib_error_meta,
    json_type_lib_error_attr
};

enum json_type_lib_error_oper_type_t
{
    json_type_lib_error_oper_parse,
    json_type_lib_error_oper_parse_done
};

enum json_type_lib_error_oper_context_t
{
    json_type_lib_error_oper_sobj_context
};

struct json_type_lib_error_oper_t
{
    enum json_type_lib_error_oper_type_t type;
    enum json_type_lib_error_oper_context_t context;
};

enum json_type_lib_error_sys_context_t
{
    json_type_lib_error_file_open_context,
    json_type_lib_error_file_stat_context,
    json_type_lib_error_file_read_context,
    json_type_lib_error_file_close_context
};

struct json_type_lib_error_sys_t
{
    enum json_type_lib_error_sys_context_t context;
    int error;
};

enum json_type_lib_generic_lib_error_type_t
{
    json_type_lib_generic_lib_error_invalid_name,
    json_type_lib_generic_lib_error_type_name_not_available,
    json_type_lib_generic_lib_error_type_name_not_specified,
    json_type_lib_generic_lib_error_type_def_not_found,
};

enum json_type_lib_shared_lib_error_type_t
{
    json_type_lib_shared_lib_error_invalid_name,
    json_type_lib_shared_lib_error_load_lib_failed,
    json_type_lib_shared_lib_error_get_lib_version_sym_not_found,
    json_type_lib_shared_lib_error_get_type_def_sym_not_found,
    json_type_lib_shared_lib_error_wrong_lib_version,
    json_type_lib_shared_lib_error_def_is_null,
    json_type_lib_shared_lib_error_def_not_valid,
};

enum json_type_lib_error_lib_type_t
{
    json_type_lib_error_generic_lib,
    json_type_lib_error_shared_lib,
};

struct json_type_lib_error_lib_t
{
    enum json_type_lib_error_lib_type_t type;
    union {
        enum json_type_lib_generic_lib_error_type_t generic;
        enum json_type_lib_shared_lib_error_type_t  shared;
    } lib;
    const char* name;
};

#define json_type_lib_error_ast_t json_error_info_t

enum json_type_ruler_error_type_t
{
    json_type_ruler_error_invalid_top_value,
    json_type_ruler_error_empty_obj_not_allowed,
    json_type_ruler_error_invalid_obj_first_key,
    json_type_ruler_error_invalid_type_obj_size_not_two,
    json_type_ruler_error_invalid_name_obj_size_not_two,
    json_type_ruler_error_invalid_plain_obj_size_not_one,
    json_type_ruler_error_invalid_type_obj_type_not_obj_arr_list,
    json_type_ruler_error_invalid_type_obj_2nd_arg_not_args,
    json_type_ruler_error_invalid_name_obj_2nd_arg_not_type,
    json_type_ruler_error_invalid_plain_obj_type_not_null_bool_num_str,
    json_type_ruler_error_invalid_name_obj_name_not_str,
    json_type_ruler_error_invalid_name_obj_type_arg_not_a_type,
    json_type_ruler_error_invalid_object_obj_args_not_array_of_name_objs,
    json_type_ruler_error_invalid_list_obj_args_not_of_type_array_of_types,
    json_type_ruler_error_invalid_array_obj_args_neither_type_nor_array_of_types,
    json_type_ruler_error_invalid_array_elem_is_neither_type_nor_name_obj,
    json_type_ruler_error_invalid_array_elem_is_either_type_xor_name_obj
};

struct json_type_lib_error_meta_t
{
    enum json_type_ruler_error_type_t type;
    struct json_error_pos_t           pos;
};

enum json_type_lib_error_attr_type_t
{
    json_type_lib_error_attr_invalid_list_dup_any,
    json_type_lib_error_attr_invalid_list_dup_plain,
    json_type_lib_error_attr_invalid_list_dup_object,
    json_type_lib_error_attr_invalid_list_dup_open_array,
    json_type_lib_error_attr_invalid_list_dup_closed_array,
    json_type_lib_error_attr_invalid_list_arrays_ambiguity,
    json_type_lib_error_attr_invalid_list_object_ambiguity,
    json_type_lib_error_attr_invalid_list_array_ambiguity,
    json_type_lib_error_attr_invalid_list_sort_of_object_of_object_not_supported,
    json_type_lib_error_attr_invalid_list_sort_of_object_of_array_not_supported,
    json_type_lib_error_attr_invalid_list_sort_of_object_of_list_not_supported,
    json_type_lib_error_attr_invalid_list_sort_of_array_of_object_not_supported,
    json_type_lib_error_attr_invalid_list_sort_of_array_of_array_not_supported,
    json_type_lib_error_attr_invalid_list_sort_of_array_of_list_not_supported,
    json_type_lib_error_attr_invalid_list_elem_is_list,
    json_type_lib_error_attr_invalid_defs_dup_name,
};

struct json_type_lib_error_attr_t
{
    enum json_type_lib_error_attr_type_t type;
    struct json_error_pos_t              pos[2];
};

struct json_type_lib_error_info_t
{
    enum json_type_lib_error_t type;
    union {
        struct json_type_lib_error_oper_t oper;
        struct json_type_lib_error_sys_t  sys;
        struct json_type_lib_error_lib_t  lib;
        struct json_type_lib_error_ast_t  ast;
        struct json_type_lib_error_meta_t meta;
        struct json_type_lib_error_attr_t attr;
    };
    struct json_file_info_t file_info;
};

enum json_type_error_type_t {
    json_type_error_none,
    json_type_error_parse,
    json_type_error_type_lib,
    json_type_error_type_check,
};

enum json_type_check_error_type_t
{
    json_type_check_error_none,
    json_type_check_error_type_mismatch,
    json_type_check_error_too_many_args,
    json_type_check_error_too_few_args,
    json_type_check_error_invalid_arg_name,
};

enum json_type_check_error_key_arg_type_t
{
    json_type_check_error_key_arg_name_type,
    json_type_check_error_key_arg_object_trie_type,
};

struct json_type_check_error_key_arg_t
{
    enum json_type_check_error_key_arg_type_t type;
    union {
        const uchar_t*                             name;
        const struct json_type_object_trie_node_t* object_trie;
    };
};

enum json_type_check_error_val_arg_type_t
{
    json_type_check_error_val_arg_node_type,
    json_type_check_error_val_arg_object_trie_type,
    json_type_check_error_val_arg_array_trie_type
};

struct json_type_object_trie_node_t;
struct json_type_trie_node_t;

struct json_type_check_error_array_trie_val_arg_t
{
    const struct json_type_trie_node_t* open;
    const struct json_type_trie_node_t* closed;
    size_t n_arg;
};

struct json_type_check_error_val_arg_t
{
    enum json_type_check_error_val_arg_type_t type;
    union {
        const struct json_type_node_t*                    node;
        const struct json_type_object_trie_node_t*        object_trie;
        struct json_type_check_error_array_trie_val_arg_t array_trie;
    };
};

enum json_type_check_error_arg_type_t
{
    json_type_check_error_arg_none_type,
    json_type_check_error_arg_key_type,
    json_type_check_error_arg_val_type
};

struct json_type_check_error_arg_t
{
    enum json_type_check_error_arg_type_t type;
    union {
        // ...                                 none;
        struct json_type_check_error_key_arg_t key;
        struct json_type_check_error_val_arg_t val;
    };
};

#define json_type_parse_error_info_t json_error_info_t

struct json_type_check_error_info_t
{
    enum json_type_check_error_type_t  type;
    struct json_type_check_error_arg_t arg;
    struct json_error_pos_t            pos;
};

struct json_type_error_info_t
{
    enum json_type_error_type_t type;
    union {
        struct json_type_parse_error_info_t parse;
        struct json_type_lib_error_info_t   type_lib;
        struct json_type_check_error_info_t type_check;
    };
};

const char* json_error_info_get_name(
    const struct json_error_info_t*);

const char* json_error_info_get_desc(
    const struct json_error_info_t*);

const struct json_error_info_t* json_get_error_info(
    struct json_obj_t*);

const struct json_error_info_t* json_ast_get_error_info(
    struct json_ast_t*);

const struct json_type_error_info_t* json_type_get_error_info(
    struct json_type_t*);

#ifdef JSON_DEBUG

void json_error_info_print_error_debug(
    const struct json_error_info_t*, FILE*);

void json_type_error_info_print_error_debug(
    const struct json_type_error_info_t*, FILE*);

#endif

#define JSON_TYPEOF_IS_SIZES(p, n) \
    TYPEOF_IS(p, const struct json_ ## n ## _sizes_t*)

#define JSON_SIZE_VALIDATE(s)              \
    do {                                   \
        if (s == 0) INVALID_ARG("%zu", s); \
    } while (0)

#define JSON_OBJ_SIZES_VALIDATE_(o)       \
    do {                                  \
        JSON_SIZE_VALIDATE(o buf_max);    \
        JSON_SIZE_VALIDATE(o buf_init);   \
        JSON_SIZE_VALIDATE(o stack_max);  \
        JSON_SIZE_VALIDATE(o stack_init); \
    } while (0)
#define JSON_OBJ_SIZES_VALIDATE(p)            \
    do {                                      \
        STATIC(JSON_TYPEOF_IS_SIZES(p, obj)); \
        JSON_OBJ_SIZES_VALIDATE_(p->);        \
    } while (0)

#define JSON_AST_SIZES_VALIDATE_(o)      \
    do {                                 \
        JSON_SIZE_VALIDATE(o pool_size); \
    } while (0)
#define JSON_AST_SIZES_VALIDATE(p)            \
    do {                                      \
        STATIC(JSON_TYPEOF_IS_SIZES(p, ast)); \
        JSON_OBJ_SIZES_VALIDATE_(p->obj.);    \
        JSON_AST_SIZES_VALIDATE_(p->);        \
    } while (0)

#define JSON_TYPE_LIB_SIZES_VALIDATE_(o)      \
    do {                                      \
        JSON_SIZE_VALIDATE(o pool_size);      \
        JSON_SIZE_VALIDATE(o text_max_size);  \
        JSON_SIZE_VALIDATE(o ptr_space_size); \
    } while (0)
#define JSON_TYPE_LIB_SIZES_VALIDATE(p)            \
    do {                                           \
        STATIC(JSON_TYPEOF_IS_SIZES(p, type_lib)); \
        JSON_AST_SIZES_VALIDATE_(p->ast.);         \
        JSON_TYPE_LIB_SIZES_VALIDATE_(p->);        \
    } while (0)

#define JSON_TYPE_SIZES_VALIDATE(p)             \
    do {                                        \
        STATIC(JSON_TYPEOF_IS_SIZES(p, type));  \
        JSON_OBJ_SIZES_VALIDATE_(p->obj.);      \
        JSON_TYPE_LIB_SIZES_VALIDATE_(p->lib.); \
    } while (0)

#endif/*__ERROR_H*/


