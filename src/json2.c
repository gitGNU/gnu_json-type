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

#include <stdlib.h>
#include <string.h>

#include "lib/json.h"

#include "obj.h"
#include "common.h"
#include "json2.h"

#ifdef DEBUG
#define PRINT_DEBUG_COND this->debug
#include "debug.h"
#endif

// stev: TODO: add an option to output object types in the form of
// PATH_TO_OBJECT@TYPE=VALUE, where TYPE is '0' for 'null', 'b' for
// 'boolean', 'n' for 'number', 's' for 'string', 'o' for 'object' and
// 'a' for 'array'.

// stev: TODO: add an option for extra checks of object keys: not allowed
// to contain '/', '=' and '@' chars and only to contain alphanumeric
// chars (that are chars in the set 'isalnum')

struct obj_json2_string_t
{
    size_t size;
    struct obj_json2_string_t* prev;
    struct obj_json2_string_t* next;
    uchar_t ptr[0];
};

struct obj_json2_string_stack_t
{
    struct obj_json2_string_t* top;
    char* end;
    char* ptr;
    char buf[0];
};

#define ASSERT_STRING_STACK_INVARIANTS(stack) \
    do {                                      \
        ASSERT(stack->end > stack->buf);      \
        ASSERT(stack->ptr >= stack->buf);     \
        ASSERT(stack->ptr <= stack->end);     \
    } while (0)

static struct obj_json2_string_stack_t* create_string_stack(
    size_t size)
{
    struct obj_json2_string_stack_t* stack;
    const size_t n = sizeof(struct obj_json2_string_stack_t);
    size_t s;

    ASSERT(size > 0);

    VERIFY_SIZE_ADD_NO_OVERFLOW(n, size);
    s = n + size;

    if (!(stack = malloc(s)))
        OOM_ERROR("malloc failed creating obj_json2_string_stack_t");

    memset(stack, 0, s);

    stack->end = stack->buf + size;
    stack->ptr = stack->buf;

    return stack;
}

static void destroy_string_stack(
    struct obj_json2_string_stack_t* stack)
{
    free(stack);
}

static void push_string(
    struct obj_json2_string_stack_t* stack,
    const uchar_t* str,
    size_t len)
{
    const size_t n =
        sizeof(struct obj_json2_string_t);
    const size_t m =
        MEM_ALIGNOF(struct obj_json2_string_t);
    struct obj_json2_string_t* r;
    size_t s = len, u, a;

    ASSERT_STRING_STACK_INVARIANTS(stack);

    u = PTR_DIFF(stack->end, stack->ptr);
    a = PTR_ALIGN_SIZE(stack->ptr, m);

    ASSERT_SIZE_INC_NO_OVERFLOW(s);
    s ++;

    ASSERT_SIZE_ADD_NO_OVERFLOW(s, n);
    s += n;

    ASSERT_SIZE_ADD_NO_OVERFLOW(s, a);
    s += a;

    if (s > u)
        fatal_error("string stack overflow (by %zu bytes)",
            SIZE_SUB(s, u));

    r = (struct obj_json2_string_t*) (stack->ptr + a);
    r->prev = stack->top;
    if (r->prev != NULL) {
        r->next = r->prev->next;
        r->next->prev = r;
        r->prev->next = r;
    }
    else {
        r->next = r;
        r->prev = r;
    }
    r->size = s;

    memcpy(r->ptr, str, len);
    r->ptr[len] = 0;

    stack->ptr += s;
    stack->top = r;
}

static void pop_string(
    struct obj_json2_string_stack_t* stack)
{
    const struct obj_json2_string_t* r;
    size_t u;

    ASSERT_STRING_STACK_INVARIANTS(stack);

    r = stack->top;
    if (r == NULL)
        return;

    u = PTR_DIFF(stack->ptr, stack->buf);
    ASSERT(u >= r->size);

    stack->ptr -= r->size;
    if (r->next != r) {
        r->prev->next = r->next;
        r->next->prev = r->prev;
        stack->top = r->prev;
    }
    else
        stack->top = NULL;
}

static void set_last(
    struct obj_json2_t* this,
    const uchar_t* str, size_t len)
{
    if (this->last)
        pop_string(this->stack);
    push_string(this->stack, str, len);
    this->last = true;
}

static void push_last(struct obj_json2_t* this)
{
    ASSERT(this->last);
    this->last = false;
}

static void pop_last(struct obj_json2_t* this)
{
    pop_string(this->stack);
    this->last = true;
}

static void print_path(struct obj_json2_t* this)
{
    const struct obj_json2_string_t* s;

    s = this->stack->top;
    if (s == NULL)
        return;

    do {
        s = s->next;
        print_fmt("/%s", s->ptr);
    } while (s != this->stack->top);
}

static void print_path_nl(struct obj_json2_t* this)
{
    print_path(this);
    print_chr('\n');
}

static void print_bool(struct obj_json2_t* this, bool val)
{
    print_path(this);
    print_fmt("=%d\n", val);
}

static void print_number(
    struct obj_json2_t* this, const uchar_t* str, size_t len)
{
    print_path(this);
    print_fmt("=%.*s\n", SIZE_AS_INT(len), str);
}

static void print_string(
    struct obj_json2_t* this, const uchar_t* str, size_t len)
{
    const uchar_t* e = str + len;
    size_t s;
    bool n;

    do {
        ASSERT(str <= e);
        ASSERT(str + len == e);

        if ((n = !mem_find(str, len, '\n', &s)))
            s = len;
        ASSERT(s <= len);

        print_path(this);
        print_fmt("=%.*s\n", SIZE_AS_INT(s), str);

        if (n)
            break;

        str += ++ s;
        len -= s;
    }
    while (len);
}

static bool json2_null(struct obj_json2_t* this)
{
    print_path_nl(this);
    return true;
}

static bool json2_bool(struct obj_json2_t* this, bool val)
{
    print_bool(this, val);
    return true;
}

static bool json2_number(
    struct obj_json2_t* this, const uchar_t* str, size_t len)
{
    print_number(this, str, len);
    return true;
}

static bool json2_string(
    struct obj_json2_t* this, const uchar_t* str, size_t len)
{
    print_string(this, str, len);
    return true;
}

static bool json2_object_key(
    struct obj_json2_t* this, const uchar_t* str, size_t len)
{
    set_last(this, str, len);
    return true;
}

static bool json2_object_start(struct obj_json2_t* this)
{
    push_last(this);
    return true;
}

static bool json2_object_end(struct obj_json2_t* this)
{
    pop_last(this);
    return true;
}

static bool json2_array_sep(struct obj_json2_t* this)
{
    print_path_nl(this);
    return true;
}

static bool json2_value_sep(struct obj_json2_t* this)
{
    print_chr('\n');
    return true;
}

static struct json_handler_t handler = {
    .null_func         = (json_null_func_t)         json2_null,
    .boolean_func      = (json_boolean_func_t)      json2_bool,
    .number_func       = (json_number_func_t)       json2_number,
    .string_func       = (json_string_func_t)       json2_string,
    .object_start_func = (json_object_start_func_t) json2_object_start,
    .object_key_func   = (json_object_key_func_t)   json2_object_key,
    .object_end_func   = (json_object_end_func_t)   json2_object_end,
    .array_sep_func    = (json_array_sep_func_t)    json2_array_sep,
    .value_sep_func    = (json_value_sep_func_t)    json2_value_sep
};

static void obj_json2_init(
    struct obj_json2_t* this, const struct obj_type_t* type,
    const struct options_t* opts)
{
    obj_init(OBJ_SUPER(this), type);
    OBJ_INIT(this, type);

    ASSERT(handler.array_start_func == NULL);
    ASSERT(handler.array_end_func == NULL);

    (opts->type_check != options_type_check_none_type
        ? obj_json_base_init_type
        : obj_json_base_init_obj)(
            JSON_BASE(this), opts, &handler, this);

    this->stack = create_string_stack(VERIFY_SIZE_NOT_NULL(
        opts->sizes_json2_stack_size));

    this->last = true;
}

static void obj_json2_done(struct obj_json2_t* this)
{
    destroy_string_stack(this->stack);

    obj_json_base_done(JSON_BASE(this));

    obj_done(OBJ_SUPER(this));
}

static int obj_json2_run(struct obj_json2_t* this)
{
    return obj_json_base_run(JSON_BASE(this));
}

OBJ_IMPL(
    obj_json2,
    obj_json2_t,
    obj_json2_init,
    obj_json2_done,
    obj_json2_run
);


