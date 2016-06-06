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

#ifndef __JSON_TYPE_H
#define __JSON_TYPE_H

#include "json.h"

struct json_type_node_t;

enum json_type_node_type_t
{
    json_type_any_node_type,
    json_type_plain_node_type,
    json_type_object_node_type,
    json_type_array_node_type,
    json_type_list_node_type,
};

// the simple types, i.e. all types below, except for:
//   `{ "type": "object", "args": ... }',
//   `{ "type": "array", "args": ... }' and
//   `{ "type": "list", "args": ... }',

enum json_type_any_node_type_t
{
    // the string `"type"' value
    json_type_any_type_type,

    // the string `"null"' value
    json_type_any_null_type,

    // the string `"boolean"' value
    json_type_any_boolean_type,

    // the string `"number"' value
    json_type_any_number_type,

    // the string `"string"' value
    json_type_any_string_type,

    // the string `"object"' value
    json_type_any_object_type,

    // the string `"array"' value
    json_type_any_array_type,
};

struct json_type_any_node_t
{
    enum json_type_any_node_type_t type;
};

// the plain value:
//   `{ "plain": $plain }'
// where
//    $plain is any plain value:
//      a null, a boolean, a number or a string value
enum json_type_plain_type_t {
    json_type_plain_null_type,
    json_type_plain_boolean_type,
    json_type_plain_number_type,
    json_type_plain_string_type,
};

struct json_type_plain_node_t
{
    enum json_type_plain_type_t type;
    union {
        // ...         null;
        bool           boolean;
        const uchar_t* number;
        const uchar_t* string;
    } val;
};

// the object value:
//   `{ "type": "object", "args": [ $arg* ] }'
// where $arg is the object value:
//   `{ "name": $string, "type": $type }'
// where:
//   $string is any string value and
//   $type is any type object

struct json_type_object_node_arg_t
{
    const uchar_t* name;
    const struct json_type_node_t* type;
    struct json_text_pos_t pos;
};

struct json_type_object_node_t
{
    const struct json_type_object_node_arg_t* args;
    size_t size;
};

// the object value:
//   `{ "type": "array", "args": $type }' or
//   `{ "type": "array", "args": [ $type* ] }'
// where $type is any type

enum json_type_array_node_type_t
{
    json_type_open_array_node_type,
    json_type_closed_array_node_type,
};

struct json_type_open_array_node_t
{
    const struct json_type_node_t* arg;
};

struct json_type_closed_array_node_t
{
    const struct json_type_node_t** args;
    size_t size;
};

struct json_type_array_node_t
{
    enum json_type_array_node_type_t type;
    union {
        struct json_type_open_array_node_t   open;
        struct json_type_closed_array_node_t closed;
    } val;
};

// the object value:
//   `{ "type": "list", "args": [ $type* ] }'
// where $type is any type

struct json_type_list_node_t
{
    const struct json_type_node_t** args;
    size_t size;
};

struct json_type_list_attr_t;

struct json_type_node_t
{
    enum json_type_node_type_t type;
    union {
        struct json_type_any_node_t    any;
        struct json_type_plain_node_t  plain;
        struct json_type_object_node_t object;
        struct json_type_array_node_t  array;
        struct json_type_list_node_t   list;
    } node;
    union {
        // const struct json_type_any_attr_t*    any;
        // const struct json_type_plain_attr_t*  plain;
        // const struct json_type_object_attr_t* object;
        // const struct json_type_array_attr_t*  array;
        const struct json_type_list_attr_t*      list;
    } attr;
    struct json_text_pos_t pos;
};

enum json_type_object_sym_type_t
{
    json_type_object_sym_null_type,
    json_type_object_sym_name_type,
    json_type_object_sym_node_type
};

struct json_type_object_sym_t
{
    enum json_type_object_sym_type_t   type;
    union {
        // ...                         null;
        const uchar_t*                 name;
        const struct json_type_node_t* node;
    } val;
};

// stev: definition of 'struct json_type_trie_t'

#define TRIE_NAME     json_type
#define TRIE_SYM_TYPE const struct json_type_node_t*
#define TRIE_VAL_TYPE const struct json_type_node_t*
#define TRIE_ALLOC_OBJ_TYPE struct json_type_lib_t

#define TRIE_NEED_STRUCT_ONLY
#include "trie-impl.h"
#undef  TRIE_NEED_STRUCT_ONLY

#undef  TRIE_VAL_TYPE
#undef  TRIE_SYM_TYPE
#undef  TRIE_NAME

// stev: definition of 'struct json_type_object_trie_t'

#define TRIE_NAME     json_type_object
#define TRIE_SYM_TYPE struct json_type_object_sym_t
#define TRIE_VAL_TYPE const struct json_type_node_t*

#define TRIE_NEED_STRUCT_ONLY
#include "trie-impl.h"
#undef  TRIE_NEED_STRUCT_ONLY

#undef  TRIE_VAL_TYPE
#undef  TRIE_SYM_TYPE
#undef  TRIE_NAME

struct json_type_list_attr_t
{
    const struct json_type_trie_t*        any;
    const struct json_type_trie_t*        plain;
    const struct json_type_object_trie_t* object;
    const struct json_type_trie_t*        open_array;
    const struct json_type_trie_t*        closed_array;
};

enum json_type_def_type_t
{
    json_type_def_node_type,
    json_type_def_dict_type
};

#define json_type_dict_arg_t json_type_object_node_arg_t

struct json_type_dict_t
{
    struct json_text_pos_t pos;
    const struct json_type_dict_arg_t* args;
    size_t size;
};

struct json_type_def_t
{
    enum json_type_def_type_t type;
    union {
        const struct json_type_node_t* node;
        const struct json_type_dict_t* dict;
    } val;
};

#endif/*__JSON_TYPE_H*/


