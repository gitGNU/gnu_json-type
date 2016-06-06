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

#ifdef JSON_AST_NEED_VISITOR

#define JSON_AST_VISITOR_TABLE_TYPE 0
#define JSON_AST_VISITOR_STATIC_TYPE 1

#ifndef JSON_AST_VISITOR_TYPE
#error  JSON_AST_VISITOR_TYPE is not defined
#elif   JSON_AST_VISITOR_TYPE != JSON_AST_VISITOR_TABLE_TYPE && \
        JSON_AST_VISITOR_TYPE != JSON_AST_VISITOR_STATIC_TYPE
#error  JSON_AST_VISITOR_TYPE is neither 0 (i.e. `table') nor 1 (i.e. `static')
#endif

#ifndef JSON_AST_VISITOR_NAME
#error  JSON_AST_VISITOR_NAME is not defined
#endif

#ifndef JSON_AST_VISITOR_RESULT_TYPE
#error  JSON_AST_VISITOR_RESULT_TYPE is not defined
#endif

#ifndef JSON_AST_VISITOR_OBJECT_TYPE
#error  JSON_AST_VISITOR_OBJECT_TYPE is not defined
#endif

#define JSON_AST_VISITOR_MAKE_NAME_(n, s) n ## s
#define JSON_AST_VISITOR_MAKE_NAME(n, s)  JSON_AST_VISITOR_MAKE_NAME_(n, s)

#define JSON_AST_VISITOR_TYPE_NAME \
    JSON_AST_VISITOR_MAKE_NAME(JSON_AST_VISITOR_NAME, _visitor_t)

#define JSON_AST_VISITOR_VISIT_FUNC_NAME \
    JSON_AST_VISITOR_MAKE_NAME(JSON_AST_VISITOR_NAME, _visit)

#if     JSON_AST_VISITOR_TYPE == JSON_AST_VISITOR_TABLE_TYPE
#define JSON_AST_VISITOR_FUNC_TYPE_NAME(n) \
    JSON_AST_VISITOR_MAKE_NAME(JSON_AST_VISITOR_NAME, _node_ ## n ## _func_t)
#else
#define JSON_AST_VISITOR_FUNC_TYPE_NAME(n) \
    JSON_AST_VISITOR_MAKE_NAME(JSON_AST_VISITOR_NAME, _ ## n)
#endif

#if     JSON_AST_VISITOR_TYPE == JSON_AST_VISITOR_TABLE_TYPE
#define JSON_AST_VISITOR_FUNC_TYPE_DEF(n)                  \
    typedef JSON_AST_VISITOR_RESULT_TYPE                   \
        (*JSON_AST_VISITOR_FUNC_TYPE_NAME(n))(             \
            JSON_AST_VISITOR_OBJECT_TYPE* this,            \
            const struct json_ast_ ## n ## _node_t* node);
#else
#define JSON_AST_VISITOR_FUNC_TYPE_DEF(n)                  \
    static JSON_AST_VISITOR_RESULT_TYPE                    \
        JSON_AST_VISITOR_FUNC_TYPE_NAME(n)(                \
            JSON_AST_VISITOR_OBJECT_TYPE* this,            \
            const struct json_ast_ ## n ## _node_t* node);
#endif

JSON_AST_VISITOR_FUNC_TYPE_DEF(null)
JSON_AST_VISITOR_FUNC_TYPE_DEF(boolean)
JSON_AST_VISITOR_FUNC_TYPE_DEF(number)
JSON_AST_VISITOR_FUNC_TYPE_DEF(string)
JSON_AST_VISITOR_FUNC_TYPE_DEF(object)
JSON_AST_VISITOR_FUNC_TYPE_DEF(array)

#if JSON_AST_VISITOR_TYPE == JSON_AST_VISITOR_TABLE_TYPE
struct JSON_AST_VISITOR_TYPE_NAME
{
    JSON_AST_VISITOR_FUNC_TYPE_NAME(null)    null_func;
    JSON_AST_VISITOR_FUNC_TYPE_NAME(boolean) boolean_func;
    JSON_AST_VISITOR_FUNC_TYPE_NAME(number)  number_func;
    JSON_AST_VISITOR_FUNC_TYPE_NAME(string)  string_func;
    JSON_AST_VISITOR_FUNC_TYPE_NAME(object)  object_func;
    JSON_AST_VISITOR_FUNC_TYPE_NAME(array)   array_func;
};

#define JSON_AST_VISITOR_CALL(n)         \
    (                                    \
        vis->n ## _func(obj, &(node->n)) \
    )
#else
#define JSON_AST_VISITOR_CALL(n)            \
    (                                       \
        JSON_AST_VISITOR_FUNC_TYPE_NAME(n)( \
            obj, &(node->n))                \
    )
#endif

static JSON_AST_VISITOR_RESULT_TYPE JSON_AST_VISITOR_VISIT_FUNC_NAME(
#if JSON_AST_VISITOR_TYPE == JSON_AST_VISITOR_TABLE_TYPE
    const struct json_ast_node_t* node,
    const struct JSON_AST_VISITOR_TYPE_NAME* vis,
    JSON_AST_VISITOR_OBJECT_TYPE* obj)
#else
    JSON_AST_VISITOR_OBJECT_TYPE* obj,
    const struct json_ast_node_t* node)
#endif
{
    ENSURE(node != NULL, "node is null");
#if JSON_AST_VISITOR_TYPE == JSON_AST_VISITOR_TABLE_TYPE
    ENSURE(vis != NULL, "vis is null");
#endif

    switch (node->type) {
    case json_ast_null_node_type:
        return JSON_AST_VISITOR_CALL(null);
    case json_ast_boolean_node_type:
        return JSON_AST_VISITOR_CALL(boolean);
    case json_ast_number_node_type:
        return JSON_AST_VISITOR_CALL(number);
    case json_ast_string_node_type:
        return JSON_AST_VISITOR_CALL(string);
    case json_ast_object_node_type:
        return JSON_AST_VISITOR_CALL(object);
    case json_ast_array_node_type:
        return JSON_AST_VISITOR_CALL(array);
    default:
        UNEXPECT_VAR("%d", node->type);
    }
}

#undef  JSON_AST_VISITOR_TABLE_TYPE
#undef  JSON_AST_VISITOR_STATIC_TYPE
#undef  JSON_AST_VISITOR_MAKE_NAME_
#undef  JSON_AST_VISITOR_MAKE_NAME
#undef  JSON_AST_VISITOR_FUNC_TYPE_NAME
#undef  JSON_AST_VISITOR_TYPE_NAME
#undef  JSON_AST_VISITOR_VISIT_FUNC_NAME 
#undef  JSON_AST_VISITOR_FUNC_TYPE_DEF
#undef  JSON_AST_VISITOR_CALL

#endif /* JSON_AST_NEED_VISITOR */


