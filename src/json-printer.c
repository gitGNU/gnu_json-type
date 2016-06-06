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

#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "lib/json.h"
#include "lib/pretty-print.h"

#include "obj.h"
#include "common.h"
#include "json-printer.h"

#ifdef DEBUG
static void print_debug(struct obj_json_printer_t* this)
{
    if (this->debug > 3)
        print_fmt("<%d,%d,%zu>",
            this->first_nl,
            this->skip_indent,
            this->indent_level);
}
#endif

static void print_indent(struct obj_json_printer_t* this)
{
    size_t n;
    bool s;

#ifdef DEBUG
    print_debug(this);
#endif

    s = this->skip_indent;
    this->skip_indent = false;
    if (s) return;

    if (this->first_nl)
        this->first_nl = false;
    else
        print_chr('\n');
    if (!this->print_dots) {
        n = SIZE_MUL(this->indent_level, SZ(4));
        print_fmt("%*s", SIZE_AS_INT(n), "");
    }
    else {
        n = this->indent_level;
        while (n --) {
            print_fmt("%-4c", '.');
        }
    }
}

static void print_indent_opt(struct obj_json_printer_t* this,
    bool empty)
{
    if (!empty)
        print_indent(this);
    else
    if (this->skip_indent)
        this->skip_indent = false;
}

static void print_val(struct obj_json_printer_t* this, const char* val)
{
#ifdef DEBUG
    print_debug(this);
#endif

    print_str(val);
    this->printed_out = true;
    this->skip_indent = false;
}

static void print_repr_val(
    struct obj_json_printer_t* this, const uchar_t* str, size_t len,
    bool quotes)
{
    enum pretty_print_repr_flags_t f = 0;

#ifdef DEBUG
    print_debug(this);
#endif

    if (quotes)
        f |= pretty_print_repr_print_quotes;
    if (this->quote_cntrl)
        f |= pretty_print_repr_unicode_quote_cntrl;
    pretty_print_repr(OBJ_SUPER(this)->output, str, len, f);
    this->printed_out = true;
    this->skip_indent = false;
}

static void print_null_val(struct obj_json_printer_t* this)
{
    print_val(this, "null");
}

static void print_bool_val(struct obj_json_printer_t* this, bool val)
{
    print_val(this, val ? "true" : "false");
}

static void print_num_val(
    struct obj_json_printer_t* this, const uchar_t* str, size_t len)
{
    print_repr_val(this, str, len, false);
}

static void print_str_val(
    struct obj_json_printer_t* this, const uchar_t* str, size_t len)
{
    print_repr_val(this, str, len, true);
}

static void print_key(
    struct obj_json_printer_t* this, const uchar_t* str, size_t len,
    bool terse)
{
    print_str_val(this, str, len);
    print_str(terse ? ":" : ": ");
}

static bool pretty_null(struct obj_json_printer_t* this)
{
    print_indent(this);
    print_null_val(this);

    return true;
}

static bool pretty_bool(struct obj_json_printer_t* this, bool val)
{
    print_indent(this);
    print_bool_val(this, val);

    return true;
}

static bool pretty_number(
    struct obj_json_printer_t* this, const uchar_t* str, size_t len)
{
    print_indent(this);
    print_num_val(this, str, len);

    return true;
}

static bool pretty_string(
    struct obj_json_printer_t* this, const uchar_t* str, size_t len)
{
    print_indent(this);
    print_str_val(this, str, len);

    return true;
}

static bool pretty_object_key(
    struct obj_json_printer_t* this, const uchar_t* str, size_t len)
{
    print_indent(this);
    print_key(this, str, len, false);
    this->skip_indent = true;

    return true;
}

static bool pretty_object_start(struct obj_json_printer_t* this)
{
    print_indent(this);
    print_chr('{');
    this->printed_out = true;
    this->indent_level ++;

    return true;
}

static bool pretty_object_end(struct obj_json_printer_t* this)
{
    this->indent_level --;
    print_indent(this);
    print_chr('}');
    this->printed_out = true;

    return true;
}

static void pretty_object_end_opt(struct obj_json_printer_t* this,
    bool empty)
{
    this->indent_level --;
    print_indent_opt(this, empty);
    print_chr('}');
    this->printed_out = true;
}

static bool pretty_array_start(struct obj_json_printer_t* this)
{
    print_indent(this);
    print_chr('[');
    this->printed_out = true;
    this->indent_level ++;

    return true;
}

static bool pretty_array_end(struct obj_json_printer_t* this)
{
    this->indent_level --;
    print_indent(this);
    print_chr(']');
    this->printed_out = true;

    return true;
}

static void pretty_array_end_opt(struct obj_json_printer_t* this,
    bool empty)
{
    this->indent_level --;
    print_indent_opt(this, empty);
    print_chr(']');
    this->printed_out = true;
}

static bool pretty_aggr_sep(struct obj_json_printer_t* this)
{
    print_chr(',');
    this->printed_out = true;

    return true;
}

static bool terse_null(struct obj_json_printer_t* this)
{
    print_null_val(this);
    return true;
}

static bool terse_bool(struct obj_json_printer_t* this, bool val)
{
    print_bool_val(this, val);
    return true;
}

static bool terse_number(
    struct obj_json_printer_t* this, const uchar_t* str, size_t len)
{
    print_num_val(this, str, len);
    return true;
}

static bool terse_string(
    struct obj_json_printer_t* this, const uchar_t* str, size_t len)
{
    print_str_val(this, str, len);
    return true;
}

static bool terse_object_key(
    struct obj_json_printer_t* this, const uchar_t* str, size_t len)
{
    print_key(this, str, len, true);
    return true;
}

static bool terse_object_start(struct obj_json_printer_t* this)
{
    print_chr('{');
    this->printed_out = true;
    return true;
}

static bool terse_object_end(struct obj_json_printer_t* this)
{
    print_chr('}');
    this->printed_out = true;
    return true;
}

static bool terse_array_start(struct obj_json_printer_t* this)
{
    print_chr('[');
    this->printed_out = true;
    return true;
}

static bool terse_array_end(struct obj_json_printer_t* this)
{
    print_chr(']');
    this->printed_out = true;
    return true;
}

static bool terse_aggr_sep(struct obj_json_printer_t* this)
{
    print_chr(',');
    this->printed_out = true;
    return true;
}

static bool terse_value_sep(struct obj_json_printer_t* this)
{
    if (this->newline_sep) {
        print_chr('\n');
        this->printed_out = true;
    }
    return true;
}

static void ast_visit_node(struct obj_json_printer_t* this,
    const struct json_ast_node_t* node);

static void* pretty_node_null(struct obj_json_printer_t* this,
    const struct json_ast_null_node_t* node UNUSED)
{
    pretty_null(this);
    return NULL;
}

static void* pretty_node_bool(struct obj_json_printer_t* this,
    const struct json_ast_boolean_node_t* node)
{
    pretty_bool(this, node->val);
    return NULL;
}

static void* pretty_node_number(struct obj_json_printer_t* this, 
    const struct json_ast_number_node_t* node)
{
    pretty_number(this, node->buf.ptr, node->buf.size);
    return NULL;
}

static void* pretty_node_string(struct obj_json_printer_t* this,
    const struct json_ast_string_node_t* node)
{
    pretty_string(this, node->buf.ptr, node->buf.size);
    return NULL;
}

static void* pretty_node_object(struct obj_json_printer_t* this,
    const struct json_ast_object_node_t* node)
{
    const struct json_ast_object_arg_t *p, *e;
    size_t n = node->size;

    pretty_object_start(this);

    for (p = node->args, e = p + n; p < e; p ++, n --) {
        pretty_object_key(this, p->key->buf.ptr, p->key->buf.size);
        ast_visit_node(this, p->val);
        if (n > 1)
            pretty_aggr_sep(this);
    }

    pretty_object_end_opt(this, node->size == 0);

    return NULL;
}

static void* pretty_node_array(struct obj_json_printer_t* this,
    const struct json_ast_array_node_t* node)
{
    struct json_ast_node_t *const* p, *const* e;
    size_t n = node->size;

    pretty_array_start(this);

    for (p = node->args, e = p + n; p < e; p ++, n --) {
        ast_visit_node(this, *p);
        if (n > 1)
            pretty_aggr_sep(this);
    }

    pretty_array_end_opt(this, node->size == 0);

    return NULL;
}

static void ast_node_enter(struct obj_json_printer_t* this,
    bool type, const char* name, char what)
{
    print_indent(this);
    if (name)
        print_fmt("%s%s %s%c",
            type ? "# " : ".", name,
            !type ? "= " : "", what);
    else
        print_chr(what);
    this->printed_out = true;
    this->indent_level ++;
}

static void ast_node_leave(struct obj_json_printer_t* this,
    char what, bool empty)
{
    this->indent_level --;
    if (!empty) print_indent(this);
    print_chr(what);
    this->printed_out = true;
}

static void ast_node_enter_obj(struct obj_json_printer_t* this,
    const char* name)
{
    ast_node_enter(this, true, name, '{');
}

static void ast_node_leave_obj(struct obj_json_printer_t* this)
{
    ast_node_leave(this, '}', false);
}

static void ast_node_enter_list(struct obj_json_printer_t* this,
    const char* name)
{
    ast_node_enter(this, false, name, '[');
}

static void ast_node_leave_list(struct obj_json_printer_t* this,
    bool empty)
{
    ast_node_leave(this, ']', empty);
}

static void ast_node_empty_obj(struct obj_json_printer_t* this,
    const char* name)
{
    ast_node_enter_obj(this, name);
    ast_node_leave(this, '}', true);
}

static void ast_node_prop(struct obj_json_printer_t* this,
    const char* name)
{
    print_indent(this);
    print_fmt(".%s = ", name);
    this->printed_out = true;
    this->skip_indent = true;
}

static void ast_node_bool_prop(struct obj_json_printer_t* this,
    const char* name, bool val)
{
    print_indent(this);
    print_fmt(".%s = ", name);
    this->printed_out = true;
    print_bool_val(this, val);
}

static void ast_node_str_prop(struct obj_json_printer_t* this,
    const char* name, const uchar_t* str, size_t len)
{
    print_indent(this);
    print_fmt(".%s = ", name);
    this->printed_out = true;
    print_str_val(this, str, len);
}

static void ast_node_size_prop(struct obj_json_printer_t* this,
    const char* name, size_t val)
{
    print_indent(this);
    print_fmt(".%s = %zu", name, val);
    this->printed_out = true;
}

static void* ast_node_null(struct obj_json_printer_t* this,
    const struct json_ast_null_node_t* node UNUSED)
{
    ast_node_empty_obj(this, "null");

    return NULL;
}

static void* ast_node_bool(struct obj_json_printer_t* this,
    const struct json_ast_boolean_node_t* node)
{
    ast_node_enter_obj(this, "boolean");
    ast_node_bool_prop(this, "val", node->val);
    ast_node_leave_obj(this);

    return NULL;
}

static void ast_node_buf_prop(struct obj_json_printer_t* this,
    const char* name, const struct json_ast_buf_t* buf)
{
    ast_node_enter_obj(this, name);

    ast_node_prop(this, "buf");

    ast_node_enter_obj(this, NULL);

    ast_node_str_prop(this, "ptr", buf->ptr, buf->size);
    ast_node_size_prop(this, "size", buf->size);

    ast_node_leave_obj(this);

    ast_node_leave_obj(this);
}

static void* ast_node_number(struct obj_json_printer_t* this, 
    const struct json_ast_number_node_t* node)
{
    ast_node_buf_prop(this, "number", &node->buf);

    return NULL;
}

static void* ast_node_string(struct obj_json_printer_t* this,
    const struct json_ast_string_node_t* node)
{
    ast_node_buf_prop(this, "string", &node->buf);

    return NULL;
}

static void* ast_node_object(struct obj_json_printer_t* this,
    const struct json_ast_object_node_t* node)
{
    const struct json_ast_object_arg_t *p, *e;

    ast_node_enter_obj(this, "object");

    ast_node_size_prop(this, "size", node->size);

    ast_node_enter_list(this, "args");
    for (p = node->args, e = p + node->size; p < e; p ++) {
        ast_node_enter_obj(this, NULL);

        ast_node_prop(this, "key");
        ast_node_string(this, p->key);

        ast_node_prop(this, "val");
        ast_visit_node(this, p->val);

        ast_node_leave_obj(this);
    }
    ast_node_leave_list(this, !node->size);

    ast_node_leave_obj(this);

    return NULL;
}

static void* ast_node_array(struct obj_json_printer_t* this,
    const struct json_ast_array_node_t* node)
{
    struct json_ast_node_t *const* p, *const* e;

    ast_node_enter_obj(this, "array");

    ast_node_size_prop(this, "size", node->size);

    ast_node_enter_list(this, "args");
    for (p = node->args, e = p + node->size; p < e; p ++)
        ast_visit_node(this, *p);
    ast_node_leave_list(this, !node->size);

    ast_node_leave_obj(this);

    return NULL;
}

#define PRINT_PLAIN_TYPE_BEGIN()                              \
    do {                                                      \
        pretty_object_start(this);                            \
                                                              \
        pretty_object_key(this, (const uchar_t*) "plain", 5);

#define PRINT_PLAIN_TYPE_END()                                \
        pretty_object_end(this);                              \
    } while (0)

static void print_aggregate_type_start(struct obj_json_printer_t* this,
    const char* name)
{
    pretty_object_start(this);
    pretty_object_key(this, (const uchar_t*) "type", 4);
    pretty_string(this, (const uchar_t*) name, strlen(name));
    pretty_aggr_sep(this);
    pretty_object_key(this, (const uchar_t*) "args", 4);
}

static void print_aggregate_type_end(struct obj_json_printer_t* this)
{
    pretty_object_end(this);
}

static void* ast_type_null(struct obj_json_printer_t* this,
    const struct json_ast_null_node_t* node UNUSED)
{
    PRINT_PLAIN_TYPE_BEGIN();
    pretty_null(this);
    PRINT_PLAIN_TYPE_END();
    return NULL;
}

static void* ast_type_bool(struct obj_json_printer_t* this,
    const struct json_ast_boolean_node_t* node)
{
    PRINT_PLAIN_TYPE_BEGIN();
    pretty_bool(this, node->val);
    PRINT_PLAIN_TYPE_END();
    return NULL;
}

static void* ast_type_number(struct obj_json_printer_t* this, 
    const struct json_ast_number_node_t* node)
{
    PRINT_PLAIN_TYPE_BEGIN();
    pretty_number(this, node->buf.ptr, node->buf.size);
    PRINT_PLAIN_TYPE_END();
    return NULL;
}

static void* ast_type_string(struct obj_json_printer_t* this,
    const struct json_ast_string_node_t* node)
{
    PRINT_PLAIN_TYPE_BEGIN();
    pretty_string(this, node->buf.ptr, node->buf.size);
    PRINT_PLAIN_TYPE_END();
    return NULL;
}

static void* ast_type_object(struct obj_json_printer_t* this,
    const struct json_ast_object_node_t* node)
{
    const struct json_ast_object_arg_t *p, *e;
    size_t n = node->size;

    print_aggregate_type_start(this, "object");

    pretty_array_start(this);
    for (p = node->args, e = p + n; p < e; p ++, n --) {
        pretty_object_start(this);

        pretty_object_key(this, (const uchar_t*) "name", 4);
        pretty_string(this, p->key->buf.ptr, p->key->buf.size);
        pretty_aggr_sep(this);

        pretty_object_key(this, (const uchar_t*) "type", 4);
        ast_visit_node(this, p->val);

        pretty_object_end(this);

        if (n > 1)
            pretty_aggr_sep(this);
    }
    pretty_array_end_opt(this, node->size == 0);

    print_aggregate_type_end(this);

    return NULL;
}

static void* ast_type_array(struct obj_json_printer_t* this,
    const struct json_ast_array_node_t* node)
{
    struct json_ast_node_t *const* p, *const* e;
    size_t n = node->size;

    print_aggregate_type_start(this, "array");

    pretty_array_start(this);
    for (p = node->args, e = p + n; p < e; p ++, n --) {
        ast_visit_node(this, *p);
        if (n > 1)
            pretty_aggr_sep(this);
    }
    pretty_array_end_opt(this, node->size == 0);

    print_aggregate_type_end(this);

    return NULL;
}

static struct json_handler_t pretty_handler = {
    .null_func         = (json_null_func_t)         pretty_null,
    .boolean_func      = (json_boolean_func_t)      pretty_bool,
    .number_func       = (json_number_func_t)       pretty_number,
    .string_func       = (json_string_func_t)       pretty_string,
    .object_start_func = (json_object_start_func_t) pretty_object_start,
    .object_key_func   = (json_object_key_func_t)   pretty_object_key,
    .object_sep_func   = (json_object_sep_func_t)   pretty_aggr_sep,
    .object_end_func   = (json_object_end_func_t)   pretty_object_end,
    .array_start_func  = (json_array_start_func_t)  pretty_array_start,
    .array_sep_func    = (json_array_sep_func_t)    pretty_aggr_sep,
    .array_end_func    = (json_array_end_func_t)    pretty_array_end,
};

static struct json_handler_t terse_handler = {
    .null_func         = (json_null_func_t)         terse_null,
    .boolean_func      = (json_boolean_func_t)      terse_bool,
    .number_func       = (json_number_func_t)       terse_number,
    .string_func       = (json_string_func_t)       terse_string,
    .object_start_func = (json_object_start_func_t) terse_object_start,
    .object_key_func   = (json_object_key_func_t)   terse_object_key,
    .object_sep_func   = (json_object_sep_func_t)   terse_aggr_sep,
    .object_end_func   = (json_object_end_func_t)   terse_object_end,
    .array_start_func  = (json_array_start_func_t)  terse_array_start,
    .array_sep_func    = (json_array_sep_func_t)    terse_aggr_sep,
    .array_end_func    = (json_array_end_func_t)    terse_array_end,
    .value_sep_func    = (json_value_sep_func_t)    terse_value_sep,
};

static struct json_ast_visitor_t pretty_node_visitor = {
    .null_func    = (json_ast_node_null_func_t)     pretty_node_null,
    .boolean_func = (json_ast_node_boolean_func_t)  pretty_node_bool,
    .number_func  = (json_ast_node_number_func_t)   pretty_node_number,
    .string_func  = (json_ast_node_string_func_t)   pretty_node_string,
    .object_func  = (json_ast_node_object_func_t)   pretty_node_object,
    .array_func   = (json_ast_node_array_func_t)    pretty_node_array,
};

static struct json_ast_visitor_t ast_node_visitor = {
    .null_func    = (json_ast_node_null_func_t)     ast_node_null,
    .boolean_func = (json_ast_node_boolean_func_t)  ast_node_bool,
    .number_func  = (json_ast_node_number_func_t)   ast_node_number,
    .string_func  = (json_ast_node_string_func_t)   ast_node_string,
    .object_func  = (json_ast_node_object_func_t)   ast_node_object,
    .array_func   = (json_ast_node_array_func_t)    ast_node_array,
};

static struct json_ast_visitor_t ast_type_visitor = {
    .null_func    = (json_ast_node_null_func_t)     ast_type_null,
    .boolean_func = (json_ast_node_boolean_func_t)  ast_type_bool,
    .number_func  = (json_ast_node_number_func_t)   ast_type_number,
    .string_func  = (json_ast_node_string_func_t)   ast_type_string,
    .object_func  = (json_ast_node_object_func_t)   ast_type_object,
    .array_func   = (json_ast_node_array_func_t)    ast_type_array,
};

static void ast_visit_node(struct obj_json_printer_t* this,
    const struct json_ast_node_t* node)
{
    json_ast_visit(node, this->visitor, this);
}

static void ast_print(struct obj_json_printer_t* this)
{
    struct json_ast_t* a =
        obj_json_base_get_ast(JSON_BASE(this));
    const struct json_ast_node_t* n = 
        json_ast_get_root(a);

    ASSERT(n != NULL);

    ast_visit_node(this, n);
}

static void obj_json_printer_init(
    struct obj_json_printer_t* this, const struct obj_type_t* type,
    const struct options_t* opts)
{
    obj_init(OBJ_SUPER(this), type);
    OBJ_INIT(this, type);

    ASSERT(pretty_handler.value_sep_func == NULL);

    if (opts->print_type == options_pretty_print_type ||
        opts->print_type == options_terse_print_type)
        (opts->type_check != options_type_check_none_type
            ? obj_json_base_init_type
            : obj_json_base_init_obj)(
                JSON_BASE(this), opts, 
                opts->print_type ==  options_pretty_print_type
                ? &pretty_handler : &terse_handler,
                this);
    else
    if (opts->print_type == options_from_ast_print_type ||
        opts->print_type == options_type_print_type ||
        opts->print_type == options_ast_print_type)
        obj_json_base_init_ast(JSON_BASE(this), opts);
    else
        UNEXPECT_ERR(
            "invalid print-type: %d",
            opts->print_type);

    this->visitor =
          opts->print_type == options_from_ast_print_type
        ? &pretty_node_visitor
        : opts->print_type == options_type_print_type
        ? &ast_type_visitor
        : opts->print_type == options_ast_print_type
        ? &ast_node_visitor
        : NULL;
    this->first_nl =
        opts->print_type != options_terse_print_type;
    this->printed_out = false;
    this->skip_indent = false;
    this->quote_cntrl = opts->quote_cntrl;
    this->print_dots = opts->print_dots;
    this->newline_sep = opts->newline_sep;
#ifdef DEBUG
    this->debug = BITS_TRUNC_BITS(opts->debug_printer, debug_bits);
#endif
    this->indent_level = 0;
}

static void obj_json_printer_done(struct obj_json_printer_t* this)
{
    obj_json_base_done(JSON_BASE(this));

    obj_done(OBJ_SUPER(this));
}

static int obj_json_printer_run(struct obj_json_printer_t* this)
{
    int r;

    r = obj_json_base_run(JSON_BASE(this));

    if (!r) {
        if (this->visitor != NULL) 
            ast_print(this);
    }

    if (!this->first_nl && this->printed_out)
        print_chr('\n');

    return r;
}

OBJ_IMPL(
    obj_json_printer,
    obj_json_printer_t,
    obj_json_printer_init,
    obj_json_printer_done,
    obj_json_printer_run
);

