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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#include "json.h"
#include "common.h"
#include "mem-buf.h"
#include "pretty-print.h"
#include "int-traits.h"
#include "ptr-traits.h"
#include "error.h"

#ifdef JSON_DEBUG
#define PRINT_DEBUG_COND obj->debug_lexer
#include "debug.h"
#endif

size_t json_version()
{
    return JSON_VERSION;
}

const char* json_build_datetime()
{
    return __DATE__ " " __TIME__;
}

static bool json_encode_utf8(uchar_t* buf, size_t* len)
{
    typedef unsigned int uint_t;
    uchar_t* p = buf;
    uint_t c = 0;

    STATIC(TYPE_WIDTH(uchar_t) >= 8);
    STATIC(TYPE_WIDTH(uint_t) >= 16);

    for (; p < buf + 4; p ++) {
        ASSERT((*p & 0xf0u) == 0);
        c = (c << 4) | *p;
    }

    // stev: see the man page UTF-8(7)

    // stev: see Table 3-6, UTF-8 Bit Distribution
    // The Unicode Standard Version 8.0 - Core Specification, Chapter 3, p. 125
    // http://www.unicode.org/versions/Unicode8.0.0/ch03.pdf

    if ((c >= 0xd800u && c <= 0xdfffu)
        || c == 0xfffeu || c == 0xffffu
        || c >= 0x200000u)
        return false;

    p = buf;
    if (c < 0x80u)
        *p ++ = (uchar_t) c;  
    else
    if (c < 0x800u) {
        *p ++ = (uchar_t) ((c >> 6) | 0xc0u);
        *p ++ = (uchar_t) ((c & 0x3f) | 0x80u);
    }
    else
    if (c < 0x10000u) {
        *p ++ = (uchar_t) ((c >> 12) | 0xe0u);
        *p ++ = (uchar_t) (((c >> 6) & 0x3fu) | 0x80u);
        *p ++ = (uchar_t) ((c & 0x3fu) | 0x80u);
    }
    else {
        *p ++ = (uchar_t) ((c >> 18) | 0xf0u);
        *p ++ = (uchar_t) (((c >> 12) & 0x3fu) | 0x80u);
        *p ++ = (uchar_t) (((c >> 6) & 0x3fu) | 0x80u);
        *p ++ = (uchar_t) ((c & 0x3fu) | 0x80u);
    }
    *len = PTR_DIFF(p, buf);

    return true;
}

static bool json_validate_utf8(const uchar_t* buf, size_t len, size_t* err)
{
    const uchar_t *ptr, *end;

    // stev: see Table 3-7, Well-Formed UTF-8 Byte Sequences
    // The Unicode Standard Version 8.0 - Core Specification, Chapter 3, p. 125
    // http://www.unicode.org/versions/Unicode8.0.0/ch03.pdf

#define INC(p) ({ if (++ (p) >= end) { *err = len; return false; } (p); })
#define ERR()  (*err = PTR_DIFF(ptr, buf), false)

    for (ptr = buf, end = buf + len; ptr < end; ptr ++) {
        uchar_t ch = *ptr;

        if (ch < 0x80)
            continue;
        else
        if (ch < 0xc2)
            return ERR();
        else
        if (ch < 0xe0) {
            if ((*INC(ptr) ^ 0x80) >= 0x40)
                return ERR();
        }
        else
        if (ch < 0xf0) {
            if (((*INC(ptr) ^ 0x80) >= 0x40) ||
                (ch < 0xe1 && *ptr < 0xa0) ||
                ((*INC(ptr) ^ 0x80) >= 0x40))
                return ERR();
        }
        else
        if (ch < 0xf5) {
            if (((*INC(ptr) ^ 0x80) >= 0x40) ||
                (ch < 0xf1 && *ptr < 0x90) ||
                (ch > 0xf3 && *ptr >= 0x90) ||
                ((*INC(ptr) ^ 0x80) >= 0x40) ||
                ((*INC(ptr) ^ 0x80) >= 0x40))
                return ERR();
        }
        else
            return ERR();
    }

#undef ERR
#undef INC

    return true;
}

enum json_parser_state_t
{
    json_parser_state_start,
    json_parser_state_value,
    json_parser_state_object,
    json_parser_state_object_arg,
    json_parser_state_object_key,
    json_parser_state_object_value,
    json_parser_state_array,
    json_parser_state_array_value,
    json_parser_state_done
};

#ifdef JSON_DEBUG

#define CASE_STATE(n) [json_parser_state_ ## n] = #n

static const char* json_parser_state_get_name(
    enum json_parser_state_t state)
{
    static const char* const names[] = {
        CASE_STATE(start),
        CASE_STATE(value),
        CASE_STATE(object),
        CASE_STATE(object_arg),
        CASE_STATE(object_key),
        CASE_STATE(object_value),
        CASE_STATE(array),
        CASE_STATE(array_value),
        CASE_STATE(done)
    };
    return ARRAY_NON_NULL_ELEM(names, state);
}

static void json_parser_state_print_debug(
    enum json_parser_state_t state)
{
    print_debug_str(json_parser_state_get_name(state));
}

#endif

#define STACK_NAME             json_parser
#define STACK_ELEM_TYPE   enum json_parser_state_t
#define STACK_ELEM_PRINT_DEBUG json_parser_state_print_debug
#ifdef JSON_DEBUG
#define STACK_NEED_PRINT_DEBUG
#endif
#define STACK_NEED_MAX_SIZE
#include "stack-impl.h"

#define JSON_PARSER_STACK_IS_INIT() \
    STACK_IS_INIT(&obj->parser_stack)
#define JSON_PARSER_STACK_PUSH(state) \
    STACK_PUSH(&obj->parser_stack, state)
#define JSON_PARSER_STACK_POP() \
    STACK_POP(&obj->parser_stack)
#define JSON_PARSER_STACK_TOP() \
    STACK_TOP(&obj->parser_stack)
#define JSON_PARSER_STACK_TOP_N(n) \
    STACK_TOP_N(&obj->parser_stack, n)
#define JSON_PARSER_STACK_SHIFT_TOP(from, to) \
    STACK_SHIFT_TOP(&obj->parser_stack, from, to)

enum json_token_type_t
{
    json_bof_token,
    json_eof_token,
    json_wsp_token,
    json_null_token,
    json_boolean_token,
    json_number_token,
    json_string_token,
    json_left_cbrace_token,  // = '{'
    json_right_cbrace_token, // = '}'
    json_left_sqbrk_token,   // = '['
    json_right_sqbrk_token,  // = ']'
    json_comma_token,        // = ','
    json_colon_token,        // = ':'
};

#ifdef JSON_DEBUG

#define CASE_TOKEN2(n, v) [json_ ## n ## _token] = #v
#define CASE_TOKEN(n)     CASE_TOKEN2(n, n)

static bool json_token_type_print_debug(enum json_token_type_t type)
{
    static const char* const names[] = {
        CASE_TOKEN(bof),
        CASE_TOKEN(eof),
        CASE_TOKEN(wsp),
        CASE_TOKEN(null),
        CASE_TOKEN(boolean),
        CASE_TOKEN(number),
        CASE_TOKEN(string),
        CASE_TOKEN2(left_cbrace, '{'),
        CASE_TOKEN2(right_cbrace, '}'),
        CASE_TOKEN2(left_sqbrk, '['),
        CASE_TOKEN2(right_sqbrk, ']'),
        CASE_TOKEN2(comma, ','),
        CASE_TOKEN2(colon, ':')
    };
    const char* e;

    if ((e = ARRAY_CHAR_NULL_ELEM(names, type))) {
        print_debug_str(e);
        return true;
    }
    else {
        print_debug_fmt("%d", type);
        return false;
    }
}

#endif

#define json_mem_buf_t mem_buf_t

struct json_mem_buf_token_val_t
{
    struct json_mem_buf_t* buf;
};

struct json_token_val_t
{
    union {
        bool                            boolean;
        struct json_mem_buf_token_val_t number;
        struct json_mem_buf_token_val_t string;
    };
};

#define json_token_pos_t json_error_pos_t

static inline struct json_token_pos_t json_token_pos_offset(
    struct json_token_pos_t pos, size_t col)
{
    ASSERT_SIZE_ADD_NO_OVERFLOW(pos.col, col);
    pos.col += col;
    return pos;
}

enum json_token_part_type_t {
    json_token_complete_part,
    json_token_literal_part,
    json_token_number_part,
    json_token_string_part
};

struct json_token_literal_part_t
{
    const char* end;
    const char* ptr;
};

enum json_token_number_state_t
{
    json_token_number_start_state,
    json_token_number_integer_state,
    json_token_number_integer2_state,
    json_token_number_fractional_state,
    json_token_number_fractional2_state,
    json_token_number_exponent_state,
    json_token_number_exponent2_state,
    json_token_number_exponent3_state,
    json_token_number_digitsn_state,
    json_token_number_digits2_state,
    json_token_number_digits2n_state,
    json_token_number_digits3_state,
    json_token_number_digits3n_state,
    json_token_number_error_state,
};

#ifdef JSON_DEBUG

#undef  CASE_STATE
#define CASE_STATE(n) [json_token_number_ ## n ## _state] = #n

static const char* json_token_number_state_get_name(
    enum json_token_number_state_t state)
{
    static const char* const names[] = {
        CASE_STATE(start),
        CASE_STATE(integer),
        CASE_STATE(integer2),
        CASE_STATE(fractional),
        CASE_STATE(fractional2),
        CASE_STATE(exponent),
        CASE_STATE(exponent2),
        CASE_STATE(exponent3),
        CASE_STATE(digitsn),
        CASE_STATE(digits2),
        CASE_STATE(digits2n),
        CASE_STATE(digits3),
        CASE_STATE(digits3n),
        CASE_STATE(error)
    };
    return ARRAY_NON_NULL_ELEM(names, state);
}

#endif

struct json_token_number_part_t
{
    enum json_token_number_state_t state;
};

enum json_token_string_state_t
{
    json_token_string_start_state,
    json_token_string_plain_char_state,
    json_token_string_esc_char_state,
    json_token_string_hexa1_state,
    json_token_string_hexa2_state,
    json_token_string_hexa3_state,
    json_token_string_hexa4_state,
    json_token_string_error_state,
};

#ifdef JSON_DEBUG

#undef  CASE_STATE
#define CASE_STATE(n) [json_token_string_ ## n ## _state] = #n

static const char* json_token_string_state_get_name(
    enum json_token_string_state_t state)
{
    static const char* const names[] = {
        CASE_STATE(start),
        CASE_STATE(plain_char),
        CASE_STATE(esc_char),
        CASE_STATE(hexa1),
        CASE_STATE(hexa2),
        CASE_STATE(hexa3),
        CASE_STATE(hexa4),
        CASE_STATE(error)
    };
    return ARRAY_NON_NULL_ELEM(names, state);
}

#endif

struct json_token_string_part_t
{
    enum json_token_string_state_t state;
    size_t pos;
};

struct json_token_part_t
{
    enum json_token_part_type_t type;
    union {
        struct json_token_literal_part_t literal;
        struct json_token_number_part_t  number;
        struct json_token_string_part_t  string;
    };
};

#define JSON_ERROR_TYPE()      (obj->error_info.type)
#define JSON_TOKEN_TYPE()      (obj->current_tok.type)
#define JSON_TOKEN_PART_TYPE() (obj->current_tok.part.type)

#define JSON_ERROR_TYPE_IS(n) \
    (                         \
        JSON_ERROR_TYPE() ==  \
        json_error_ ## n      \
    )
#define JSON_TOKEN_TYPE_IS(n) \
    (                         \
        JSON_TOKEN_TYPE() ==  \
        json_ ## n ## _token  \
    )
#define JSON_TOKEN_PART_TYPE_IS(n) \
    (                              \
        JSON_TOKEN_PART_TYPE() ==  \
        json_token_ ## n ## _part  \
    )

#define JSON_TOKEN_VALUE(n)            \
    ({                                 \
        ASSERT(JSON_TOKEN_TYPE_IS(n)); \
        &(obj->current_tok.val.n);     \
    })
#define JSON_TOKEN_PART(n)                  \
    ({                                      \
        ASSERT(JSON_TOKEN_PART_TYPE_IS(n)); \
        &(obj->current_tok.part.n);         \
    })

struct json_token_t
{
    enum json_token_type_t   type;
    struct json_token_val_t  val;
    struct json_token_pos_t  pos;
    struct json_token_part_t part;                 
};

#ifdef JSON_DEBUG

#define TOKEN_TYPE_IS(n) \
    (tok->type == json_ ## n ## _token)

#define TOKEN_VAL(n)              \
    ({                            \
        ASSERT(TOKEN_TYPE_IS(n)); \
        &(tok->val.n);            \
    })

static void json_token_boolean_val_print_debug(
    const struct json_token_t* tok)
{
    print_debug_fmt("%d", *TOKEN_VAL(boolean));
}

static void json_token_mem_buf_val_print_debug(
    const struct json_mem_buf_token_val_t* val)
{
    const struct json_mem_buf_t* buf = val->buf;

    print_debug_fmt(
        "{.buf={.ptr=%p .size=%zu .len=%zu ",
        buf->ptr, buf->size, buf->len);
    pretty_print_string(stderr, buf->ptr, buf->len,
        pretty_print_string_quotes);
    print_debug_str("}}");
}

static void json_token_number_val_print_debug(
    const struct json_token_t* tok)
{
    json_token_mem_buf_val_print_debug(TOKEN_VAL(number));
}

static void json_token_string_val_print_debug(
    const struct json_token_t* tok)
{
    json_token_mem_buf_val_print_debug(TOKEN_VAL(string));
}

#define CASE_VAL(n) [json_ ## n ## _token] = { \
    .name = #n, .func = json_token_ ## n ## _val_print_debug }

static void json_token_val_print_debug(
    const struct json_token_t* tok)
{
    struct val_info_t
    {
        const char* name;
        void (*func)(const struct json_token_t*);
    };
    static const struct val_info_t infos[] = {
        CASE_VAL(boolean),
        CASE_VAL(number),
        CASE_VAL(string),
    };
    const struct val_info_t* p;

    print_debug_str("{");

    if ((p = ARRAY_NULL_ELEM_REF(infos, tok->type)) &&
        p->name && p->func) {
        print_debug_fmt(".%s=", p->name);
        p->func(tok);
    }
    else
        print_debug_str("null");

    print_debug_str("}");
}

#define PART_TYPE_IS(n) \
    (part->type == json_token_ ## n ##_part)

#define PART_VALUE(n)            \
    ({                           \
        ASSERT(PART_TYPE_IS(n)); \
        &(part->n);              \
    })

static void json_token_literal_part_print_debug(
    const struct json_token_part_t* part)
{
    const struct json_token_literal_part_t* val =
        PART_VALUE(literal);

    print_debug_fmt(
        "{.end=%p .ptr=%p}", val->end, val->ptr);
}

static void json_token_number_part_print_debug(
    const struct json_token_part_t* part)
{
    const struct json_token_number_part_t* val =
        PART_VALUE(number);

    print_debug_fmt(
        "{.state=%s}", json_token_number_state_get_name(
            val->state));
}

static void json_token_string_part_print_debug(
    const struct json_token_part_t* part)
{
    const struct json_token_string_part_t* val =
        PART_VALUE(string);

    print_debug_fmt(
        "{.state=%s .pos=%zu}", json_token_string_state_get_name(
            val->state), val->pos);
}

#define CASE_PART2(n, f) [json_token_ ## n ## _part] = { .name = #n, .func = f }
#define CASE_PART(n)     CASE_PART2(n, json_token_ ## n ## _part_print_debug)

static void json_token_part_print_debug(const struct json_token_part_t* part)
{
    struct part_info_t
    {
        const char* name;
        void (*func)(const struct json_token_part_t*);
    };
    static const struct part_info_t infos[] = {
        CASE_PART2(complete, NULL),
        CASE_PART(literal),
        CASE_PART(number),
        CASE_PART(string)
    };
    const struct part_info_t* p;

    print_debug_str("{.type=");

    if ((p = ARRAY_NULL_ELEM_REF(infos, part->type))) {
        print_debug_fmt("%s", p->name);
        if (p->func)
            p->func(part);
    }
    else
        print_debug_fmt("%d", part->type);

    print_debug_str("}");
}

static void json_token_print_debug(const struct json_token_t* tok,
    bool print_val)
{
    print_debug_str("{.type=");

    if (json_token_type_print_debug(tok->type)) {
        if (print_val) {
            print_debug_str(" .val=");
            json_token_val_print_debug(tok);
        }
        print_debug_fmt(" .pos={.line=%zu .col=%zu} .part=",
            tok->pos.line, tok->pos.col);
        json_token_part_print_debug(&tok->part);
    }

    print_debug_str("}");
}

#endif

enum { debug_bits = 4, n_values_bits = 2 };

struct json_obj_t
{
    size_t                       config;
    const struct json_handler_t* handler;
    void*                        handler_obj;
    struct json_obj_sizes_t      sizes;
#ifdef JSON_DEBUG
    bits_t                       debug_lexer:
                                 debug_bits;
    bits_t                       debug_parser:
                                 debug_bits;
#endif
    bits_t                       n_values:
                                 n_values_bits;
    struct json_parser_stack_t   parser_stack;
    size_t                       current_conf;
    enum json_parse_status_t     current_stat;
    struct json_token_t          current_tok;
    struct json_token_pos_t      current_pos;
    struct json_mem_buf_t        current_tok_buf;
    struct json_error_info_t     error_info;
};

struct json_parse_buf_t
{
    const uchar_t* end;
    const uchar_t* ptr;
};

#ifdef JSON_DEBUG

static void json_parse_buf_print_debug(const struct json_parse_buf_t* buf)
{
    size_t n;

    print_debug_fmt("{.ptr=%p .end=%p", buf->ptr, buf->end);
    if (buf->end >= buf->ptr && (n = PTR_DIFF(buf->end, buf->ptr))) {
        print_debug_str(" ");
        pretty_print_string(
            stderr, buf->ptr, n,
            pretty_print_string_quotes);
    }
    print_debug_str("}");
}

#endif

static inline void json_mem_buf_init(struct json_mem_buf_t* buf,
    size_t max, size_t init)
{ mem_buf_init(buf, max, init); }

static inline void json_mem_buf_done(struct json_mem_buf_t* buf)
{ mem_buf_done(buf); }

static inline void json_mem_buf_reset(struct json_mem_buf_t* buf)
{ mem_buf_update(buf, NULL, 0, mem_buf_reset); }

static inline void json_mem_buf_assign(struct json_mem_buf_t* buf,
    const uchar_t* str, size_t len)
{ mem_buf_update(buf, str, len, mem_buf_reset); }

static inline void json_mem_buf_append(struct json_mem_buf_t* buf,
    const uchar_t* str, size_t len)
{ mem_buf_update(buf, str, len, mem_buf_append); }

#define JSON_MEM_BUF_APPEND(buf, ch) MEM_BUF_APPEND(buf, ch)

static bool json_mem_buf_encode_utf8(struct json_mem_buf_t* buf,
    const uchar_t* ptr)
{
    size_t w;

    // stev: main invariant of buf
    ASSERT(buf->size >= buf->len);

    // stev: ptr is inside buf and
    // at four bytes from the end
    ASSERT(buf->len >= 4);
    ASSERT(buf->ptr <= ptr);
    ASSERT(buf->ptr + buf->size > ptr);
    ASSERT(buf->ptr + buf->len == ptr + 4);

    if (json_encode_utf8((uchar_t*) ptr, &w)) {
        ASSERT(w <= 4U && w > 0);
        buf->len -= 4U - w;
        return true;
    }
    return false;
}

static inline bool json_mem_buf_validate_utf8(struct json_mem_buf_t* buf,
    size_t* err)
{ return json_validate_utf8(buf->ptr, buf->len, err); }

#define JSON_PARSER_ERROR_POS(t, w, p, r)                    \
    ({                                                       \
        obj->error_info.type = json_error_ ## t;             \
        obj->error_info.what.t = json_ ## t ## _error_ ## w; \
        obj->error_info.pos = p;                             \
        r;                                                   \
    })

#define JSON_LEX_ERROR_POS(w, p) \
    JSON_PARSER_ERROR_POS(lex, w, p, false)
#define JSON_PARSE_ERROR_POS(w, p) \
    JSON_PARSER_ERROR_POS(parse, w, p, JSON_STATUS(error))

#define JSON_LEX_ERROR(w)   JSON_LEX_ERROR_POS(w, obj->current_pos)
#define JSON_PARSE_ERROR(w) JSON_PARSE_ERROR_POS(w, obj->current_tok.pos)

static bool json_parse_literal_token(
    struct json_obj_t* obj, struct json_parse_buf_t* buf)
{
    struct json_token_literal_part_t* part;
    size_t n = 0;

    ASSERT(JSON_TOKEN_PART_TYPE_IS(literal));
    ASSERT(buf->ptr < buf->end);

    part = JSON_TOKEN_PART(literal);
    ASSERT(part->ptr < part->end);

    while (buf->ptr < buf->end &&
            part->ptr < part->end &&
           *part->ptr == *buf->ptr) {
        part->ptr ++;
        buf->ptr ++;
        n ++;
    }

    ASSERT_SIZE_ADD_NO_OVERFLOW(obj->current_pos.col, n);
    obj->current_pos.col += n;

    if (part->ptr >= part->end) {
        obj->current_tok.part.type = json_token_complete_part;
        return true;
    }
    if (buf->ptr >= buf->end)
        return true;

    return JSON_LEX_ERROR(invalid_literal);
}

static bool json_parse_literal_end_token(struct json_obj_t* obj)
{
    ASSERT(JSON_TOKEN_PART_TYPE_IS(literal));
    return JSON_LEX_ERROR(invalid_literal);
}

#ifdef JSON_DEBUG
#define PRINT_PARSE_TOKEN_BUF(n, w)                                           \
    do {                                                                      \
        PRINT_DEBUG_BEGIN("state=%-" #w "s ch=",                              \
            json_token_ ## n ## _state_get_name(part->state));                \
        s = pretty_print_char(stderr, ch, pretty_print_char_quotes);          \
        ASSERT(s >= 3 && s <= 6);                                             \
        PRINT_DEBUG_FMT("%-*s buf={.ptr=%p .end=%p ", SIZE_AS_INT(6 - s), "", \
            buf->ptr, buf->end);                                              \
        pretty_print_string(stderr, buf->ptr, PTR_DIFF(buf->end, buf->ptr),   \
            pretty_print_string_quotes);                                      \
        PRINT_DEBUG_STR("}");                                                 \
        PRINT_DEBUG_END();                                                    \
    } while (0)
#else
#define PRINT_PARSE_TOKEN_BUF(n, w) \
    do {} while (0)
#endif

enum {
    cntrl_mask = 0x1fu,
    ascii_mask = 0x7fu,
    ascii_size = ascii_mask + 1,
    del_char = ascii_mask
};

#define CH(x)       ((char)(x))
#define IS_CNTRL(c) (((c) & ~cntrl_mask) == 0 || ((c) == del_char))
#define IS_ASCII(c) (((c) & ~ascii_mask) == 0)

#define JSON_NUM_STATE(n) (json_token_number_ ## n ## _state)

static bool json_parse_number_token(
    struct json_obj_t* obj, struct json_parse_buf_t* buf)
{
    static const bool digits[ascii_size] = {
        ['0'] = true,  ['1'] = true,  ['2'] = true,  ['3'] = true,  ['4'] = true,
        ['5'] = true,  ['6'] = true,  ['7'] = true,  ['8'] = true,  ['9'] = true,
    };

#define IS_DIGIT(c) (IS_ASCII(c) && digits[c])

    struct json_token_number_part_t* part;
    struct json_mem_buf_token_val_t* val;
    struct json_mem_buf_t* muf;
    bool done = false;
    size_t n = 0;
#ifdef JSON_DEBUG
    size_t s;
#endif

    ASSERT(JSON_TOKEN_TYPE_IS(number));
    ASSERT(JSON_TOKEN_PART_TYPE_IS(number));
    ASSERT(buf->ptr < buf->end);

    val = JSON_TOKEN_VALUE(number);
    part = JSON_TOKEN_PART(number);
    muf = val->buf;

    ASSERT(muf->size >= muf->len);

    while (buf->ptr < buf->end && !done) {
        uchar_t ch = *buf->ptr;

    next_state:
        PRINT_PARSE_TOKEN_BUF(number, 11);

        switch (part->state) {

        // [ "-" ] ( "0" | [1-9] [0-9]* ) [ "." [0-9]+ ] [ [eE] [ [-+] ] [0-9]+ ]

        case JSON_NUM_STATE(start):
            ASSERT(muf->len == 0);
            ASSERT(ch == '-' || IS_DIGIT(ch));

            part->state = JSON_NUM_STATE(integer);
            goto next_state;

        case JSON_NUM_STATE(integer):
            part->state = JSON_NUM_STATE(integer2);
            if (ch != '-')
                goto next_state;
            break;

        case JSON_NUM_STATE(integer2):
            if (!IS_DIGIT(ch)) {
                part->state = JSON_NUM_STATE(error);
                goto loop_out;
            }
            part->state = ch == '0'
                ? JSON_NUM_STATE(fractional)
                : JSON_NUM_STATE(digitsn);
            break;

        case JSON_NUM_STATE(digits2):
        case JSON_NUM_STATE(digits3):
            if (!IS_DIGIT(ch)) {
                part->state = JSON_NUM_STATE(error);
                goto loop_out;
            }
            STATIC(
                JSON_NUM_STATE(digits2) + 1 ==
                JSON_NUM_STATE(digits2n) &&
                JSON_NUM_STATE(digits3) + 1 ==
                JSON_NUM_STATE(digits3n));
            part->state ++;
            break;

        case JSON_NUM_STATE(digitsn):
            if (!IS_DIGIT(ch)) {
                part->state = JSON_NUM_STATE(fractional);
                goto next_state;
            }
            break;

        case JSON_NUM_STATE(digits2n):
            if (!IS_DIGIT(ch)) {
                part->state = JSON_NUM_STATE(exponent);
                goto next_state;
            }
            break;

        case JSON_NUM_STATE(digits3n):
            if (!IS_DIGIT(ch)) {
                done = true;
                goto loop_out;
            }
            break;

        case JSON_NUM_STATE(fractional):
            part->state = ch == '.'
                ? JSON_NUM_STATE(fractional2)
                : JSON_NUM_STATE(exponent);
            goto next_state;

        case JSON_NUM_STATE(fractional2):
            if (ch != '.') {
                part->state = JSON_NUM_STATE(error);
                goto loop_out;
            }
            part->state = JSON_NUM_STATE(digits2);
            break;

        case JSON_NUM_STATE(exponent):
            if (ch == 'e' || ch == 'E') {
                part->state = JSON_NUM_STATE(exponent2);
                goto next_state;
            }
            else {
                done = true;
                goto loop_out;
            }

        case JSON_NUM_STATE(exponent2):
            if (ch != 'e' && ch != 'E') {
                part->state = JSON_NUM_STATE(error);
                goto loop_out;
            }
            part->state = JSON_NUM_STATE(exponent3);
            break;

        case JSON_NUM_STATE(exponent3):
            part->state = JSON_NUM_STATE(digits3);
            if (ch != '-' && ch != '+')
                goto next_state;
            break;

        default:
            UNEXPECT_VAR("%d", part->state);
        }

        JSON_MEM_BUF_APPEND(muf, ch);

        buf->ptr ++;
        n ++;
    }
loop_out:

    ASSERT_SIZE_ADD_NO_OVERFLOW(obj->current_pos.col, n);
    obj->current_pos.col += n;

    if (done) {
        ASSERT(
            part->state == JSON_NUM_STATE(digits3n) ||
            part->state == JSON_NUM_STATE(exponent));

        JSON_MEM_BUF_APPEND(muf, CH(0));
        obj->current_tok.part.type = json_token_complete_part;
        return true;
    }
    if (part->state != JSON_NUM_STATE(error) &&
        buf->ptr >= buf->end)
        return true;

    return JSON_LEX_ERROR(invalid_number_literal);
}

static bool json_parse_number_end_token(struct json_obj_t* obj)
{
    struct json_token_number_part_t* part;

    ASSERT(JSON_TOKEN_TYPE_IS(number));
    ASSERT(JSON_TOKEN_PART_TYPE_IS(number));

    part = JSON_TOKEN_PART(number);

    if (part->state == JSON_NUM_STATE(digitsn) ||
        part->state == JSON_NUM_STATE(digits2n) ||
        part->state == JSON_NUM_STATE(digits3n) ||
        part->state == JSON_NUM_STATE(fractional)) {
        struct json_mem_buf_token_val_t* val;
        struct json_mem_buf_t* muf;

        val = JSON_TOKEN_VALUE(number);
        muf = val->buf;

        JSON_MEM_BUF_APPEND(muf, CH(0));

        obj->current_tok.part.type = json_token_complete_part;
        return true;
    }

    return JSON_LEX_ERROR(invalid_number_literal);
}

#define JSON_OBJECT_CONF(p) \
    ((obj->current_conf & (1U << json_ ## p ## _config)) != 0)

#define JSON_STR_STATE(n) (json_token_string_ ## n ## _state)

static bool json_parse_string_token(
    struct json_obj_t* obj, struct json_parse_buf_t* buf)
{
    enum { hex_bit = 0x10u };

    static const uchar_t unescape[ascii_size] = {
        ['"'] = '"',  ['\\'] = '\\', ['/'] = '/',
        ['b'] = '\b', ['f']  = '\f', ['n'] = '\n', ['r'] = '\r', ['t'] = '\t'
    };
#define H(c) (c | hex_bit)
    static const uchar_t hexa[ascii_size] = {
        ['0'] = H(0),  ['1'] = H(1),  ['2'] = H(2),  ['3'] = H(3),  ['4'] = H(4),
        ['5'] = H(5),  ['6'] = H(6),  ['7'] = H(7),  ['8'] = H(8),  ['9'] = H(9),
        ['a'] = H(10), ['b'] = H(11), ['c'] = H(12), ['d'] = H(13), ['e'] = H(14),
        ['f'] = H(15),
        ['A'] = H(10), ['B'] = H(11), ['C'] = H(12), ['D'] = H(13), ['E'] = H(14),
        ['F'] = H(15)
    };
#undef H

    struct json_token_string_part_t* part;
    struct json_mem_buf_token_val_t* val;
    struct json_mem_buf_t* muf;
    bool done = false;
    size_t n = 0;
#ifdef JSON_DEBUG
    size_t s;
#endif

    ASSERT(JSON_TOKEN_TYPE_IS(string));
    ASSERT(JSON_TOKEN_PART_TYPE_IS(string));
    ASSERT(buf->ptr < buf->end);

    val = JSON_TOKEN_VALUE(string);
    part = JSON_TOKEN_PART(string);
    muf = val->buf;

    ASSERT(muf->size >= muf->len);

    while (buf->ptr < buf->end && !done) {
        uchar_t ch = *buf->ptr;

        PRINT_PARSE_TOKEN_BUF(string, 10);

        switch (part->state) {
        case JSON_STR_STATE(start):
            ASSERT(muf->len == 0);
            ASSERT(ch == '"');

            part->state = JSON_STR_STATE(plain_char);
            break;

        case JSON_STR_STATE(plain_char):
            if ((!IS_ASCII(ch) && JSON_OBJECT_CONF(disallow_non_ascii)) ||
                  IS_CNTRL(ch)) {
                part->state = JSON_STR_STATE(error);
                goto loop_out;
            }
            else
            if (ch == '\\')
                part->state = JSON_STR_STATE(esc_char);
            else
            if (ch != '"')
                JSON_MEM_BUF_APPEND(muf, ch);
            else
                done = true;
            break;

        case JSON_STR_STATE(esc_char):
            if (ch == 'u')
                part->state = JSON_STR_STATE(hexa1);
            else
            if (!IS_ASCII(ch) || (ch = unescape[ch]) == 0) {
                part->state = JSON_STR_STATE(error);
                goto loop_out;
            }
            else {
                part->state = JSON_STR_STATE(plain_char);
                JSON_MEM_BUF_APPEND(muf, ch);
            }
            break;

        case JSON_STR_STATE(hexa1):
        case JSON_STR_STATE(hexa2):
        case JSON_STR_STATE(hexa3):
        case JSON_STR_STATE(hexa4):
            if (!IS_ASCII(ch) || (ch = hexa[ch]) == 0) {
                part->state = JSON_STR_STATE(error);
                goto loop_out;
            }
            if (part->state == JSON_STR_STATE(hexa1))
                part->pos = muf->len;

            ch &= ~ hex_bit;
            JSON_MEM_BUF_APPEND(muf, ch);

            STATIC(
                JSON_STR_STATE(hexa1) + 1 == JSON_STR_STATE(hexa2) &&
                JSON_STR_STATE(hexa2) + 1 == JSON_STR_STATE(hexa3) &&
                JSON_STR_STATE(hexa3) + 1 == JSON_STR_STATE(hexa4));
            if (part->state == JSON_STR_STATE(hexa4)) {
                if (!json_mem_buf_encode_utf8(
                        muf, muf->ptr + part->pos)) {
                    part->state = JSON_STR_STATE(error);
                    goto loop_out;
                }
                part->state = JSON_STR_STATE(plain_char);
            }
            else
                part->state ++;
            break;

        default:
            UNEXPECT_VAR("%d", part->state);
        }

        buf->ptr ++;
        n ++;
    }
loop_out:

    ASSERT_SIZE_ADD_NO_OVERFLOW(obj->current_pos.col, n);
    obj->current_pos.col += n;

    if (done) {
        size_t pos;

        ASSERT(part->state == JSON_STR_STATE(plain_char));

        if (JSON_OBJECT_CONF(validate_utf8) &&
            !json_mem_buf_validate_utf8(muf, &pos)) {
            struct json_token_pos_t err = json_token_pos_offset(
                obj->current_tok.pos, SIZE_PRE_INC(pos));
            return JSON_LEX_ERROR_POS(invalid_utf8_encoding, err);
        }

        JSON_MEM_BUF_APPEND(muf, CH(0));
        obj->current_tok.part.type = json_token_complete_part;
        return true;
    }
    if (part->state != JSON_STR_STATE(error) &&
        buf->ptr >= buf->end)
        return true;

    return JSON_LEX_ERROR(invalid_string_literal);
}

static bool json_parse_string_end_token(struct json_obj_t* obj)
{
    ASSERT(JSON_TOKEN_PART_TYPE_IS(string));
    return JSON_LEX_ERROR(invalid_string_literal);
}

static bool json_parse_part_token(
    struct json_obj_t* obj, struct json_parse_buf_t* buf)
{
    enum json_token_part_type_t type;

    type = JSON_TOKEN_PART_TYPE();

    switch (type) {
    case json_token_literal_part:
        return json_parse_literal_token(obj, buf);
    case json_token_number_part:
        return json_parse_number_token(obj, buf);
    case json_token_string_part:
        return json_parse_string_token(obj, buf);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

static bool json_parse_end_token(struct json_obj_t* obj)
{
    enum json_token_part_type_t type;

    type = JSON_TOKEN_PART_TYPE();

    switch (type) {
    case json_token_literal_part:
        return json_parse_literal_end_token(obj);
    case json_token_number_part:
        return json_parse_number_end_token(obj);
    case json_token_string_part:
        return json_parse_string_end_token(obj);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

#define JSON_PARSED_WSP(n)                                     \
    do {                                                       \
        obj->current_tok.type = json_wsp_token;                \
        obj->current_tok.pos = obj->current_pos;               \
        obj->current_tok.part.type = json_token_complete_part; \
        ASSERT_SIZE_ADD_NO_OVERFLOW(obj->current_pos.col, n);  \
        obj->current_pos.col += n;                             \
    } while (0)

static bool json_parse_wsp(
    struct json_obj_t* obj, struct json_parse_buf_t* buf)
{
    const uchar_t* p = buf->ptr;
    size_t n = 0;

    ASSERT(JSON_TOKEN_PART_TYPE_IS(complete));
    ASSERT(buf->ptr < buf->end);

    do {
        switch (*buf->ptr) {
        case '\n':
            ASSERT_SIZE_INC_NO_OVERFLOW(
                obj->current_pos.line);
            obj->current_pos.line ++;
            obj->current_pos.col = 1;
            n = 0;
            continue;
        case ' ':
        case '\t':
        case '\r':
            n ++;
            continue;
        default:
            goto parse_out;
        }
    } while (++ buf->ptr < buf->end);

parse_out:
    if (PTR_DIFF(buf->ptr, p))
        JSON_PARSED_WSP(n);

    return true;
}

#define JSON_PARSE_CHAR(n)                                     \
    ({                                                         \
        obj->current_tok.type = json_ ## n ## _token;          \
        obj->current_tok.pos = obj->current_pos;               \
        obj->current_tok.part.type = json_token_complete_part; \
        obj->current_pos.col ++;                               \
        buf->ptr ++;                                           \
        true;                                                  \
    })

#define JSON_PARSE_LITERAL_PART(l)                                         \
    ({                                                                     \
        obj->current_tok.pos = obj->current_pos;                           \
        obj->current_tok.part.type = json_token_literal_part;              \
        obj->current_tok.part.literal.end = lit_ ## l + strlen(lit_ ## l); \
        obj->current_tok.part.literal.ptr = lit_ ## l;                     \
        json_parse_literal_token(obj, buf);                                \
    })

#define JSON_PARSE_LITERAL(n)                         \
    ({                                                \
        obj->current_tok.type = json_ ## n ## _token; \
        JSON_PARSE_LITERAL_PART(n);                   \
    })

#define JSON_PARSE_LITERAL_VAL(n, l)                  \
    ({                                                \
        obj->current_tok.type = json_ ## n ## _token; \
        obj->current_tok.val.n = l;                   \
        JSON_PARSE_LITERAL_PART(l);                   \
    })

#define JSON_PARSE_LITERAL_BUF_PART(n)                                    \
    ({                                                                    \
        obj->current_tok.type = json_ ## n ## _token;                     \
        obj->current_tok.val.n.buf = &obj->current_tok_buf;               \
        obj->current_tok.pos = obj->current_pos;                          \
        obj->current_tok.part.type = json_token_ ## n ## _part;           \
        obj->current_tok.part.n.state = json_token_ ## n ## _start_state; \
        json_mem_buf_reset(&obj->current_tok_buf);                        \
        json_parse_ ## n ## _token(obj, buf);                             \
    })

#define JSON_PARSE_NUMBER() JSON_PARSE_LITERAL_BUF_PART(number)
#define JSON_PARSE_STRING() JSON_PARSE_LITERAL_BUF_PART(string)

static bool json_parse_begin_token(
    struct json_obj_t* obj, struct json_parse_buf_t* buf)
{
    static const char lit_null[] = "null";
    static const char lit_0[]    = "false";
    static const char lit_1[]    = "true";

    ASSERT(JSON_TOKEN_PART_TYPE_IS(complete));

    ASSERT(buf->ptr < buf->end);

    if (!json_parse_wsp(obj, buf))
        return false;

    if (buf->ptr >= buf->end)
        return true;

    switch (*buf->ptr) {
    case '{':
        return JSON_PARSE_CHAR(left_cbrace);
    case '}':
        return JSON_PARSE_CHAR(right_cbrace);
    case '[':
        return JSON_PARSE_CHAR(left_sqbrk);
    case ']':
        return JSON_PARSE_CHAR(right_sqbrk);
    case ',':
        return JSON_PARSE_CHAR(comma);
    case ':':
        return JSON_PARSE_CHAR(colon);
    case 'n':
        return JSON_PARSE_LITERAL(null);
    case 'f':
        return JSON_PARSE_LITERAL_VAL(boolean, false);
    case 't':
        return JSON_PARSE_LITERAL_VAL(boolean, true);
    case '-':
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        return JSON_PARSE_NUMBER();
    case '"':
        return JSON_PARSE_STRING();
    default:
        return JSON_LEX_ERROR(invalid_char);
    }
}

#define JSON_PARSE_INSERT_EOF()                                \
    ({                                                         \
        obj->current_tok.type = json_eof_token;                \
        obj->current_tok.pos = obj->current_pos;               \
        obj->current_tok.part.type = json_token_complete_part; \
        true;                                                  \
    })

static bool json_parse_token(
    struct json_obj_t* obj, struct json_parse_buf_t* buf, bool done)
{
    bool complete = JSON_TOKEN_PART_TYPE_IS(complete);

    if (!done && complete)
        return json_parse_begin_token(obj, buf);
    if (!done)
        return json_parse_part_token(obj, buf);
    if (!complete)
        return json_parse_end_token(obj);
    else
        return JSON_PARSE_INSERT_EOF();
}

#define JSON_TOKEN_VAL(t)              \
    ({                                 \
        ASSERT(JSON_TOKEN_TYPE_IS(t)); \
        obj->current_tok.val.t;        \
    })

#define JSON_TOKEN_BUF_PTR(t)            \
    ({                                   \
        ASSERT(JSON_TOKEN_TYPE_IS(t));   \
        obj->current_tok.val.t.buf->ptr; \
    })

// stev: we are decrementing 'buf->len' because
// it accounts for the null char 'buf->ptr' is
// containing at its the end 

#define JSON_TOKEN_BUF_LEN(t)                \
    ({                                       \
        ASSERT(JSON_TOKEN_TYPE_IS(t));       \
        size_t __l =                         \
            obj->current_tok.val.t.buf->len; \
        ASSERT_SIZE_DEC_NO_OVERFLOW(__l);    \
        __l - 1;                             \
    })

#define JSON_CALL_HANDLER_(n, ...)                                       \
    do {                                                                 \
        if (obj->handler != NULL &&                                      \
            obj->handler->n ## _func != NULL &&                          \
            !obj->handler->n ## _func(obj->handler_obj, ## __VA_ARGS__)) \
            return JSON_STATUS(canceled);                                \
    } while (0)

#define JSON_CALL_HANDLER(n) \
    JSON_CALL_HANDLER_(n)
#define JSON_CALL_HANDLER_VAL(n) \
    JSON_CALL_HANDLER_(n, JSON_TOKEN_VAL(n))
#define JSON_CALL_HANDLER_BUF_FUNC(n, t) \
    JSON_CALL_HANDLER_(n, JSON_TOKEN_BUF_PTR(t), JSON_TOKEN_BUF_LEN(t))
#define JSON_CALL_HANDLER_BUF(n) \
    JSON_CALL_HANDLER_BUF_FUNC(n, n)

#define JSON_STACK_IS_INIT() JSON_PARSER_STACK_IS_INIT()

#define JSON_TOP_STATE()     JSON_PARSER_STACK_TOP()
#define JSON_TOP_STATE_N(n)  JSON_PARSER_STACK_TOP_N(n)

#define JSON_PUSH_STATE(s)            \
    do {                              \
        JSON_PARSER_STACK_PUSH(       \
            json_parser_state_ ## s); \
    } while (0)

#define JSON_SHIFT_STATES(s0, s1)      \
    do {                               \
        JSON_PARSER_STACK_SHIFT_TOP(   \
            json_parser_state_ ## s0,  \
            json_parser_state_ ## s1); \
    } while (0)

#define JSON_POP_STATE(s)                       \
    do {                                        \
        enum json_parser_state_t __t =          \
            JSON_PARSER_STACK_POP();            \
        ASSERT(json_parser_state_ ## s == __t); \
    } while (0)

#define JSON_UNEXPECT_TOK()                \
    (                                      \
        JSON_PARSE_ERROR(unexpected_token) \
    )

#define JSON_PARSE_HALT()                         \
    do {                                          \
        UNEXPECT_ERR("state=%d token=%d",         \
            JSON_TOP_STATE(), JSON_TOKEN_TYPE()); \
    } while (0)

#define JSON_TOKEN(n) json_ ## n ## _token
#define JSON_STATE(n) json_parser_state_ ## n

#define JSON_PARSER_CONF(p)                             \
    (                                                   \
        JSON_TOP_STATE_N(SZ(1)) != JSON_STATE(start) || \
        JSON_OBJECT_CONF(p)                             \
    )

#ifdef JSON_DEBUG

#define JSON_PARSE_PRINT_DEBUG(t)                 \
    do {                                          \
        PRINT_DEBUG_BEGIN("%s > state=%s token=", \
            t, json_parser_state_get_name(        \
                JSON_TOP_STATE()));               \
        json_token_print_debug(&obj->current_tok, \
            obj->debug_lexer > 1);                \
        PRINT_DEBUG_STR(" puf=");                 \
        json_parse_buf_print_debug(&puf);         \
        PRINT_DEBUG_END();                        \
    } while (0)

#define JSON_PARSE_STACK_DEBUG()           \
    do {                                   \
        if (obj->debug_parser > 1) {       \
            PRINT_DEBUG_BEGIN("stack=");   \
            json_parser_stack_print_debug( \
                &obj->parser_stack);       \
            PRINT_DEBUG_END();             \
        }                                  \
    } while (0)

#else
#define JSON_PARSE_PRINT_DEBUG(t) \
    do {} while (0)
#define JSON_PARSE_STACK_DEBUG() \
    do {} while (0)
#endif

#undef  PRINT_DEBUG_COND
#define PRINT_DEBUG_COND obj->debug_parser

#define JSON_STATUS(s)                               \
    (                                                \
        obj->current_stat = json_parse_status_ ## s, \
        json_parse_status_ ## s                      \
    )
#define JSON_STATUS_IS(s)                            \
    (                                                \
        obj->current_stat == json_parse_status_ ## s \
    )
#define JSON_CURRENT_STATUS() (obj->current_stat)

static enum json_parse_status_t json_do_parse(
    struct json_obj_t* obj, const uchar_t* buf, size_t len, bool done)
{
    enum json_parser_state_t state;
    struct json_parse_buf_t puf;

    ASSERT(JSON_STATUS_IS(ok));
    ASSERT(JSON_ERROR_TYPE_IS(none));

    if (!JSON_STACK_IS_INIT()) {
        json_parser_stack_init(
            &obj->parser_stack,
            obj->sizes.stack_max,
            obj->sizes.stack_init);

        obj->current_conf = obj->config;
        JSON_PUSH_STATE(start);
    }
    else
    if (JSON_TOP_STATE() == JSON_STATE(done)) {
        ASSERT(JSON_TOKEN_TYPE_IS(eof));

        if (done)
            return JSON_STATUS(ok);

        obj->current_conf = obj->config;
        JSON_SHIFT_STATES(done, start);
    }

    puf.ptr = buf;
    puf.end = buf + len;

next_token:
    JSON_PARSE_PRINT_DEBUG("token");

    if (!done && puf.ptr >= puf.end)
        return JSON_STATUS(ok);

    if (!json_parse_token(obj, &puf, done))
        return JSON_STATUS(error);

    if (!JSON_TOKEN_PART_TYPE_IS(complete) ||
         JSON_TOKEN_TYPE_IS(wsp)) {
        ASSERT(puf.ptr >= puf.end);
        return JSON_STATUS(ok);
    }

next_state:
    JSON_PARSE_PRINT_DEBUG("state");
    JSON_PARSE_STACK_DEBUG();

    state = JSON_TOP_STATE();

    switch (state) {

    case JSON_STATE(start):
        switch (JSON_TOKEN_TYPE()) {

        case JSON_TOKEN(eof):
            if (obj->n_values == 0)
                return JSON_UNEXPECT_TOK();
            JSON_SHIFT_STATES(start, done);
            return JSON_STATUS(ok);

        case JSON_TOKEN(null):
        case JSON_TOKEN(boolean):
        case JSON_TOKEN(number):
        case JSON_TOKEN(string):
        case JSON_TOKEN(left_cbrace):
        case JSON_TOKEN(left_sqbrk):
            if (BITS_INC_MAX(obj->n_values, n_values_bits) > 1) {
                if (!JSON_OBJECT_CONF(allow_multi_objects))
                    return JSON_PARSE_ERROR(multi_objects);
                JSON_CALL_HANDLER(value_sep);
            }
            JSON_PUSH_STATE(value);
            goto next_state;

        default:
            return JSON_UNEXPECT_TOK();
        }

    case JSON_STATE(value):
        switch (JSON_TOKEN_TYPE()) {

        case JSON_TOKEN(null):
            if (!JSON_PARSER_CONF(allow_literal_value))
                return JSON_PARSE_ERROR(literal_value);
            JSON_CALL_HANDLER(null);
            JSON_POP_STATE(value);
            goto next_token;

        case JSON_TOKEN(boolean):
            if (!JSON_PARSER_CONF(allow_literal_value))
                return JSON_PARSE_ERROR(literal_value);
            JSON_CALL_HANDLER_VAL(boolean);
            JSON_POP_STATE(value);
            goto next_token;

        case JSON_TOKEN(number):
            if (!JSON_PARSER_CONF(allow_literal_value))
                return JSON_PARSE_ERROR(literal_value);
            JSON_CALL_HANDLER_BUF(number);
            JSON_POP_STATE(value);
            goto next_token;

        case JSON_TOKEN(string):
            if (!JSON_PARSER_CONF(allow_literal_value))
                return JSON_PARSE_ERROR(literal_value);
            JSON_CALL_HANDLER_BUF(string);
            JSON_POP_STATE(value);
            goto next_token;

        case JSON_TOKEN(left_cbrace):
            JSON_CALL_HANDLER(object_start);
            JSON_SHIFT_STATES(value, object);
            goto next_token;

        case JSON_TOKEN(left_sqbrk):
            JSON_CALL_HANDLER(array_start);
            JSON_SHIFT_STATES(value, array);
            goto next_token;

        default:
            return JSON_UNEXPECT_TOK();
        }

    case JSON_STATE(object):
        switch (JSON_TOKEN_TYPE()) {

        case JSON_TOKEN(right_cbrace):
            JSON_CALL_HANDLER(object_end);
            JSON_POP_STATE(object);
            goto next_token;

        case JSON_TOKEN(string):
            JSON_SHIFT_STATES(
                object, object_arg);
            goto next_state;

        default:
            return JSON_UNEXPECT_TOK();
        }

    case JSON_STATE(object_arg):
        switch (JSON_TOKEN_TYPE()) {

        case JSON_TOKEN(string):
            JSON_CALL_HANDLER_BUF_FUNC(
                object_key, string);
            JSON_SHIFT_STATES(
                object_arg, object_key);
            goto next_token;

        default:
            return JSON_UNEXPECT_TOK();
        }

    case JSON_STATE(object_key):
        switch (JSON_TOKEN_TYPE()) {

        case JSON_TOKEN(colon):
            JSON_SHIFT_STATES(
                object_key, object_value);
            JSON_PUSH_STATE(value);
            goto next_token;

        default:
            return JSON_UNEXPECT_TOK();
        }

    case JSON_STATE(object_value):
        switch (JSON_TOKEN_TYPE()) {

        case JSON_TOKEN(comma):
            JSON_CALL_HANDLER(object_sep);
            JSON_SHIFT_STATES(object_value,
                object_arg);
            goto next_token;

        case JSON_TOKEN(right_cbrace):
            JSON_CALL_HANDLER(object_end);
            JSON_POP_STATE(object_value);
            goto next_token;

        default:
            return JSON_UNEXPECT_TOK();
        }

    case JSON_STATE(array):
        switch (JSON_TOKEN_TYPE()) {

        case JSON_TOKEN(null):
        case JSON_TOKEN(boolean):
        case JSON_TOKEN(number):
        case JSON_TOKEN(string):
        case JSON_TOKEN(left_cbrace):
        case JSON_TOKEN(left_sqbrk):
            JSON_SHIFT_STATES(
                array, array_value);
            JSON_PUSH_STATE(value);
            goto next_state;

        case JSON_TOKEN(right_sqbrk):
            JSON_CALL_HANDLER(array_end);
            JSON_POP_STATE(array);
            goto next_token;

        default:
            return JSON_UNEXPECT_TOK();
        }

    case JSON_STATE(array_value):
        switch (JSON_TOKEN_TYPE()) {

        case JSON_TOKEN(comma):
            JSON_CALL_HANDLER(array_sep);
            JSON_PUSH_STATE(value);
            goto next_token;

        case JSON_TOKEN(right_sqbrk):
            JSON_CALL_HANDLER(array_end);
            JSON_POP_STATE(array_value);
            goto next_token;

        default:
            return JSON_UNEXPECT_TOK();
        }

    default:
        JSON_PARSE_HALT();
    }

    return JSON_STATUS(ok);
}

#ifdef JSON_DEBUG
#define JSON_PARSE_RETURN(e)                               \
    ({                                                     \
        enum json_parse_status_t __r = (e);                \
        PRINT_DEBUG_BEGIN("state=%s token=",               \
            json_parser_state_get_name(JSON_TOP_STATE())); \
        json_token_print_debug(&obj->current_tok,          \
            obj->debug_lexer > 1);                         \
        PRINT_DEBUG_FMT(": return %s",                     \
            json_parse_status_get_name(__r));              \
        PRINT_DEBUG_END();                                 \
        JSON_PARSE_STACK_DEBUG();                          \
        __r;                                               \
    })
#else
#define JSON_PARSE_RETURN(e) e
#endif

enum json_parse_status_t json_parse(
    struct json_obj_t* obj, const uchar_t* buf, size_t len)
{
    if (JSON_STATUS_IS(ok))
        return JSON_PARSE_RETURN(json_do_parse(obj, buf, len, false));
    else
        return JSON_PARSE_RETURN(JSON_CURRENT_STATUS());
}

enum json_parse_status_t json_parse_done(struct json_obj_t* obj)
{
    if (JSON_STATUS_IS(ok))
        return JSON_PARSE_RETURN(json_do_parse(obj, NULL, 0, true));
    else
         return JSON_PARSE_RETURN(JSON_CURRENT_STATUS());
}

struct json_obj_t* json_obj_create(
    const struct json_handler_t* handler, void* handler_obj,
    const struct json_obj_sizes_t* sizes)
{
    struct json_obj_t* obj;

    JSON_OBJ_SIZES_VALIDATE(sizes);

    obj = malloc(sizeof(struct json_obj_t));
    ENSURE(obj != NULL, "malloc failed");

    memset(obj, 0, sizeof(struct json_obj_t));
    obj->handler = handler;
    obj->handler_obj = handler_obj;
    obj->sizes = *sizes;
    obj->current_pos.line = 1;
    obj->current_pos.col = 1;

#ifdef JSON_DEBUG
    obj->debug_parser = SIZE_TRUNC_BITS(
        json_debug_get_level(json_debug_obj_class),
        debug_bits);
    if (obj->debug_parser)
        obj->debug_lexer = obj->debug_parser - 1;
#endif

    json_mem_buf_init(&obj->current_tok_buf,
        obj->sizes.buf_max, obj->sizes.buf_init);

    return obj;
}

void json_obj_destroy(struct json_obj_t* obj)
{
    if (JSON_STACK_IS_INIT())
        json_parser_stack_done(&obj->parser_stack);
    json_mem_buf_done(&obj->current_tok_buf);
    free(obj);
}

bool json_config_get_param(
    struct json_obj_t* obj, enum json_config_param_t param)
{
    switch (param) {
    case json_allow_literal_value_config:
    case json_allow_multi_objects_config:
    case json_disallow_non_ascii_config:
    case json_validate_utf8_config:
        return obj->config & (1U << param);
    default:
        INVALID_ARG("%d", param);
    }
}

void json_config_set_param(
    struct json_obj_t* obj, enum json_config_param_t param,
    bool val)
{
    switch (param) {
    case json_allow_literal_value_config:
    case json_allow_multi_objects_config:
    case json_disallow_non_ascii_config:
    case json_validate_utf8_config:
        if (val)
            obj->config |= (1U << param);
        else
            obj->config &= ~(1U << param);
        break;
    default:
        INVALID_ARG("%d", param);
    }
}

size_t json_parse_get_config(struct json_obj_t* obj)
{
    return JSON_STACK_IS_INIT()
        ? obj->current_conf
        : obj->config;
}

const struct json_error_info_t* json_get_error_info(
    struct json_obj_t* obj)
{
    return &obj->error_info;
}

static const char* json_error_type_get_desc(
    enum json_error_type_t type)
{
    switch (type) {
    case json_error_none:
        return "none";
    case json_error_lex:
        return "lex";
    case json_error_parse:
        return "parse";
    default:
        UNEXPECT_VAR("%d", type);
    }
}

#ifdef JSON_DEBUG

#define CASE(E) case json_parse_status_ ## E: return #E;

const char* json_parse_status_get_name(
    enum json_parse_status_t status)
{
    switch (status) {
    CASE(ok);
    CASE(error);
    CASE(canceled);
    default:
        INVALID_ARG("%d", status);
    }
}

#undef  CASE
#define CASE(E) case json_error_ ## E: return #E;

static const char* json_error_type_get_name(
    enum json_error_type_t type)
{
    switch (type) {
    CASE(none);
    CASE(lex);
    CASE(parse);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

#undef  CASE
#define CASE(E) case json_lex_error_ ## E: return #E;

static const char* json_lex_error_get_name(
    enum json_lex_error_t err)
{
    switch (err) {
    CASE(ok);
    CASE(invalid_char);
    CASE(invalid_literal);
    CASE(invalid_number_literal);
    CASE(invalid_string_literal);
    CASE(invalid_utf8_encoding);
    default:
        UNEXPECT_VAR("%d", err);
    }
}

#endif

static const char* json_lex_error_get_desc(
    enum json_lex_error_t err)
{
    switch (err) {
    case json_lex_error_ok:
        return "OK";
    case json_lex_error_invalid_char:
        return "invalid char";
    case json_lex_error_invalid_literal:
        return "invalid literal";
    case json_lex_error_invalid_number_literal:
        return "invalid number literal";
    case json_lex_error_invalid_string_literal:
        return "invalid string literal";
    case json_lex_error_invalid_utf8_encoding:
        return "invalid utf-8 encoding";
    default:
        UNEXPECT_VAR("%d", err);
    }
}

#ifdef JSON_DEBUG

#undef  CASE
#define CASE(E) case json_parse_error_ ## E: return #E;

static const char* json_parse_error_get_name(
    enum json_parse_error_t err)
{
    switch (err) {
    CASE(ok);
    CASE(literal_value);
    CASE(multi_objects);
    CASE(unexpected_token);
    default:
        UNEXPECT_VAR("%d", err);
    }
}

#endif

static const char* json_parse_error_get_desc(
    enum json_parse_error_t err)
{
    switch (err) {
    case json_parse_error_ok:
        return "OK";
    case json_parse_error_literal_value:
        return "literal values are not allowed";
    case json_parse_error_multi_objects:
        return "multiple objects are not allowed";
    case json_parse_error_unexpected_token:
        return "unexpected token";
    default:
        UNEXPECT_VAR("%d", err);
    }
}

#ifdef JSON_DEBUG

const char* json_error_info_get_name(
    const struct json_error_info_t* info)
{
    switch (info->type) {
    case json_error_none:
        return "none";
    case json_error_lex:
        return json_lex_error_get_name(info->what.lex);
    case json_error_parse:
        return json_parse_error_get_name(info->what.parse);
    default:
        UNEXPECT_VAR("%d", info->type);
    }
}

#endif

const char* json_error_info_get_desc(
    const struct json_error_info_t* info)
{
    switch (info->type) {
    case json_error_none:
        return "-";
    case json_error_lex:
        return json_lex_error_get_desc(info->what.lex);
    case json_error_parse:
        return json_parse_error_get_desc(info->what.parse);
    default:
        UNEXPECT_VAR("%d", info->type);
    }
}

struct json_text_pos_t json_get_current_tok_pos(
    struct json_obj_t* obj)
{
    return obj->current_tok.pos;
}

bool json_get_is_error(struct json_obj_t* obj)
{
    return obj->error_info.type != json_error_none;
}

struct json_error_pos_t json_get_error_pos(struct json_obj_t* obj)
{
    return obj->error_info.pos;
}

const struct json_file_info_t* json_get_error_file(
    struct json_obj_t* obj UNUSED)
{
    return NULL;
}

void json_print_error_desc(struct json_obj_t* obj, FILE* file)
{
    fprintf(file, "%s error: %s",
        json_error_type_get_desc(obj->error_info.type),
        json_error_info_get_desc(&obj->error_info));
}

#ifdef JSON_DEBUG
void json_error_info_print_error_debug(
    const struct json_error_info_t* info, FILE* file)
{
    fprintf(file, "{.type=%s .what=%s .pos={.line=%zu .col=%zu}}",
        json_error_type_get_name(info->type),
        json_error_info_get_name(info),
        info->pos.line, info->pos.col);
}

void json_print_error_debug(struct json_obj_t* obj, FILE* file)
{
    json_error_info_print_error_debug(&obj->error_info, file);
}
#endif


