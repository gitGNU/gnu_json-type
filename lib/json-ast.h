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

#ifndef __JSON_AST_H
#define __JSON_AST_H

#include "json.h"

enum json_ast_node_type_t
{
    json_ast_null_node_type,
    json_ast_boolean_node_type,
    json_ast_number_node_type,
    json_ast_string_node_type,
    json_ast_object_node_type,
    json_ast_array_node_type
};

struct json_ast_node_t
{
    enum json_ast_node_type_t type;
    union {
        struct json_ast_null_node_t    null;
        struct json_ast_boolean_node_t boolean;
        struct json_ast_number_node_t  number;
        struct json_ast_string_node_t  string;
        struct json_ast_object_node_t  object;
        struct json_ast_array_node_t   array;
    };
    struct json_text_pos_t pos;
};

struct json_text_pos_t json_get_current_tok_pos(
    struct json_obj_t* obj);

#define JSON_AST_TYPEOF_IS_NODE_(q, p) \
    TYPEOF_IS(p, q struct json_ast_node_t*)

#define JSON_AST_TYPEOF_IS_NODE(p) \
    JSON_AST_TYPEOF_IS_NODE_(, p)
#define JSON_AST_TYPEOF_IS_NODE_CONST(p) \
    JSON_AST_TYPEOF_IS_NODE_(const, p)

#define JSON_AST_NODE_IS_(q, p, n)                \
    (                                             \
        STATIC(JSON_AST_TYPEOF_IS_NODE_(q, p)),   \
        (p)->type == json_ast_ ## n ## _node_type \
    )

#define JSON_AST_NODE_IS(p, n) \
    JSON_AST_NODE_IS_(, p, n)
#define JSON_AST_NODE_IS_CONST(p, n) \
    JSON_AST_NODE_IS_(const, p, n)

#define JSON_AST_NODE_AS_(q, p, n)          \
    ({                                      \
        ASSERT(JSON_AST_NODE_IS_(q, p, n)); \
        &((p)->n);                          \
    })

#define JSON_AST_NODE_AS(p, n) \
    JSON_AST_NODE_AS_(, p, n)
#define JSON_AST_NODE_AS_CONST(p, n) \
    JSON_AST_NODE_AS_(const, p, n)

#define JSON_AST_NODE_AS_IF_(q, p, n) \
    (                                 \
        JSON_AST_NODE_IS_(q, p, n)    \
        ? &((p)->n) : NULL            \
    )
#define JSON_AST_NODE_AS_IF(p, n) \
    JSON_AST_NODE_AS_IF_(, p, n)
#define JSON_AST_NODE_AS_IF_CONST(p, n) \
    JSON_AST_NODE_AS_IF_(const, p, n)

#define JSON_AST_TYPEOF_IS_(q, p, n) \
    TYPEOF_IS(p, q struct json_ast_ ## n ## _node_t*)

#define JSON_AST_TO_NODE_(q, p, n)            \
    ({                                        \
        q struct json_ast_node_t* __r;        \
        STATIC(JSON_AST_TYPEOF_IS_(q, p, n)); \
        __r = (q struct json_ast_node_t*)     \
            ((char*)(p) - offsetof(           \
                struct json_ast_node_t, n));  \
        ASSERT(JSON_AST_NODE_IS_(q, __r, n)); \
        __r;                                  \
    })

#define JSON_AST_TYPEOF_IS(p, n)       JSON_AST_TYPEOF_IS_(, p, n)
#define JSON_AST_TYPEOF_IS_CONST(p, n) JSON_AST_TYPEOF_IS_(const, p, n)

#define JSON_AST_TO_NODE(p, n)         JSON_AST_TO_NODE_(, p, n)
#define JSON_AST_TO_NODE_CONST(p, n)   JSON_AST_TO_NODE_(const, p, n)

#endif/*__JSON_AST_H*/



