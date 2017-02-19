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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json.h"
#include "json-ast.h"
#include "int-traits.h"
#include "ptr-traits.h"
#include "pool-alloc.h"
#include "common.h"
#include "error.h"

#ifdef JSON_DEBUG
#define PRINT_DEBUG_COND ast->debug
#include "debug.h"
#endif

// stev: important TODO: review all paired invocations of
// 'POP(?!_N)|TOP(?:_N)?' with 'ENSURE' as to replace them
// with new macro calls '(?:POP(?!_N)|TOP(?:_N)?)_AS'

#define CASE_STATE(n) [json_ast_ ## n ## _node_type] = #n

static const char* json_ast_node_type_get_name(
    enum json_ast_node_type_t type)
{
    static const char* const names[] = {
        CASE_STATE(null),
        CASE_STATE(boolean),
        CASE_STATE(number),
        CASE_STATE(string),
        CASE_STATE(object),
        CASE_STATE(array),
    };
    return ARRAY_NON_NULL_ELEM(names, type);
}

enum json_ast_obj_type_t
{
    json_ast_obj_node_type,
    json_ast_obj_mark_type,
};

struct json_ast_obj_t
{
    enum json_ast_obj_type_t type;
    union {
        struct json_ast_node_t*  node;
        struct json_text_pos_t   mark;
    };
};

#define STACK_NAME             json_ast
#define STACK_ELEM_TYPE struct json_ast_obj_t
#define STACK_ELEM_PRINT_DEBUG json_ast_obj_print_debug
#define STACK_ELEM_EQUAL(x, y) ((x)->type == (y)->type)
#define STACK_NEED_FIND_FIRST
#define STACK_NEED_MAX_SIZE
#include "stack-impl.h"

#define JSON_AST_STACK_PUSH(t, n)               \
    do {                                        \
        struct json_ast_obj_t __o;              \
        __o.type = json_ast_obj_ ## t ## _type; \
        __o.t = n;                              \
        STACK_PUSH(&ast->obj_stack, __o);       \
    } while (0)

#define JSON_AST_STACK_PUSH_NODE(n) JSON_AST_STACK_PUSH(node, n)
#define JSON_AST_STACK_PUSH_MARK(p) JSON_AST_STACK_PUSH(mark, p)

#define JSON_AST_STACK_SIZE()       STACK_SIZE(&ast->obj_stack)
#define JSON_AST_STACK_POP()        STACK_POP(&ast->obj_stack)
#define JSON_AST_STACK_TOP_N(n)     STACK_TOP_N(&ast->obj_stack, n)
#define JSON_AST_STACK_POP_N(n)     STACK_POP_N(&ast->obj_stack, n)

static size_t json_ast_stack_find_first_mark(
    struct json_ast_stack_t* stack)
{
    static const struct json_ast_obj_t mark = {
        .type = json_ast_obj_mark_type,
    };
    size_t n;
    bool b;

    b = json_ast_stack_find_first(stack, &mark, &n);
    ENSURE(b, "invalid obj stack: mark not found");

    return n;
}

struct json_ast_t
{
    struct json_obj_t*      json_obj;
    struct pool_alloc_t     pool_alloc;
    struct json_ast_stack_t obj_stack;
    struct json_ast_node_t* root;
    bits_t                  done: 1;
};

static void* json_ast_allocate(
    struct json_ast_t* ast, size_t size, size_t align)
{
    void* n;

    if ((n = pool_alloc_allocate(
            &ast->pool_alloc, size, align)) == NULL)
        fatal_error("AST pool alloc failed");

    memset(n, 0, size);

    return n;
}

static struct json_ast_node_t* json_ast_new_node(
    struct json_ast_t* ast)
{
    return json_ast_allocate(
        ast, sizeof(struct json_ast_node_t),
        MEM_ALIGNOF(struct json_ast_node_t));
}

static struct json_ast_node_t* json_ast_new_null_node(
    struct json_ast_t* ast,
    const struct json_text_pos_t* pos)
{
    struct json_ast_node_t* n;

    n = json_ast_new_node(ast);
    ASSERT(n != NULL);

    n->type = json_ast_null_node_type;
    n->pos = *pos;

    return n;
}

static struct json_ast_node_t* json_ast_new_boolean_node(
    struct json_ast_t* ast, bool val,
    const struct json_text_pos_t* pos)
{
    struct json_ast_node_t* n;

    n = json_ast_new_node(ast);
    ASSERT(n != NULL);

    n->type = json_ast_boolean_node_type;
    n->boolean.val = val;
    n->pos = *pos;

    return n;
}

static struct json_ast_node_t* json_ast_new_number_node(
    struct json_ast_t* ast, const struct json_ast_buf_t* buf,
    const struct json_text_pos_t* pos)
{
    struct json_ast_node_t* n;

    n = json_ast_new_node(ast);
    ASSERT(n != NULL);

    n->type = json_ast_number_node_type;
    n->number.buf = *buf;
    n->pos = *pos;

    return n;
}

static struct json_ast_node_t* json_ast_new_string_node(
    struct json_ast_t* ast, const struct json_ast_buf_t* buf,
    const struct json_text_pos_t* pos)
{
    struct json_ast_node_t* n;

    n = json_ast_new_node(ast);
    ASSERT(n != NULL);

    n->type = json_ast_string_node_type;
    n->string.buf = *buf;
    n->pos = *pos;

    return n;
}

static struct json_ast_node_t* json_ast_new_object_node(
    struct json_ast_t* ast, struct json_ast_object_arg_t* args,
    size_t size, const struct json_text_pos_t* pos)
{
    struct json_ast_node_t* n;

    n = json_ast_new_node(ast);
    ASSERT(n != NULL);

    n->type = json_ast_object_node_type;
    n->object.args = args;
    n->object.size = size;
    n->pos = *pos;

    return n;
}

static struct json_ast_node_t* json_ast_new_array_node(
    struct json_ast_t* ast, struct json_ast_node_t** args,
    size_t size, const struct json_text_pos_t* pos)
{
    struct json_ast_node_t* n;

    n = json_ast_new_node(ast);
    ASSERT(n != NULL);

    n->type = json_ast_array_node_type;
    n->array.args = args;
    n->array.size = size;
    n->pos = *pos;

    return n;
}

static bool json_ast_null(struct json_ast_t* ast)
{
    struct json_text_pos_t w;
    struct json_ast_node_t* n;

    w = json_get_current_tok_pos(ast->json_obj);
    n = json_ast_new_null_node(ast, &w);
    JSON_AST_STACK_PUSH_NODE(n);
    return true;
}

static bool json_ast_boolean(struct json_ast_t* ast,
    bool val)
{
    struct json_text_pos_t w;
    struct json_ast_node_t* n;

    w = json_get_current_tok_pos(ast->json_obj);
    n = json_ast_new_boolean_node(ast, val, &w);
    JSON_AST_STACK_PUSH_NODE(n);
    return true;
}

#define JSON_AST_ALLOCATE_ARRAY(t, n)                \
    ({                                               \
        STATIC(TYPEOF_IS_SIZET(n));                  \
        ASSERT_SIZE_MUL_NO_OVERFLOW((n), sizeof(t)); \
        json_ast_allocate(ast, (n) * sizeof(t),      \
            MEM_ALIGNOF(t[0]));                      \
    })

#define JSON_AST_INIT_BUF(b, n)         \
    do {                                \
        uchar_t* __b;                   \
        ASSERT_SIZE_INC_NO_OVERFLOW(n); \
        __b = JSON_AST_ALLOCATE_ARRAY(  \
            uchar_t, (n) + 1);          \
        memcpy(__b, val, (n));          \
        __b[(n)] = 0;                   \
        b.ptr = __b;                    \
        b.size = (n);                   \
    } while (0)

static bool json_ast_number(
    struct json_ast_t* ast, const uchar_t* val, size_t len)
{
    struct json_text_pos_t w;
    struct json_ast_node_t* n;
    struct json_ast_buf_t b;

    JSON_AST_INIT_BUF(b, len);

    w = json_get_current_tok_pos(ast->json_obj);
    n = json_ast_new_number_node(ast, &b, &w);
    JSON_AST_STACK_PUSH_NODE(n);

    return true;
}

static bool json_ast_string(
    struct json_ast_t* ast, const uchar_t* val, size_t len)
{
    struct json_text_pos_t w;
    struct json_ast_node_t* n;
    struct json_ast_buf_t b;

    JSON_AST_INIT_BUF(b, len);

    w = json_get_current_tok_pos(ast->json_obj);
    n = json_ast_new_string_node(ast, &b, &w);
    JSON_AST_STACK_PUSH_NODE(n);

    return true;
}

static bool json_ast_object_start(struct json_ast_t* ast)
{
    struct json_text_pos_t w;

    w = json_get_current_tok_pos(ast->json_obj);
    JSON_AST_STACK_PUSH_MARK(w);
    return true;
}

#define ENSURE_JSON_AST_OBJ_IS(o, n)                       \
    do {                                                   \
        ENSURE((o).type == json_ast_obj_ ## n ## _type,    \
            "invalid obj stack: element is not '" #n "'"); \
    } while (0)

#define ENSURE_JSON_AST_NODE_OBJ_IS(o, n)                       \
    do {                                                        \
        ENSURE((o).node->type == json_ast_ ## n ## _node_type,  \
            "invalid obj stack: node is not '" #n "' but '%s'", \
            json_ast_node_type_get_name((o).node->type));       \
    } while (0)

static bool json_ast_object_end(struct json_ast_t* ast)
{
    struct json_ast_obj_t o;
    struct json_text_pos_t w;
    struct json_ast_node_t* n;
    struct json_ast_object_arg_t *p = NULL, *q;
    size_t i, k, s = 0;

    i = json_ast_stack_find_first_mark(
            &ast->obj_stack);
    o = JSON_AST_STACK_TOP_N(i);
    ENSURE_JSON_AST_OBJ_IS(o, mark);
    w = o.mark;

    if (i > 0) {
        ENSURE((i % 2) == 0,
            "invalid obj stack: number of elements "
            "until mark is odd");

        s = i / 2;
        p = JSON_AST_ALLOCATE_ARRAY(
                struct json_ast_object_arg_t, s);

        for (k = i - 1, q = p; ; k -= 2, q ++) {
            o = JSON_AST_STACK_TOP_N(k);
            ENSURE_JSON_AST_OBJ_IS(o, node);
            ENSURE_JSON_AST_NODE_OBJ_IS(o, string);

            q->key = &o.node->string;
            ASSERT(
                JSON_AST_TO_NODE(q->key, string) ==
                o.node); 

            o = JSON_AST_STACK_TOP_N(k - 1);
            ENSURE_JSON_AST_OBJ_IS(o, node);

            q->val = o.node;

            if (k == 1) break;
        }
    }
    JSON_AST_STACK_POP_N(i);

    n = json_ast_new_object_node(ast, p, s, &w);
    JSON_AST_STACK_PUSH_NODE(n);

    return true;
}

static bool json_ast_array_start(struct json_ast_t* ast)
{
    struct json_text_pos_t w;

    w = json_get_current_tok_pos(ast->json_obj);
    JSON_AST_STACK_PUSH_MARK(w);
    return true;
}

static void json_ast_array_end_pos(struct json_ast_t* ast,
    size_t pos, const struct json_text_pos_t* where, bool mark)
{
    struct json_ast_obj_t o;
    struct json_ast_node_t *n, **p = NULL, **q;
    size_t k, s = pos;

    if (pos > 0) {
        p = JSON_AST_ALLOCATE_ARRAY(
                struct json_ast_node_t*, pos);

        for (k = pos - 1, q = p; ; k --) {
            o = JSON_AST_STACK_TOP_N(k);
            ENSURE_JSON_AST_OBJ_IS(o, node);

            *q ++ = o.node;

            if (k == 0) break;
        }
    }
    if (!mark) {
        ASSERT_SIZE_DEC_NO_OVERFLOW(s);
        s --;
    }
    JSON_AST_STACK_POP_N(s);

    n = json_ast_new_array_node(ast, p, pos, where);
    JSON_AST_STACK_PUSH_NODE(n);
}

static bool json_ast_array_end(struct json_ast_t* ast)
{
    struct json_ast_obj_t o;
    struct json_text_pos_t w;
    size_t i;

    i = json_ast_stack_find_first_mark(
            &ast->obj_stack);
    o = JSON_AST_STACK_TOP_N(i);
    ENSURE_JSON_AST_OBJ_IS(o, mark);
    w = o.mark;

    json_ast_array_end_pos(ast, i, &w, true);
    return true;
}

static struct json_handler_t handler = {
    .null_func         = (json_null_func_t)         json_ast_null,
    .boolean_func      = (json_boolean_func_t)      json_ast_boolean,
    .number_func       = (json_number_func_t)       json_ast_number,
    .string_func       = (json_string_func_t)       json_ast_string,
    .object_start_func = (json_object_start_func_t) json_ast_object_start,
    .object_key_func   = (json_object_key_func_t)   json_ast_string,
    .object_end_func   = (json_object_end_func_t)   json_ast_object_end,
    .array_start_func  = (json_array_start_func_t)  json_ast_array_start,
    .array_end_func    = (json_array_end_func_t)    json_ast_array_end,
};

struct json_ast_t* json_ast_create(
    const struct json_ast_sizes_t* sizes)
{
    struct json_ast_t* ast;

    JSON_AST_SIZES_VALIDATE(sizes);

    ast = malloc(sizeof(struct json_ast_t));
    ENSURE(ast != NULL, "malloc failed");

    memset(ast, 0, sizeof(struct json_ast_t));

    STATIC(
        TYPES_COMPATIBLE(
            json_object_key_func_t,
            json_string_func_t));
    ASSERT(
        handler.object_key_func == 
        handler.string_func);

    ast->json_obj = json_obj_create(&handler, ast, &sizes->obj);

    pool_alloc_init(
        &ast->pool_alloc, sizes->pool_size);
    json_ast_stack_init(
        &ast->obj_stack, sizes->obj.stack_max, sizes->obj.stack_init);

    return ast;
}

void json_ast_destroy(struct json_ast_t* ast)
{
    json_ast_stack_done(&ast->obj_stack);
    pool_alloc_done(&ast->pool_alloc);
    json_obj_destroy(ast->json_obj);
    free(ast);
}

bool json_ast_config_get_param(
    struct json_ast_t* ast, enum json_config_param_t param)
{
    return json_config_get_param(ast->json_obj, param);
}

void json_ast_config_set_param(
    struct json_ast_t* ast, enum json_config_param_t param,
    bool val)
{
    json_config_set_param(ast->json_obj, param, val);
}

const struct json_ast_node_t* json_ast_get_root(
    const struct json_ast_t* ast)
{
    return ast->root;
}

enum json_parse_status_t json_ast_parse(
    struct json_ast_t* ast, const uchar_t* buf, size_t len)
{
    return json_parse(ast->json_obj, buf, len);
}

enum json_parse_status_t json_ast_parse_done(
    struct json_ast_t* ast)
{
    enum json_parse_status_t r;
    struct json_text_pos_t w = {1, 1};
    struct json_ast_obj_t o;
    size_t n, c;
    bool m;

    r = json_parse_done(ast->json_obj);
    if (r != json_parse_status_ok)
        return r;

    // stev: ensure that our client can
    // do no harm by calling us twice or
    // more times in a sequence
    if (ast->done)
        return json_parse_status_ok;

    c = json_parse_get_config(ast->json_obj);
    m = c & (1 << json_allow_multi_objects_config);
    n = JSON_AST_STACK_SIZE();

    if ((!m && n != 1) || (m && n == 0))
        UNEXPECT_ERR("invalid obj stack: size is %zu", n);

    if (n > 1)
        json_ast_array_end_pos(ast, n, &w, false);

    n = JSON_AST_STACK_SIZE();
    ASSERT(n == 1);

    o = JSON_AST_STACK_POP();
    ENSURE_JSON_AST_OBJ_IS(o, node);

    ast->root = o.node;
    ast->done = true;

    return r;
}

#define JSON_CALL_VISITOR(n)             \
    (                                    \
        vis->n ## _func(obj, &(node->n)) \
    )

void* json_ast_visit(
    const struct json_ast_node_t* node,
    const struct json_ast_visitor_t* vis,
    void* obj)
{
    ENSURE(node != NULL, "node is null");
    ENSURE(vis != NULL, "vis is null");

    switch (node->type) {
    case json_ast_null_node_type:
        return JSON_CALL_VISITOR(null);
    case json_ast_boolean_node_type:
        return JSON_CALL_VISITOR(boolean);
    case json_ast_number_node_type:
        return JSON_CALL_VISITOR(number);
    case json_ast_string_node_type:
        return JSON_CALL_VISITOR(string);
    case json_ast_object_node_type:
        return JSON_CALL_VISITOR(object);
    case json_ast_array_node_type:
        return JSON_CALL_VISITOR(array);
    default:
        UNEXPECT_VAR("%d", node->type);
    }

    return NULL;
}

const struct json_error_info_t* json_ast_get_error_info(
    struct json_ast_t* ast)
{
    return json_get_error_info(ast->json_obj);
}

bool json_ast_get_is_error(struct json_ast_t* ast)
{
    return json_get_is_error(ast->json_obj);
}

struct json_error_pos_t json_ast_get_error_pos(
    struct json_ast_t* ast)
{
    return json_get_error_pos(ast->json_obj);
}

const struct json_file_info_t* json_ast_get_error_file(
    struct json_ast_t* ast)
{
    return json_get_error_file(ast->json_obj);
}

void json_ast_print_error_desc(struct json_ast_t* ast,
    FILE* file)
{
    json_print_error_desc(ast->json_obj, file);
}

#ifdef JSON_DEBUG
void json_ast_print_error_debug(struct json_ast_t* ast,
    FILE* file)
{
    json_print_error_debug(ast->json_obj, file);
}
#endif


