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

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

#include "lib/json.h"
#include "lib/pretty-print.h"

#include "obj.h"
#include "common.h"
#include "json-base.h"
#include "json-intf.h"

#ifdef DEBUG
#define PRINT_DEBUG_COND this->debug
#include "debug.h"
#endif

static void obj_json_base_init_base(
    struct obj_json_base_t* this, const struct options_t* opts)
{
    VERIFY_SIZE_DEC_NO_OVERFLOW(opts->sizes_error_buf_max);
    VERIFY_SIZE_MUL_NO_OVERFLOW(opts->sizes_error_ctxt_size, SZ(2));
    VERIFY(2 * opts->sizes_error_ctxt_size <= opts->sizes_error_buf_max - 1);

    VERIFY(opts->sizes_input_buf_size <= SSIZE_MAX);

    this->input = opts->input;
    this->descr = 0;

    this->err_max = VERIFY_SIZE_NOT_NULL(opts->sizes_error_buf_max);
    this->err_init = VERIFY_SIZE_NOT_NULL(opts->sizes_error_buf_init);
    this->buf_size = VERIFY_SIZE_NOT_NULL(opts->sizes_input_buf_size);
    this->err_ctxt = VERIFY_SIZE_NOT_NULL(opts->sizes_error_ctxt_size);

    this->ascii_only = opts->ascii_only;
    this->no_error = opts->no_error;
    this->verbose = opts->verbose;

#ifdef DEBUG
    this->debug = BITS_TRUNC_BITS(opts->debug_base, debug_bits);
#ifdef JSON_DEBUG
    json_debug_set_level(json_debug_obj_class, opts->debug_obj);
    json_debug_set_level(json_debug_ast_class, opts->debug_ast);
    json_debug_set_level(json_debug_type_class, opts->debug_type);
    json_debug_set_level(json_debug_type_lib_class, opts->debug_type_lib);
    json_debug_set_level(json_debug_type_ruler_class, opts->debug_type_ruler);
#endif
#endif

    this->fixed_size_buf = true;
    this->echo_input =
        opts->object == OBJ_JSON_PARSER &&
        opts->parse_type == options_echo_parse_type;

    if (this->input) {
        this->descr = open(this->input, O_RDONLY);
        if (this->descr <= 0)
            sys_error("failed opening file '%s'", this->input);
    }

    this->json = NULL;
    this->buf = NULL;
}

static void obj_json_base_init_json(
    struct obj_json_base_t* this, const struct options_t* opts)
{
    json_intf_config_set_param(
        this->json, json_allow_literal_value_config, opts->liter_value);
    json_intf_config_set_param(
        this->json, json_disallow_non_ascii_config, opts->ascii_only);
    json_intf_config_set_param(
        this->json, json_allow_multi_objects_config, opts->multi_obj);
    json_intf_config_set_param(
        this->json, json_validate_utf8_config, opts->valid_utf8);
}

#define OBJ_JSON_OBJ_SIZES(n)                          \
    {                                                  \
        .buf_max = opts->sizes_ ## n ## _buf_max,      \
        .buf_init = opts->sizes_ ## n ## _buf_init,    \
        .stack_max = opts->sizes_ ## n ## _stack_max,  \
        .stack_init = opts->sizes_ ## n ## _stack_init \
    }
#define OBJ_JSON_AST_SIZES(n)                        \
    {                                                \
        .obj = OBJ_JSON_OBJ_SIZES(n),                \
        .pool_size = opts->sizes_ ## n ## _pool_size \
    }
#define OBJ_JSON_TYPE_LIB_SIZES(n)                              \
    {                                                           \
        .ast = OBJ_JSON_AST_SIZES(n),                           \
        .pool_size = opts->sizes_ ## n ## _own_pool_size,       \
        .ptr_space_size = opts->sizes_ ## n ## _ptr_space_size, \
        .text_max_size = opts->sizes_ ## n ## _text_max_size    \
    }
#define OBJ_JSON_TYPE_SIZES(n)                    \
    {                                             \
        .obj = OBJ_JSON_OBJ_SIZES(n ## _obj),     \
        .lib = OBJ_JSON_TYPE_LIB_SIZES(n ## _lib) \
    }

#define OBJ_JSON_BASE_INIT_OBJ(n, ...)                    \
    do {                                                  \
        this->json = json_intf_create_ ## n(__VA_ARGS__); \
    } while (0)

void obj_json_base_init_obj(
    struct obj_json_base_t* this, const struct options_t* opts,
    const struct json_handler_t* handler, void* obj)
{
    struct json_obj_sizes_t s = OBJ_JSON_OBJ_SIZES(obj);

    obj_json_base_init_base(this, opts);

    OBJ_JSON_BASE_INIT_OBJ(obj, handler, obj, &s);

    obj_json_base_init_json(this, opts);
}

void obj_json_base_init_ast(
    struct obj_json_base_t* this, const struct options_t* opts)
{
    struct json_ast_sizes_t s = OBJ_JSON_AST_SIZES(ast);

    obj_json_base_init_base(this, opts);

    OBJ_JSON_BASE_INIT_OBJ(ast, &s);

    obj_json_base_init_json(this, opts);
}

void obj_json_base_init_type(
    struct obj_json_base_t* this, const struct options_t* opts,
    const struct json_handler_t* handler, void* obj)
{
    struct json_type_sizes_t s = OBJ_JSON_TYPE_SIZES(type);

    obj_json_base_init_base(this, opts);

    switch (opts->type_check) {

    case options_type_check_def_type:
        OBJ_JSON_BASE_INIT_OBJ(
            type_from_def,
            (const uchar_t*) opts->type_def, opts->type_name,
            handler, obj, &s);
        break;

    case options_type_check_lib_type:
        OBJ_JSON_BASE_INIT_OBJ(
            type_from_lib, opts->type_lib, opts->type_name,
            handler, obj, &s);
        break;

    default:
        UNEXPECT_ERR(
            "invalid type-check: %d",
            opts->type_check);
    }

    obj_json_base_init_json(this, opts);
}

void obj_json_base_init_text_type_lib(
    struct obj_json_base_t* this, const struct options_t* opts)
{
    struct json_type_lib_sizes_t s = OBJ_JSON_TYPE_LIB_SIZES(lib);

    obj_json_base_init_base(this, opts);

    OBJ_JSON_BASE_INIT_OBJ(text_type_lib, &s);

    obj_json_base_init_json(this, opts);

    this->fixed_size_buf = false;
}

void obj_json_base_init_sobj_type_lib(
    struct obj_json_base_t* this, const struct options_t* opts)
{
    struct json_type_lib_sizes_t s = OBJ_JSON_TYPE_LIB_SIZES(lib);

    obj_json_base_init_base(this, opts);

    OBJ_JSON_BASE_INIT_OBJ(sobj_type_lib, opts->input, &s);

    obj_json_base_init_json(this, opts);
}

void obj_json_base_done(struct obj_json_base_t* this)
{
    if (this->buf != NULL)
        mem_buf_destroy(this->buf);
    json_intf_destroy(this->json);
    close(this->descr);
}

static void pos_error_verbose(
    struct obj_json_base_t* this, const char* file,
    size_t line, size_t col, size_t offset,
    const uchar_t* buf, size_t len,
    bool verbose, bool at_eof)
{
    const uchar_t *p, *b, *e;
    size_t n;

    if (this->no_error)
        return;

    pos_error_header(file, line, col);
    json_intf_print_error_desc(this->json, stderr);
    fputc('\n', stderr);

#ifdef DEBUG
    PRINT_DEBUG_BEGIN(
        "err_ctxt=%zu line=%zu col=%zu offset=%zu "
        "verbose=%d at_eof=%d len=%zu buf=[%p]",
        this->err_ctxt, line, col, offset,
        verbose, at_eof, len, buf);
    pretty_print_string(
        stderr, buf, len,
        pretty_print_string_quotes);
    PRINT_DEBUG_END();
#endif

    if (!verbose || line == 0 || col == 0)
        return;

    ASSERT(this->err_ctxt > 0);
    ASSERT(offset <= len);

    if (len > 0  &&
        buf != NULL &&
        buf[len - 1] == '\n' &&
        at_eof) {
        if (offset == len)
            offset --;
        len --;
    }

    b = offset > this->err_ctxt
        ? buf + (offset - this->err_ctxt)
        : buf;

    ASSERT_SIZE_INC_NO_OVERFLOW(
        this->err_ctxt);
    ASSERT_SIZE_ADD_NO_OVERFLOW(
        offset, this->err_ctxt + 1);
    e = offset + this->err_ctxt + 1 < len
        ? buf + (offset + this->err_ctxt + 1)
        : buf + len;

    // b - buf <= offset:
    // b - buf == 0 <= offset or
    // b - buf == offset - err_ctxt <= offset
    n = offset - PTR_DIFF(b, buf);
    if (len > 0) {
        ASSERT_SIZE_INC_NO_OVERFLOW(n);
        n ++;
    }

    pos_error_header(file, line, col);
    for (p = b; p < e; p ++) {
        const char* f =
            pretty_print_fmt(*p, 0);
        size_t l =
            pretty_print_len(*p, 0);

        fprintf(stderr, f, *p);
        if (p < buf + offset) {
            ASSERT_SIZE_DEC_NO_OVERFLOW(l);
            ASSERT_SIZE_ADD_NO_OVERFLOW(n, l - 1);
            n += l - 1;
        }
    }
    fputc('\n', stderr);

    pos_error_header(file, line, col);
    fprintf(stderr, "%*c\n", SIZE_AS_INT(n), '^');
}

#ifdef DEBUG

#undef  CASE
#define CASE(n) [mem_buf_ ## n] = #n
static const char* const mem_buf_ops[] = {
    CASE(append),
    CASE(shiftin),
    CASE(reset)
};

#define BUF_PRINT_DEBUG(m)                              \
    do {                                                \
        if (this->buf != NULL) {                        \
            PRINT_DEBUG_BEGIN(                          \
                "%c #buf=%zu buf=", m, this->buf->len); \
            pretty_print_string(                        \
                stderr, this->buf->ptr, this->buf->len, \
                pretty_print_string_quotes);            \
            PRINT_DEBUG_END();                          \
        }                                               \
    } while (0)
#else
#define BUF_PRINT_DEBUG(m) \
    do {} while (0)
#endif

static size_t mem_get_offset(
    const uchar_t* buf, size_t len,
    const struct json_text_pos_t* pos)
{
    size_t r = 0;

    ASSERT(pos->line > 0);
    ASSERT(pos->col > 0);

    if (pos->line > 1) {
        if (!mem_find_nth(
                buf, len, '\n',
                pos->line - 1, &r))
            ASSERT(false);
        ASSERT(r < len);
        r ++;
    }
    ASSERT_SIZE_ADD_NO_OVERFLOW(r, pos->col - 1);
    r += pos->col - 1;

    return r;
}

struct text_address_t
{
    size_t beg;
    size_t line;
    size_t col;
};

static void text_address_init(
    struct text_address_t* addr)
{
    addr->beg = 0;
    addr->line = 1;
    addr->col = 1;
}

static void text_address_update(
    struct text_address_t* addr,
    const uchar_t* ptr,
    size_t len)
{
    size_t n;

    ASSERT_SIZE_ADD_NO_OVERFLOW(addr->beg, len);
    addr->beg += len;

    if ((n = mem_count(ptr, len, '\n'))) {
        size_t p = 0;

        if (!mem_rfind(ptr, len, '\n', &p))
            ASSERT(false);

        ASSERT_SIZE_SUB_NO_OVERFLOW(len, p);
        addr->col = len - p;

        ASSERT_SIZE_ADD_NO_OVERFLOW(addr->line, n);
        addr->line += n;
    }
    else {
        ASSERT_SIZE_ADD_NO_OVERFLOW(addr->col, len);
        addr->col += len;
    }
}

static void text_address_buf_update(
    struct text_address_t* addr,
    const struct mem_buf_t* buf,
    const uchar_t* ptr,
    size_t len)
{
    size_t l = len, s = buf->size;

    if (l > s)
        l = s;
    s -= l;

    if (buf->len > s)
        text_address_update(
            addr, buf->ptr, buf->len - s);

    if (len > l)
        text_address_update(
            addr, ptr, len - l);
}

static void text_address_buf_fit(
    struct text_address_t* addr,
    const struct mem_buf_t* buf,
    size_t len)
{
    ASSERT(buf->len >= len);
    ASSERT(buf->len - len <= buf->size);

    text_address_update(
        addr, buf->ptr, buf->len - len);
}

static bool text_address_get_offset(
    const struct text_address_t* addr,
    const struct json_text_pos_t* pos,
    const uchar_t* ptr, size_t len,
    size_t* res)
{
    size_t c, d, p = 0, q;

    ASSERT(addr->line > 0);
    ASSERT(addr->col > 0);
    ASSERT(pos->line > 0);
    ASSERT(pos->col > 0);

    if (pos->line < addr->line
        || (pos->line == addr->line
            && pos->col < addr->col))
        return false;

    if ((d = pos->line - addr->line)) {
        if (!mem_find_nth(ptr, len, '\n', d, &p))
            ASSERT(false);

        ASSERT_SIZE_INC_NO_OVERFLOW(p);
        p ++;

        c = pos->col - 1;
    }
    else
        c = pos->col - addr->col;

    ASSERT(p <= len);
    if (!mem_find(ptr + p, len - p, '\n', &q))
        q = len;

    if (c > q)
        return false;

    ASSERT_SIZE_ADD_NO_OVERFLOW(p, c);
    p += c;

    if (p > len)
        return false;

    *res = p;
    return true;
}

int obj_json_base_run(struct obj_json_base_t* this)
{
    uchar_t b[this->buf_size];
    struct buf_info_t
    {
        struct text_address_t buf;
        struct text_address_t input;
        size_t                error;
        size_t                size;
    } a;
    enum json_parse_status_t s;
    struct json_error_pos_t w;
    enum error_state_t {
        before_error,
        after_error,
        eof_error,
    } t = before_error;
#ifdef DEBUG
#undef  CASE
#define CASE(n) [n] = #n
    static const char* const error_states[] = {
        CASE(before_error),
        CASE(after_error),
        CASE(eof_error)
    };
#endif
    bool e, p = false;
    bool c = false;
#ifdef DEBUG
    size_t n = 0;
#endif

    ASSERT_SIZE_DEC_NO_OVERFLOW(this->err_max);
    ASSERT_SIZE_MUL_NO_OVERFLOW(this->err_ctxt, SZ(2));
    ASSERT(2 * this->err_ctxt <= this->err_max - 1);

    s = json_parse_status_ok;
    e = json_intf_get_is_error(this->json);
    w = json_intf_get_error_pos(this->json);

    if (e) {
        const struct json_file_info_t* f =
            json_intf_get_error_file(this->json);
        size_t k = f && f->buf
            ? mem_get_offset(f->buf, f->size, &w)
            : 0;

#ifdef DEBUG
        if (f != NULL)
            PRINT_DEBUG("f={.name=\"%s\" .buf=%p .size=%zu}",
                f->name, f->buf, f->size);
        else
            PRINT_DEBUG("f=(nil)");
#endif

        pos_error_verbose(
            this,
            f ? f->name : this->input,
            w.line, w.col, k,
            f ? f->buf : NULL,
            f ? f->size : 0,
            f && f->buf &&
            this->verbose,
            true);

        if (!this->echo_input)
            return e;

        s = json_parse_status_error;
        p = true;
    }

    if (this->verbose &&
        !this->no_error &&
        this->buf == NULL) {
        size_t m;
        size_t i;

        if (this->fixed_size_buf) {
            // stev: when 'fixed_size_buf' is true,
            // 'buf' will contain a preset amount
            // of the input given such that to be
            // able to output an error context of
            // requested size; in this case, 'buf'
            // is fixed-size preallocated

            m = this->err_ctxt;
            ASSERT_SIZE_MUL_NO_OVERFLOW(m, SZ(2));
            m *= 2;
            ASSERT_SIZE_INC_NO_OVERFLOW(m);
            m ++;

            i = m;
        }
        else {
            // stev: when 'fixed_size_buf' is false,
            // 'buf' will contain the input given
            // in its entirety; in this case, 'buf'
            // grows dynamically from an initially
            // allocated space of 'err_init' size,
            // up to at most 'err_max' size

            m = this->err_max;
            i = this->err_init;
        }

        this->buf = mem_buf_create(m, i);
        mem_buf_enlarge(this->buf, i);
    }

    text_address_init(&a.buf);
    text_address_init(&a.input);
    a.error = 0;

    a.size = this->err_ctxt;
    ASSERT_SIZE_INC_NO_OVERFLOW(a.size);
    a.size ++;

    for (;;) {
        ssize_t r;

        r = read(this->descr, b, this->buf_size);
        if (r < 0)
            sys_error("reading failed");
        if (r == 0) {
            if (t == after_error)
                t = eof_error;
            break;
        }
        if (this->echo_input)
            fwrite(b, r, 1, stdout);
#ifdef DEBUG
        BUF_PRINT_DEBUG('$');
        PRINT_DEBUG_BEGIN("$ r=%zd b[%p]=", r, b);
        pretty_print_string(
            stderr, b, r, pretty_print_string_quotes);
        PRINT_DEBUG_END();
#endif
        if (s == json_parse_status_ok) {
            s = json_intf_parse(this->json, b, r);
            e = json_intf_get_is_error(this->json);
            w = json_intf_get_error_pos(this->json);

#ifdef DEBUG
            n ++;
            PRINT_DEBUG_BEGIN(
                "%c n=%zu s=%s e=",
                n == 1 ? '>' : '-', n,
                json_parse_status_get_name(s));
            json_intf_print_error_debug(
                this->json, stderr);
            PRINT_DEBUG_END();
#endif

            if (s != json_parse_status_ok &&
                !this->echo_input &&
                !this->verbose)
                break;
        }

        if (this->verbose &&
            !this->no_error) {
            enum mem_buf_op_t o;
            size_t l = INT_AS_SIZE(r);
            uchar_t* p = b;

            BUF_PRINT_DEBUG('|');
            PRINT_DEBUG(
                "| w={%zu,%zu} "
                "a.buf={%zu,%zu,%zu} "
                "a.input={%zu,%zu,%zu} "
                "a.error=%zu a.size=%zu",
                w.line, w.col, a.buf.beg,
                a.buf.line, a.buf.col, a.input.beg,
                a.input.line, a.input.col,
                a.error, a.size);

            if (this->fixed_size_buf) {
                if (t == before_error &&
                    s != json_parse_status_ok) {
                    struct text_address_t* r;

                    // stev: obtain the relative offset
                    // of the input error in 'a.error'
                    if (!text_address_get_offset(
                            r = &a.input, &w, p, l, &a.error) &&
                        !text_address_get_offset(
                            r = &a.buf, &w, this->buf->ptr,
                            this->buf->len, &a.error)) {
                        r = NULL;
                        c = true;
                    }

                    PRINT_DEBUG("| l=%zu a.error=%zu c=%d r=%s",
                        l, a.error, c,
                        r == &a.input
                        ? "&a.input"
                        : r == &a.buf
                        ? "&a.buf"
                        : r == NULL
                        ? "NULL"
                        : "?");

                    // stev: shift in all input chars occuring
                    // before the erroneous one -- excluding it
                    if (a.error && r == &a.input) {
                        text_address_buf_update(
                            &a.buf, this->buf, p, a.error);
                        mem_buf_update(
                            this->buf, p, a.error,
                            mem_buf_shiftin);

                        BUF_PRINT_DEBUG('/');

                        ASSERT_SIZE_SUB_NO_OVERFLOW(l, a.error);
                        p += a.error;
                        l -= a.error;

                        a.error = this->buf->len;
                        r = &a.buf;
                    }

                    // stev: make an absolute offset of 'a.error'
                    if (r == &a.input || r == &a.buf) {
                        size_t n =
                            r == &a.input ? a.input.beg : a.buf.beg;
                        ASSERT_SIZE_ADD_NO_OVERFLOW(a.error, n);
                        a.error += n;
                    }

                    // stev: fit 'buf' to at most 'err_ctxt' chars
                    if (r != NULL && this->buf->len > this->err_ctxt) {
                        text_address_buf_fit(
                            &a.buf, this->buf, this->err_ctxt);
                        mem_buf_shift_fit(
                            this->buf, this->err_ctxt);
                    }

                    t = after_error;
                }

                if (s != json_parse_status_ok) {
                    size_t n = SIZE_SUB(
                        this->buf->size,
                        this->buf->len);

                    if (l > n)
                        l = n;
                    if (l > a.size)
                        l = a.size;

                    a.size -= l;
                }
            }

            o = this->fixed_size_buf &&
                s == json_parse_status_ok
                ? mem_buf_shiftin
                : mem_buf_append;

            PRINT_DEBUG("| l=%zu a.error=%zu a.size=%zu o=%s",
                l, a.error, a.size, ARRAY_NULL_ELEM(
                    mem_buf_ops, o));

            if (this->fixed_size_buf && !c) {
                if (o == mem_buf_shiftin)
                    text_address_buf_update(
                        &a.buf, this->buf, p, l);
                text_address_update(
                    &a.input, b, INT_AS_SIZE(r));
            }
            mem_buf_update(this->buf, p, l, o);

            if (s != json_parse_status_ok &&
                !this->echo_input &&
                !a.size)
                break;
        }
    }

    if (p)
        return true;

    BUF_PRINT_DEBUG('<');
    PRINT_DEBUG(
        "< a.buf={%zu,%zu,%zu} "
        "a.input={%zu,%zu,%zu} "
        "a.error=%zu a.size=%zu t=%s",
        a.buf.beg, a.buf.line, a.buf.col,
        a.input.beg, a.input.line, a.input.col,
        a.error, a.size, ARRAY_NULL_ELEM(
            error_states, t));

    if (this->buf != NULL)
        ASSERT_SIZE_ADD_NO_OVERFLOW(
            this->buf->len,
            a.buf.beg);
    if (t == after_error &&
        this->buf != NULL &&
        this->buf->len + a.buf.beg ==
        a.input.beg && !c) {
        ssize_t r;
        char b;

        r = read(this->descr, &b, 1);
        if (r < 0)
            sys_error("reading failed");
        if (r == 0)
            t = eof_error;
    }

    s = json_intf_parse_done(this->json);
    e = json_intf_get_is_error(this->json);
    w = json_intf_get_error_pos(this->json);

    if (this->buf != NULL &&
        e && t == before_error) {
        ASSERT(!c);
        if (this->buf->len > 0)
            c = !text_address_get_offset(
                    &a.buf, &w, this->buf->ptr,
                    this->buf->len, &a.error);
        t = eof_error;
    }
    else
    if (e && t != before_error && !c) {
        // stev: make 'a.error' a relative offset
        if (a.error >= a.buf.beg) {
            a.error -= a.buf.beg;
            c = this->buf != NULL &&
                this->buf->len < a.error;
        }
        else
            c = true;
    }

#ifdef DEBUG
    PRINT_DEBUG_BEGIN(
        "< n=%zu c=%d t=%s a.error=%zu s=%s e=",
        n, c, ARRAY_NULL_ELEM(error_states, t),
        a.error, json_parse_status_get_name(s));
    json_intf_print_error_debug(
        this->json, stderr);
    PRINT_DEBUG_END();
#endif

    if (s == json_parse_status_ok)
        ASSERT(!e);
    else
    if (this->buf != NULL && !c)
        pos_error_verbose(
            this,
            this->input,
            w.line, w.col, a.error,
            this->buf->ptr,
            this->buf->len,
            this->verbose,
            t == eof_error);
    else
        pos_error_verbose(
            this,
            this->input,
            w.line, w.col,
            0, NULL, 0,
            false, false);

    if (c)
        pos_error(this->input, 0, 0,
            "'error-context-size' is too small for "
            "printing out input error context");

    if (this->buf != NULL &&
        this->fixed_size_buf) {
        mem_buf_destroy(this->buf);
        this->buf = NULL;
    }

    return e;
}

bool obj_json_base_check_error(struct obj_json_base_t* this)
{
    struct json_error_pos_t w;

    if (!json_intf_get_is_error(this->json))
        return false;

    w = json_intf_get_error_pos(this->json);

    if (this->buf != NULL &&
        !this->fixed_size_buf &&
        w.line > 0 &&
        w.col > 0) {
        size_t k;

        k = mem_get_offset(
            this->buf->ptr, this->buf->len, &w);

        PRINT_DEBUG(
            "w={%zu,%zu} buf.len=%zu k=%zu",
            w.line, w.col, this->buf->len, k);

        pos_error_verbose(
            this,
            this->input,
            w.line, w.col, k,
            this->buf->ptr,
            this->buf->len,
            this->verbose,
            true);
    }
    else
        pos_error_verbose(
            this,
            this->input,
            w.line, w.col,
            0, NULL, 0,
            false, false);

    return true;
}

struct json_obj_t* obj_json_base_get_obj(
    struct obj_json_base_t* this)
{
    return json_intf_get_as_obj(this->json);
}

struct json_ast_t* obj_json_base_get_ast(
    struct obj_json_base_t* this)
{
    return json_intf_get_as_ast(this->json);
}

struct json_type_t* obj_json_base_get_type(
    struct obj_json_base_t* this)
{
    return json_intf_get_as_type(this->json);
}

struct json_type_lib_t* obj_json_base_get_type_lib(
    struct obj_json_base_t* this)
{
    return json_intf_get_as_type_lib(this->json);
}


