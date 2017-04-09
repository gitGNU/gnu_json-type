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
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dlfcn.h>
#include <ctype.h>
#include <time.h>

#include "json.h"
#include "json-ast.h"
#include "json-type.h"
#include "int-traits.h"
#include "ptr-traits.h"
#include "pool-alloc.h"
#include "pretty-print.h"
#include "file-buf.h"
#include "common.h"
#include "error.h"

#ifdef JSON_DEBUG
#define PRINT_DEBUG_COND ruler->debug
#include "debug.h"
#endif

#define strucmp(a, b)                                \
    (                                                \
        STATIC(TYPEOF_IS_CHAR_UCHAR_PTR(a)),         \
        STATIC(TYPEOF_IS_CHAR_UCHAR_PTR(b)),         \
        strcmp((const char*) (a), (const char*) (b)) \
    )
#define strlucmp(a, b, n)                                \
    (                                                    \
        STATIC(TYPEOF_IS_SIZET(n)),                      \
        STATIC(TYPEOF_IS_CHAR_UCHAR_PTR(a)),             \
        STATIC(TYPEOF_IS_CHAR_UCHAR_PTR(b)),             \
        strlcmp((const char*) (a), (const char*) (b), n) \
    )
#define strnucmp(a, b, n)                                \
    (                                                    \
        STATIC(TYPEOF_IS_SIZET(n)),                      \
        STATIC(TYPEOF_IS_CHAR_UCHAR_PTR(a)),             \
        STATIC(TYPEOF_IS_CHAR_UCHAR_PTR(b)),             \
        strncmp((const char*) (a), (const char*) (b), n) \
    )
#define strulen(a)                           \
    (                                        \
        STATIC(TYPEOF_IS_CHAR_UCHAR_PTR(a)), \
        strlen((const char*) (a))            \
    )
#define strupbrk(a, b)                                \
    (                                                 \
        STATIC(TYPEOF_IS_CHAR_UCHAR_PTR(a)),          \
        STATIC(TYPEOF_IS_CHAR_UCHAR_PTR(b)),          \
        strpbrk((const char*) (a), (const char*) (b)) \
    )
#define strlcmp(s, b, l)               \
    (                                  \
        STATIC(TYPEOF_IS_CHAR_PTR(s)), \
        STATIC(TYPEOF_IS_CHAR_PTR(b)), \
        STATIC(TYPEOF_IS_SIZET(l)),    \
        (strlen(s) == l) &&            \
        (memcmp(s, b, l) == 0)         \
    )

enum json_type_ruler_basic_type_t
{
    json_type_ruler_basic_type_type,
    json_type_ruler_basic_null_type,
    json_type_ruler_basic_boolean_type,
    json_type_ruler_basic_number_type,
    json_type_ruler_basic_string_type,
    json_type_ruler_basic_object_type,
    json_type_ruler_basic_array_type,
};

typedef void (*json_type_ruler_empty_rule_t)(void*,
    const struct json_text_pos_t*);
typedef void (*json_type_ruler_basic_rule_t)(void*,
    const struct json_text_pos_t*,
    enum json_type_ruler_basic_type_t);
typedef void (*json_type_ruler_boolean_rule_t)(void*,
    const struct json_text_pos_t*,
    bool);
typedef void (*json_type_ruler_string_rule_t)(void*,
    const struct json_text_pos_t*,
    const uchar_t*);
typedef void (*json_type_ruler_size_rule_t)(void*,
    const struct json_text_pos_t*,
    size_t);

struct json_type_ruler_rules_t
{
    json_type_ruler_basic_rule_t   basic_type_rule;
    json_type_ruler_empty_rule_t   plain_null_rule;
    json_type_ruler_boolean_rule_t plain_bool_rule;
    json_type_ruler_string_rule_t  plain_num_rule;
    json_type_ruler_string_rule_t  plain_str_rule;
    json_type_ruler_string_rule_t  arg_name_rule;
    json_type_ruler_size_rule_t    args_object_rule;
    json_type_ruler_empty_rule_t   mono_array_rule;
    json_type_ruler_size_rule_t    args_array_rule;
    json_type_ruler_size_rule_t    args_list_rule;
    json_type_ruler_size_rule_t    args_top_rule;
};

enum { debug_bits = 4 };

struct json_type_ruler_t
{
#ifdef JSON_DEBUG
    bits_t debug:
           debug_bits;
#endif
    const struct json_type_ruler_rules_t*
           rules;
    void*  rules_obj;
};

static void json_type_ruler_init(
    struct json_type_ruler_t* ruler,
    const struct json_type_ruler_rules_t* rules,
    void* rules_obj)
{
    memset(ruler, 0, sizeof(struct json_type_ruler_t));

#ifdef JSON_DEBUG
    ruler->debug = SIZE_TRUNC_BITS(
        json_debug_get_level(json_debug_type_ruler_class),
        debug_bits);
#endif

    ruler->rules = rules;
    ruler->rules_obj = rules_obj;
}

static void json_type_ruler_done(
    struct json_type_ruler_t* ruler UNUSED)
{
    // stev: nop
}

enum json_type_ruler_obj_first_key_t
{
    json_type_ruler_obj_first_key_type,
    json_type_ruler_obj_first_key_name,
    json_type_ruler_obj_first_key_plain
};

// $ . ~/lookup-gen/commands.sh
// $ print() { printf '%s\n' "$@"; }
// $ adjust-func() { ssed -R '1s/^/static /;1s/char/uchar_t/;1s/\&/*/;s/(,\s+)/\1enum /;s/(?=t =)/*/;1s/(?<=\()/\n\t/;s/([a-z0-9_]+_)t::([a-z]+)/\1\2/'; }

// $ print type name plain|gen-func -f json_type_ruler_lookup_obj_first_key -r json_type_ruler_obj_first_key_t|adjust-func

static bool json_type_ruler_lookup_obj_first_key(
    const uchar_t* n, enum json_type_ruler_obj_first_key_t* t)
{
    // pattern: name|plain|type
    switch (*n ++) {
    case 'n':
        if (*n ++ == 'a' &&
            *n ++ == 'm' &&
            *n ++ == 'e' &&
            *n == 0) {
            *t = json_type_ruler_obj_first_key_name;
            return true;
        }
        return false;
    case 'p':
        if (*n ++ == 'l' &&
            *n ++ == 'a' &&
            *n ++ == 'i' &&
            *n ++ == 'n' &&
            *n == 0) {
            *t = json_type_ruler_obj_first_key_plain;
            return true;
        }
        return false;
    case 't':
        if (*n ++ == 'y' &&
            *n ++ == 'p' &&
            *n ++ == 'e' &&
            *n == 0) {
            *t = json_type_ruler_obj_first_key_type;
            return true;
        }
    }
    return false;
}

enum json_type_ruler_obj_type_t
{
    json_type_ruler_obj_type_object,
    json_type_ruler_obj_type_array,
    json_type_ruler_obj_type_list
};

// $ print object array list|gen-func -f json_type_ruler_lookup_obj_type -r json_type_ruler_obj_type_t|adjust-func

static bool json_type_ruler_lookup_obj_type(
    const uchar_t* n, enum json_type_ruler_obj_type_t* t)
{
    // pattern: array|list|object
    switch (*n ++) {
    case 'a':
        if (*n ++ == 'r' &&
            *n ++ == 'r' &&
            *n ++ == 'a' &&
            *n ++ == 'y' &&
            *n == 0) {
            *t = json_type_ruler_obj_type_array;
            return true;
        }
        return false;
    case 'l':
        if (*n ++ == 'i' &&
            *n ++ == 's' &&
            *n ++ == 't' &&
            *n == 0) {
            *t = json_type_ruler_obj_type_list;
            return true;
        }
        return false;
    case 'o':
        if (*n ++ == 'b' &&
            *n ++ == 'j' &&
            *n ++ == 'e' &&
            *n ++ == 'c' &&
            *n ++ == 't' &&
            *n == 0) {
            *t = json_type_ruler_obj_type_object;
            return true;
        }
    }
    return false;
}

#define JSON_TYPE_RULER_RESULT(t, p, v)              \
    ({                                               \
        struct json_type_ruler_result_t __r;         \
        __r.type = json_type_ruler_ ## t ## _result; \
        __r.t = v;                                   \
        __r.pos = p;                                 \
        __r;                                         \
    })

#define JSON_TYPE_RULER_ERROR_POS(p, e) \
    JSON_TYPE_RULER_RESULT(err, p,      \
        json_type_ruler_error_ ## e)

#define JSON_TYPE_RULER_ERROR_NODE(n, t, e)     \
    ({                                          \
        const struct json_ast_node_t* __q =     \
            JSON_AST_TO_NODE_CONST(n, t);       \
        JSON_TYPE_RULER_ERROR_POS(__q->pos, e); \
    })

#define JSON_TYPE_RULER_ERROR(t, e) \
    JSON_TYPE_RULER_ERROR_NODE(node, t, e)

#define JSON_TYPE_RULER_VAL_(t, o)                \
    ({                                            \
        const struct json_ast_node_t* __q =       \
            JSON_AST_TO_NODE_CONST(node, t);      \
        JSON_TYPE_RULER_RESULT(val, __q->pos, o); \
    })

#define JSON_TYPE_RULER_VAL(t, n)              \
    JSON_TYPE_RULER_VAL_(                      \
        t, json_type_ruler_val_ ## n ## _type)

#define JSON_TYPE_RULER_VAL_CONST(t) \
    JSON_TYPE_RULER_VAL(t, const)

#define JSON_TYPE_RULER_CALL_RULE(t, n, ...)     \
    do {                                         \
        if (ruler->rules != NULL &&              \
            ruler->rules->n ## _rule != NULL) {  \
            const struct json_ast_node_t* __n =  \
                JSON_AST_TO_NODE_CONST(node, t); \
            ruler->rules->n ## _rule(            \
                ruler->rules_obj, &__n->pos,     \
                ## __VA_ARGS__);                 \
        }                                        \
    } while (0)

#define JSON_TYPE_RULER_CALL_RULE_POS(n, p, ...) \
    do {                                         \
        if (ruler->rules != NULL &&              \
            ruler->rules->n ## _rule != NULL) {  \
            ruler->rules->n ## _rule(            \
                ruler->rules_obj, &p,            \
                ## __VA_ARGS__);                 \
        }                                        \
    } while (0)

#define JSON_TYPE_RULER_VAL_BASIC(v)                      \
    ({                                                    \
        JSON_TYPE_RULER_CALL_RULE(string, basic_type, v); \
        JSON_TYPE_RULER_VAL(string, basic);               \
    })

#define JSON_TYPE_RULER_VAL_PLAIN_NULL_OBJ()           \
    ({                                                 \
        JSON_TYPE_RULER_CALL_RULE(object, plain_null); \
        JSON_TYPE_RULER_VAL(object, plain_obj);        \
    })

#define JSON_TYPE_RULER_VAL_PLAIN_BOOLEAN_OBJ(v)          \
    ({                                                    \
        JSON_TYPE_RULER_CALL_RULE(object, plain_bool, v); \
        JSON_TYPE_RULER_VAL(object, plain_obj);           \
    })

#define JSON_TYPE_RULER_VAL_PLAIN_NUMBER_OBJ(v)          \
    ({                                                   \
        JSON_TYPE_RULER_CALL_RULE(object, plain_num, v); \
        JSON_TYPE_RULER_VAL(object, plain_obj);          \
    })

#define JSON_TYPE_RULER_VAL_PLAIN_STRING_OBJ(v)          \
    ({                                                   \
        JSON_TYPE_RULER_CALL_RULE(object, plain_str, v); \
        JSON_TYPE_RULER_VAL(object, plain_obj);          \
    })

#define JSON_TYPE_RULER_VAL_OBJECT_OBJ(v)                  \
    ({                                                     \
        JSON_TYPE_RULER_CALL_RULE(object, args_object, v); \
        JSON_TYPE_RULER_VAL(object, object_obj);           \
    })

#define JSON_TYPE_RULER_VAL_MONO_ARRAY_OBJ()           \
    ({                                                 \
        JSON_TYPE_RULER_CALL_RULE(object, mono_array); \
        JSON_TYPE_RULER_VAL(object, array_obj);        \
    })

#define JSON_TYPE_RULER_VAL_ARGS_ARRAY_OBJ(v)             \
    ({                                                    \
        JSON_TYPE_RULER_CALL_RULE(object, args_array, v); \
        JSON_TYPE_RULER_VAL(object, array_obj);           \
    })

#define JSON_TYPE_RULER_VAL_LIST_OBJ(v)                  \
    ({                                                   \
        JSON_TYPE_RULER_CALL_RULE(object, args_list, v); \
        JSON_TYPE_RULER_VAL(object, list_obj);           \
    })

#define JSON_TYPE_RULER_VAL_NAME_OBJ(p, v)             \
    ({                                                 \
        JSON_TYPE_RULER_CALL_RULE_POS(arg_name, p, v); \
        JSON_TYPE_RULER_VAL(object, name_obj);         \
    })

#define JSON_TYPE_RULER_VAL_TOP_OBJ(v) \
    JSON_TYPE_RULER_CALL_RULE(array, args_top, v)

#define JSON_TYPE_RULER_VAL_ARRAY_OF(n) \
    JSON_TYPE_RULER_VAL(array, array_of_ ## n)

#define JSON_TYPE_RULER_TYPEOF_IS_RESULT(r) \
    TYPEOF_IS(r, struct json_type_ruler_result_t)

#define JSON_TYPE_RULER_RESULT_IS(r, n)              \
    (                                                \
        STATIC(JSON_TYPE_RULER_TYPEOF_IS_RESULT(r)), \
        r.type == json_type_ruler_ ## n ## _result   \
    )

#define JSON_TYPE_RULER_RESULT_IS_ERROR(r) \
    JSON_TYPE_RULER_RESULT_IS(r, err)

#define JSON_TYPE_RULER_RESULT_IS_VAL(r) \
    JSON_TYPE_RULER_RESULT_IS(r, val)

#define JSON_TYPE_RULER_VAL_TYPE(n) \
    json_type_ruler_val_ ## n ## _type

#define JSON_TYPE_RULER_RESULT_VAL_IS_TYPE(r)        \
    ({                                               \
        ASSERT(JSON_TYPE_RULER_RESULT_IS_VAL(r));    \
        r.val >= JSON_TYPE_RULER_VAL_TYPE(basic) &&  \
        r.val <= JSON_TYPE_RULER_VAL_TYPE(list_obj); \
    })

#define JSON_TYPE_RULER_RESULT_VAL_IS(r, n)  \
    (                                        \
        JSON_TYPE_RULER_RESULT_IS_VAL(r) &&  \
        r.val == JSON_TYPE_RULER_VAL_TYPE(n) \
    )

#define JSON_TYPE_RULER_RESULT_VAL_IS_OBJ(r, n) \
    JSON_TYPE_RULER_RESULT_VAL_IS(r, n ## _obj)

#define JSON_TYPE_RULER_RESULT_VAL_IS_NAME_OBJ(r) \
    JSON_TYPE_RULER_RESULT_VAL_IS_OBJ(r, name)

#define JSON_TYPE_RULER_RESULT_VAL_IS_PLAIN_OBJ(r) \
    JSON_TYPE_RULER_RESULT_VAL_IS_OBJ(r, plain)

#define JSON_TYPE_RULER_RESULT_VAL_IS_ARRAY_OF(r, n) \
    JSON_TYPE_RULER_RESULT_VAL_IS(r, array_of_ ## n)

enum json_type_ruler_val_type_t
{
    json_type_ruler_val_const_type,
    json_type_ruler_val_basic_type,
    json_type_ruler_val_plain_obj_type,
    json_type_ruler_val_object_obj_type,
    json_type_ruler_val_array_obj_type,
    json_type_ruler_val_list_obj_type,
    json_type_ruler_val_name_obj_type,
    json_type_ruler_val_array_of_types_type,
    json_type_ruler_val_array_of_name_objs_type,
    json_type_ruler_val_array_of_size_zero_type,
};

// $ print null type boolean number string object array|gen-func -f json_type_ruler_is_basic_type -r json_type_ruler_basic_type_t|adjust-func

static bool json_type_ruler_lookup_basic_type(
    const uchar_t* n, enum json_type_ruler_basic_type_t* t)
{
    // pattern: array|boolean|nu(ll|mber)|object|string|type
    switch (*n ++) {
    case 'a':
        if (*n ++ == 'r' &&
            *n ++ == 'r' &&
            *n ++ == 'a' &&
            *n ++ == 'y' &&
            *n == 0) {
            *t = json_type_ruler_basic_array_type;
            return true;
        }
        return false;
    case 'b':
        if (*n ++ == 'o' &&
            *n ++ == 'o' &&
            *n ++ == 'l' &&
            *n ++ == 'e' &&
            *n ++ == 'a' &&
            *n ++ == 'n' &&
            *n == 0) {
            *t = json_type_ruler_basic_boolean_type;
            return true;
        }
        return false;
    case 'n':
        if (*n ++ == 'u') {
            switch (*n ++) {
            case 'l':
                if (*n ++ == 'l' &&
                    *n == 0) {
                    *t = json_type_ruler_basic_null_type;
                    return true;
                }
                return false;
            case 'm':
                if (*n ++ == 'b' &&
                    *n ++ == 'e' &&
                    *n ++ == 'r' &&
                    *n == 0) {
                    *t = json_type_ruler_basic_number_type;
                    return true;
                }
            }
        }
        return false;
    case 'o':
        if (*n ++ == 'b' &&
            *n ++ == 'j' &&
            *n ++ == 'e' &&
            *n ++ == 'c' &&
            *n ++ == 't' &&
            *n == 0) {
            *t = json_type_ruler_basic_object_type;
            return true;
        }
        return false;
    case 's':
        if (*n ++ == 't' &&
            *n ++ == 'r' &&
            *n ++ == 'i' &&
            *n ++ == 'n' &&
            *n ++ == 'g' &&
            *n == 0) {
            *t = json_type_ruler_basic_string_type;
            return true;
        }
        return false;
    case 't':
        if (*n ++ == 'y' &&
            *n ++ == 'p' &&
            *n ++ == 'e' &&
            *n == 0) {
            *t = json_type_ruler_basic_type_type;
            return true;
        }
    }
    return false;
}

enum json_type_ruler_result_type_t
{
    json_type_ruler_err_result,
    json_type_ruler_val_result,
};

struct json_type_ruler_result_t
{
    enum json_type_ruler_result_type_t type;
    union {
        enum json_type_ruler_error_type_t err;
        enum json_type_ruler_val_type_t   val;
    };
    struct json_text_pos_t pos;
};

#ifdef JSON_DEBUG

#undef  CASE
#define CASE(E) \
    case json_type_ruler_error_ ## E: return #E

static const char* json_type_ruler_error_type_get_name(
    enum json_type_ruler_error_type_t type)
{
    switch (type) {
    CASE(invalid_top_value);
    CASE(empty_obj_not_allowed);
    CASE(invalid_obj_first_key);
    CASE(invalid_type_obj_size_not_two);
    CASE(invalid_name_obj_size_not_two);
    CASE(invalid_plain_obj_size_not_one);
    CASE(invalid_type_obj_type_not_obj_arr_list);
    CASE(invalid_type_obj_2nd_arg_not_args);
    CASE(invalid_name_obj_2nd_arg_not_type);
    CASE(invalid_plain_obj_type_not_null_bool_num_str);
    CASE(invalid_name_obj_name_not_str);
    CASE(invalid_name_obj_type_arg_not_a_type);
    CASE(invalid_object_obj_args_not_array_of_name_objs);
    CASE(invalid_list_obj_args_not_of_type_array_of_types);
    CASE(invalid_array_obj_args_neither_type_nor_array_of_types);
    CASE(invalid_array_elem_is_neither_type_nor_name_obj);
    CASE(invalid_array_elem_is_either_type_xor_name_obj);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

#undef  CASE
#define CASE(E) \
    case json_type_ruler_val_ ## E ## _type: return #E

static const char* json_type_ruler_val_type_get_name(
    enum json_type_ruler_val_type_t type)
{
    switch (type) {
    CASE(const);
    CASE(basic);
    CASE(plain_obj);
    CASE(object_obj);
    CASE(array_obj);
    CASE(list_obj);
    CASE(name_obj);
    CASE(array_of_types);
    CASE(array_of_name_objs);
    CASE(array_of_size_zero);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

#undef  CASE
#define CASE(E) \
    case json_type_ruler_ ## E ## _result: return #E

static const char* json_type_ruler_result_type_get_name(
    enum json_type_ruler_result_type_t type)
{
    switch (type) {
    CASE(err);
    CASE(val);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

static void json_type_ruler_result_print_debug(
    const struct json_type_ruler_result_t* result,
    FILE* file)
{
    fprintf(file, "{.type=%s ",
        json_type_ruler_result_type_get_name(
            result->type));
    switch (result->type) {
    case json_type_ruler_err_result:
        fprintf(file, ".err=%s",
            json_type_ruler_error_type_get_name(
                result->err));
        break;
    case json_type_ruler_val_result:
        fprintf(file, ".val=%s",
            json_type_ruler_val_type_get_name(
                result->val));
        break;
    default:
        UNEXPECT_VAR("%d", result->type);
    }
    fprintf(file, " .pos={.line=%zu .col=%zu}}",
        result->pos.line, result->pos.col);
}

#define JSON_TYPE_RULER_RESULT_PRINT_DEBUG(r)           \
    do {                                                \
        PRINT_DEBUG_BEGIN(#r "=");                      \
        json_type_ruler_result_print_debug(&r, stderr); \
        PRINT_DEBUG_END();                              \
    } while (0)

#else
#define JSON_TYPE_RULER_RESULT_PRINT_DEBUG(r) \
    do {} while (0)
#endif

// stev: meta-rules: 
// 
// (1)  key      type of attached value
//      -------  -------------------------------
//      "type"   $any, where $any is any type
//      "args"   $any or by case `[ $any* ]' (see below)
//      "name"   string
//      "plain"  null, boolean, number or string
//
// (2) when "type" is the first key of an object then its associated value must
//     be either `"object"', `"array"' or `"list"'
//
// (3) the first key of an object must be either `"type"', `"name"' or `"plain"';
//
// (4) the first key of an object determines completely its type:
//       (a) when it is `"type"': the object must have exactly two arguments;
//           the key of the second argument must be `"args"';
//       (b) when it is `"name"': the object must have exactly two arguments;
//           the key of the second argument must be `"type"'
//       (c) when it is `"plain"': the object must have exactly one argument
//
// (5) the arrays are present only associated as values to the key `"args"'
//       of an object of type of form: `{ "type": ..., "args": [ ... ] }';
//       the arrays themselves are not types
//
// (6) the arrays are of only two kinds:
//       (a) of uniform element type `{ "name": $string, "type": $type }' where
//           $string is a string and $type is any type
//       (b) of elements which are types;
//
// (7) values of type boolean, number or string can only appear associated to
//       the key `"plain"'; for strings there are a few exceptions which are
//       listed at point (9.a) below
//
// (8) the `null' value must only occur as the value associated to the key
//       `"plain"'
//
// (9) the types are precisely the following kind of values:
//       (a) `"type"', `"null"', `"boolean"', `"number"', `"string"', `"object"',
//           and `"array"'; note that each of these are exempted from being types
//           when occurring as values associated to the key `"plain"' of objects
//           of form `{ "plain": ... }' or to the key `"name"' of objects of form
//           `{ "name": ..., "type": ... }'; in such cases they are interpreted
//           as proper values
//       (b) the objects of form `{ "type": $type, "args": $args }' described
//           above; $type is allowed to be only `"object"', `"array"' or `"list"',
//           as stated by point (2); $args must be as follows:
//               (i) `[ { "name": $string, "type": $any }* ]', when
//                   $type is `"object"'
//              (ii) `[ $any* ]' or $any, when $type is `"array"'
//             (iii) `[ $any* ]', when $type is `"list"'
//           where $any is any type
//       (c) the objects of form `{ "plain": $value }' where $value is allowed
//           to be only `null' or of type boolean, number or string

static struct json_type_ruler_result_t
    json_type_ruler_visit(
        struct json_type_ruler_t* ruler,
        const struct json_ast_node_t* node);

static struct json_type_ruler_result_t
    json_type_ruler_null(
        struct json_type_ruler_t* ruler UNUSED, 
        const struct json_ast_null_node_t* node)
{
    // meta-rule (8)
    return JSON_TYPE_RULER_VAL_CONST(null);
}

static struct json_type_ruler_result_t
    json_type_ruler_boolean(
        struct json_type_ruler_t* ruler UNUSED,
        const struct json_ast_boolean_node_t* node)
{
    // meta-rule (7)
    return JSON_TYPE_RULER_VAL_CONST(boolean);
}

static struct json_type_ruler_result_t
    json_type_ruler_number(
        struct json_type_ruler_t* ruler UNUSED, 
        const struct json_ast_number_node_t* node)
{
    // meta-rule (7)
    return JSON_TYPE_RULER_VAL_CONST(number);
}

static struct json_type_ruler_result_t
    json_type_ruler_string(
        struct json_type_ruler_t* ruler,
        const struct json_ast_string_node_t* node)
{
    enum json_type_ruler_basic_type_t t;

    if (json_type_ruler_lookup_basic_type(
            node->buf.ptr, &t))
        // meta-rule (9.a)
        return JSON_TYPE_RULER_VAL_BASIC(t);
    else
        // meta-rule (7)
        return JSON_TYPE_RULER_VAL_CONST(string);
}

static struct json_type_ruler_result_t
    json_type_ruler_object(
        struct json_type_ruler_t* ruler,
        const struct json_ast_object_node_t* node)
{
    struct json_type_ruler_result_t r;
    enum json_type_ruler_obj_first_key_t k;
    const struct json_ast_string_node_t* s;
    const struct json_ast_array_node_t* a;
    const struct json_ast_node_t *v, *w;
    enum json_type_ruler_obj_type_t t;

    // meta-rule (4)
    if (node->size == 0)
        return JSON_TYPE_RULER_ERROR(
            object, empty_obj_not_allowed);

    s = node->args[0].key;
    // meta-rule (3)
    if (!json_type_ruler_lookup_obj_first_key(
            s->buf.ptr, &k))
        return JSON_TYPE_RULER_ERROR_NODE(
            s, string, invalid_obj_first_key);

    switch (k) {

    // `{ "type": ... }'
    case json_type_ruler_obj_first_key_type:
        // meta-rule (2)
        v = node->args[0].val;
        if ((s = JSON_AST_NODE_AS_IF_CONST(v, string)) == NULL ||
            !json_type_ruler_lookup_obj_type(s->buf.ptr, &t))
            return JSON_TYPE_RULER_ERROR_POS(
                v->pos, invalid_type_obj_type_not_obj_arr_list);

        // meta-rule (4.a)
        if (node->size != 2)
            return JSON_TYPE_RULER_ERROR(
                object, invalid_type_obj_size_not_two);

        s = node->args[1].key;
        // meta-rule (4.a)
        if (strucmp(s->buf.ptr, "args"))
            return JSON_TYPE_RULER_ERROR(
                object, invalid_type_obj_2nd_arg_not_args);

        v = node->args[1].val;
        r = json_type_ruler_visit(ruler, v);

        JSON_TYPE_RULER_RESULT_PRINT_DEBUG(r);

        if (JSON_TYPE_RULER_RESULT_IS_ERROR(r))
            return r;

        switch (t) {

        case json_type_ruler_obj_type_object:
            // meta-rule (9.b.i)
            if (JSON_TYPE_RULER_RESULT_VAL_IS_ARRAY_OF(r, name_objs) ||
                JSON_TYPE_RULER_RESULT_VAL_IS_ARRAY_OF(r, size_zero)) {
                a = JSON_AST_NODE_AS_CONST(v, array);
                return JSON_TYPE_RULER_VAL_OBJECT_OBJ(a->size);
            }
            else
                return JSON_TYPE_RULER_ERROR_POS(v->pos,
                    invalid_object_obj_args_not_array_of_name_objs);

        case json_type_ruler_obj_type_array:
            // meta-rule (9.b.ii)
            if (JSON_TYPE_RULER_RESULT_VAL_IS_TYPE(r))
                return JSON_TYPE_RULER_VAL_MONO_ARRAY_OBJ();
            if (JSON_TYPE_RULER_RESULT_VAL_IS_ARRAY_OF(r, types) ||
                JSON_TYPE_RULER_RESULT_VAL_IS_ARRAY_OF(r, size_zero)) {
                a = JSON_AST_NODE_AS_CONST(v, array);
                return JSON_TYPE_RULER_VAL_ARGS_ARRAY_OBJ(a->size);
            }
            else
                return JSON_TYPE_RULER_ERROR_POS(v->pos,
                    invalid_array_obj_args_neither_type_nor_array_of_types);

        case json_type_ruler_obj_type_list:
            // meta-rule (9.b.iii)
            if (JSON_TYPE_RULER_RESULT_VAL_IS_ARRAY_OF(r, types) ||
                JSON_TYPE_RULER_RESULT_VAL_IS_ARRAY_OF(r, size_zero)) {
                a = JSON_AST_NODE_AS_CONST(v, array);
                return JSON_TYPE_RULER_VAL_LIST_OBJ(a->size);
            }
            else
                return JSON_TYPE_RULER_ERROR_POS(v->pos,
                    invalid_list_obj_args_not_of_type_array_of_types);

        default:
            UNEXPECT_VAR("%d", t);
        }

    // `{ "name": ... }'
    case json_type_ruler_obj_first_key_name:
        // meta-rule (4.b)
        if (node->size != 2)
            return JSON_TYPE_RULER_ERROR(
                object, invalid_name_obj_size_not_two);

        s = node->args[1].key;
        // meta-rule (4.b)
        if (strucmp(s->buf.ptr, "type"))
            return JSON_TYPE_RULER_ERROR(
                object, invalid_name_obj_2nd_arg_not_type);

        w = node->args[0].val;
        // meta-rule (1) and (9.a)
        if (w->type != json_ast_string_node_type)
            return JSON_TYPE_RULER_ERROR_POS(
                w->pos, invalid_name_obj_name_not_str);

        v = node->args[1].val;
        r = json_type_ruler_visit(ruler, v);

        JSON_TYPE_RULER_RESULT_PRINT_DEBUG(r);

        if (JSON_TYPE_RULER_RESULT_IS_ERROR(r))
            return r;

        // meta-rule (1)
        if (!JSON_TYPE_RULER_RESULT_VAL_IS_TYPE(r))
            return JSON_TYPE_RULER_ERROR_POS(
                r.pos, invalid_name_obj_type_arg_not_a_type);

        s = JSON_AST_NODE_AS_CONST(w, string);
        return JSON_TYPE_RULER_VAL_NAME_OBJ(w->pos, s->buf.ptr);

    // `{ "plain": ... }'
    case json_type_ruler_obj_first_key_plain:
        // meta-rule (4.c)
        if (node->size != 1)
            return JSON_TYPE_RULER_ERROR(
                object, invalid_plain_obj_size_not_one);

        v = node->args[0].val;
        // meta-rule (9.a) and (9.c)
        if (v->type == json_ast_null_node_type)
            return JSON_TYPE_RULER_VAL_PLAIN_NULL_OBJ();
        if (v->type == json_ast_boolean_node_type)
            return JSON_TYPE_RULER_VAL_PLAIN_BOOLEAN_OBJ(
                v->boolean.val);
        if (v->type == json_ast_number_node_type)
            return JSON_TYPE_RULER_VAL_PLAIN_NUMBER_OBJ(
                v->number.buf.ptr);
        if (v->type == json_ast_string_node_type)
            return JSON_TYPE_RULER_VAL_PLAIN_STRING_OBJ(
                v->string.buf.ptr);
        else
            return JSON_TYPE_RULER_ERROR_POS(v->pos,
                invalid_plain_obj_type_not_null_bool_num_str);

    default:
        UNEXPECT_VAR("%d", k);
    }
}

static struct json_type_ruler_result_t
    json_type_ruler_array(
        struct json_type_ruler_t* ruler,
        const struct json_ast_array_node_t* node)
{
    struct json_type_ruler_result_t r;
    struct json_ast_node_t *const *p, *const *e;
    size_t n = 0, t = 0;

    for (p = node->args,
         e = p + node->size;
         p < e;
         p ++) {
        bool b0, b1;

        r = json_type_ruler_visit(ruler, *p);

        JSON_TYPE_RULER_RESULT_PRINT_DEBUG(r);

        if (JSON_TYPE_RULER_RESULT_IS_ERROR(r))
            return r;

        b0 = JSON_TYPE_RULER_RESULT_VAL_IS_NAME_OBJ(r);
        b1 = JSON_TYPE_RULER_RESULT_VAL_IS_TYPE(r);

        // meta-rule (6)
        if (!b0 && !b1)
            return JSON_TYPE_RULER_ERROR_POS(
                r.pos, invalid_array_elem_is_neither_type_nor_name_obj);

        n += b0;
        t += b1;

        if (n && t)
            return JSON_TYPE_RULER_ERROR_POS(
                r.pos, invalid_array_elem_is_either_type_xor_name_obj);
    }
    ASSERT(
        (n == 0 && t == node->size) ||
        (t == 0 && n == node->size));

    // stev: when 'node->size' is zero the array can be
    // interpreted as either of type 'named_objs' or 'types'
    if (node->size == 0)
        return JSON_TYPE_RULER_VAL_ARRAY_OF(size_zero);
    // meta-rule (6.a)
    if (n == node->size)
        return JSON_TYPE_RULER_VAL_ARRAY_OF(name_objs);
    // meta-rule (6.b)
    else
        return JSON_TYPE_RULER_VAL_ARRAY_OF(types);
}

#define JSON_AST_VISITOR_TYPE        1
#define JSON_AST_VISITOR_NAME        json_type_ruler
#define JSON_AST_VISITOR_RESULT_TYPE struct json_type_ruler_result_t
#define JSON_AST_VISITOR_OBJECT_TYPE struct json_type_ruler_t

#define JSON_AST_NEED_VISITOR
#include "json-ast-impl.h"
#undef  JSON_AST_NEED_VISITOR

#define JSON_TYPE_LIB_META_ERROR(e, p) \
    ({                                 \
        err->type = e;                 \
        err->pos = p;                  \
        false;                         \
    })

#define JSON_TYPE_LIB_META_ERROR_NAME(n, p) \
    JSON_TYPE_LIB_META_ERROR(json_type_ruler_error_ ## n, p)

static bool json_type_ruler_eval(
    struct json_type_ruler_t* ruler,
    const struct json_ast_t* ast,
    struct json_type_lib_error_meta_t* err)
{
    struct json_type_ruler_result_t r;
    const struct json_ast_node_t* n;
    bool a = false;

    ASSERT(ast != NULL);

    n = json_ast_get_root(ast);
    r = json_type_ruler_visit(ruler, n);

    JSON_TYPE_RULER_RESULT_PRINT_DEBUG(r);

    if (JSON_TYPE_RULER_RESULT_IS_ERROR(r))
        return JSON_TYPE_LIB_META_ERROR(r.err, r.pos);

    if (!JSON_TYPE_RULER_RESULT_VAL_IS_TYPE(r) &&
        !(a = JSON_TYPE_RULER_RESULT_VAL_IS_ARRAY_OF(
            r, name_objs)))
        return JSON_TYPE_LIB_META_ERROR_NAME(
            invalid_top_value, r.pos);

    if (a) {
        const struct json_ast_array_node_t* a;

        a = JSON_AST_NODE_AS_CONST(n, array);
#define node a
        JSON_TYPE_RULER_VAL_TOP_OBJ(a->size);
#undef  node
    }

    return true;
}

#undef  CASE
#define CASE(E) case json_type_ruler_error_ ## E

static void json_type_ruler_print_error_desc(
    enum json_type_ruler_error_type_t type, FILE* file)
{
    switch (type) {

    CASE(invalid_top_value):
        fputs("invalid top value: it must be a type or"
            " an array of \"name\" objects", file);
        break;

    CASE(empty_obj_not_allowed):
        fputs("empty objects are not allowed", file);
        break;

    CASE(invalid_obj_first_key):
        fputs("invalid first key of object: it must be "
            "\"type\", \"name\" or \"plain\"", file);
        break;

    CASE(invalid_type_obj_type_not_obj_arr_list):
        fputs("invalid \"type\" object: type not \"object\", "
            "\"array\" or \"list\"", file);
        break;

    CASE(invalid_type_obj_size_not_two):
        fputs("invalid \"type\" object: arguments not of size two", file);
        break;

    CASE(invalid_name_obj_size_not_two):
        fputs("invalid \"name\" object: arguments not of size two", file);
        break;

    CASE(invalid_plain_obj_size_not_one):
        fputs("invalid \"plain\" object: arguments not of size one", file);
        break;

    CASE(invalid_type_obj_2nd_arg_not_args):
        fputs("invalid \"type\" object: key of second argument is not \"args\"",
            file);
        break;

    CASE(invalid_name_obj_2nd_arg_not_type):
        fputs("invalid \"name\" object: key of second argument is not \"type\"",
            file);
        break;

    CASE(invalid_plain_obj_type_not_null_bool_num_str):
        fputs("invalid \"plain\" object: value must be `null', "
            "a boolean, a number or a string", file);
        break;

    CASE(invalid_name_obj_name_not_str):
        fputs("invalid \"name\" object: value of \"name\" must be a string",
            file);
        break;

    CASE(invalid_name_obj_type_arg_not_a_type):
        fputs("invalid \"name\" object: its \"type\" argument must be a type",
            file);
        break;

    CASE(invalid_object_obj_args_not_array_of_name_objs):
        fputs("invalid \"object\" object: value of \"args\" must be an array"
            " of \"name\" objects", file);
        break;

    CASE(invalid_list_obj_args_not_of_type_array_of_types):
        fputs("invalid \"list\" object: value of \"args\" must be an array "
            "of types", file);
        break;

    CASE(invalid_array_obj_args_neither_type_nor_array_of_types):
        fputs("invalid \"array\" object: value of \"args\" must be a type or "
            "an array of types", file);
        break;

    CASE(invalid_array_elem_is_neither_type_nor_name_obj):
        fputs("invalid array: element is neither a type nor a \"name\" object",
            file);
        break;

    CASE(invalid_array_elem_is_either_type_xor_name_obj):
        fputs("invalid array: its elements must all be types or all be "
            "\"name\" objects", file);
        break;

    default:
        UNEXPECT_VAR("%d", type);
    }
}

static struct json_error_pos_t json_type_lib_error_info_get_pos(
    const struct json_type_lib_error_info_t* info)
{
    switch (info->type) {
    case json_type_lib_error_none:
    case json_type_lib_error_oper:
    case json_type_lib_error_sys:
    case json_type_lib_error_lib: {
        struct json_error_pos_t p = {
            .col = 0, .line = 0
        };
        return p;
    }
    case json_type_lib_error_ast:
        return info->ast.pos;
    case json_type_lib_error_meta:
        return info->meta.pos;
    case json_type_lib_error_attr:
        return info->attr.pos[0];
    default:
        UNEXPECT_VAR("%d", info->type);
    }
}

static const struct json_file_info_t* json_type_lib_error_info_get_file(
    const struct json_type_lib_error_info_t* info)
{
    switch (info->type) {
    case json_type_lib_error_none:
        return NULL;
    case json_type_lib_error_oper:
    case json_type_lib_error_sys:
    case json_type_lib_error_lib:
    case json_type_lib_error_ast:
    case json_type_lib_error_meta:
    case json_type_lib_error_attr:
        return &info->file_info;
    default:
        UNEXPECT_VAR("%d", info->type);
    }
}

#undef  CASE
#define CASE(n) \
    case json_type_lib_error_oper_ ## n: return #n

static const char* json_type_lib_error_oper_type_get_name(
    enum json_type_lib_error_oper_type_t type)
{
    switch (type) {
    CASE(parse);
    CASE(parse_done);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

#undef  CASE
#define CASE(n, v) \
    [json_type_lib_error_oper_ ## n ## _context] = v

static void json_type_lib_error_oper_print_desc(
    const struct json_type_lib_error_oper_t* error,
    FILE* file)
{
    static const char* const contexts[] = {
        CASE(sobj, "shared library")
    };
    fprintf(file, "%s: operation '%s' not applicable",
        ARRAY_NON_NULL_ELEM(contexts, error->context),
        json_type_lib_error_oper_type_get_name(
            error->type));
}

#ifdef JSON_DEBUG

#undef  CASE
#define CASE(n) \
    [json_type_lib_error_oper_ ## n ## _context] = #n

static void json_type_lib_error_oper_print_debug(
    const struct json_type_lib_error_oper_t* error,
    FILE* file)
{
    static const char* const contexts[] = {
        CASE(sobj)
    };
    fprintf(file, ".type = %s .context=%s",
        json_type_lib_error_oper_type_get_name(
            error->type),
        ARRAY_NON_NULL_ELEM(contexts, error->context));
}

#endif

static const char* json_type_lib_error_sys_context_get_desc(
    enum json_type_lib_error_sys_context_t context)
{
    switch (context) {
    case json_type_lib_error_file_open_context:
        return "file open";
    case json_type_lib_error_file_stat_context:
        return "file stat";
    case json_type_lib_error_file_read_context:
        return "file read";
    case json_type_lib_error_file_close_context:
        return "file close";
    default:
        UNEXPECT_VAR("%d", context);
    }
}

#undef  CASE
#define CASE(n) case json_type_lib_generic_lib_error_ ## n

static void json_type_lib_generic_lib_print_error_desc(
    enum json_type_lib_generic_lib_error_type_t type,
    const char* type_name, FILE* file)
{
    switch (type) {
    CASE(invalid_name):
        fputs("invalid library base name", file);
        break;
    CASE(type_name_not_available):
        fprintf(file, "type name '%s' not available", type_name);
        break;
    CASE(type_name_not_specified):
        fprintf(file, "type name not specified");
        break;
    CASE(type_def_not_found):
        fprintf(file, "type def '%s' not found", type_name);
        break;
    default:
        UNEXPECT_VAR("%d", type);
    }
}

#undef  CASE
#define CASE(n) case json_type_lib_shared_lib_error_ ## n

static const char* json_type_lib_shared_lib_error_type_get_desc(
    enum json_type_lib_shared_lib_error_type_t type)
{
    switch (type) {
    CASE(invalid_name):
        return "invalid library base name";
    CASE(load_lib_failed):
        return "failed loading library";
    CASE(get_lib_version_sym_not_found):
        return "'get_lib_version' symbol not found";
    CASE(get_type_def_sym_not_found):
        return "'get_type_def' symbol not found";
    CASE(wrong_lib_version):
        return "wrong library version";
    CASE(def_is_null):
        return "type def is NULL";
    CASE(def_not_valid):
        return "type def is not valid";
    default:
        UNEXPECT_VAR("%d", type);
    }
}

#define JSON_TYPE_TYPEOF_IS_(q, p, n) \
    TYPEOF_IS(p, q struct json_type_ ## n ## _node_t*)

#define JSON_TYPE_TO_NODE_(q, p, n)            \
    ({                                         \
        q struct json_type_node_t* __r;        \
        STATIC(JSON_TYPE_TYPEOF_IS_(q, p, n)); \
        __r = (q struct json_type_node_t*)     \
            ((char*)(p) - offsetof(            \
                struct json_type_node_t,       \
                node.n));                      \
        ASSERT(JSON_TYPE_NODE_IS_(q, __r, n)); \
        __r;                                   \
    })

#define JSON_TYPE_TYPEOF_IS(p, n)       JSON_TYPE_TYPEOF_IS_(, p, n)
#define JSON_TYPE_TYPEOF_IS_CONST(p, n) JSON_TYPE_TYPEOF_IS_(const, p, n)

#define JSON_TYPE_TO_NODE(p, n)         JSON_TYPE_TO_NODE_(, p, n)
#define JSON_TYPE_TO_NODE_CONST(p, n)   JSON_TYPE_TO_NODE_(const, p, n)

#define JSON_TYPE_TYPEOF_IS_ANY_NODE_(q, p) \
    JSON_TYPE_TYPEOF_IS_(q, p, any)
#define JSON_TYPE_TYPEOF_IS_ANY_NODE(p) \
    JSON_TYPE_TYPEOF_IS_ANY_NODE_(, p)
#define JSON_TYPE_TYPEOF_IS_ANY_NODE_CONST(p) \
    JSON_TYPE_TYPEOF_IS_ANY_NODE_(const, p)

#define JSON_TYPE_ANY_NODE_IS_(q, p, n)              \
    (                                                \
        STATIC(JSON_TYPE_TYPEOF_IS_ANY_NODE_(q, p)), \
        (p)->type == json_type_any_ ## n ## _type    \
    )
#define JSON_TYPE_ANY_NODE_IS(p, n) \
    JSON_TYPE_ANY_NODE_IS_(, p, n)
#define JSON_TYPE_ANY_NODE_IS_CONST(p, n) \
    JSON_TYPE_ANY_NODE_IS_(const, p, n)

#define JSON_TYPE_NODE_IS_ANY_(q, p, n)            \
    (                                              \
        JSON_TYPE_NODE_IS_(q, p, any) &&           \
        JSON_TYPE_ANY_NODE_IS_(q, &p->node.any, n) \
    )
#define JSON_TYPE_NODE_IS_ANY(p, n) \
    JSON_TYPE_NODE_IS_ANY_(, p, n)
#define JSON_TYPE_NODE_IS_ANY_CONST(p, n) \
    JSON_TYPE_NODE_IS_ANY_(const, p, n)

#define JSON_TYPE_TYPEOF_IS_ARRAY_NODE_(q, p) \
    JSON_TYPE_TYPEOF_IS_(q, p, array)
#define JSON_TYPE_TYPEOF_IS_ARRAY_NODE(p) \
    JSON_TYPE_TYPEOF_IS_ARRAY_NODE_(, p)
#define JSON_TYPE_TYPEOF_IS_ARRAY_NODE_CONST(p) \
    JSON_TYPE_TYPEOF_IS_ARRAY_NODE_(const, p)

#define JSON_TYPE_ARRAY_NODE_IS_(q, p, n)                \
    (                                                    \
        STATIC(JSON_TYPE_TYPEOF_IS_ARRAY_NODE_(q, p)),   \
        (p)->type == json_type_ ## n ## _array_node_type \
    )
#define JSON_TYPE_ARRAY_NODE_IS(p, n) \
    JSON_TYPE_ARRAY_NODE_IS_(, p, n)
#define JSON_TYPE_ARRAY_NODE_IS_CONST(p, n) \
    JSON_TYPE_ARRAY_NODE_IS_(const, p, n)

#define JSON_TYPE_ARRAY_NODE_AS_(q, p, n)          \
    ({                                             \
        ASSERT(JSON_TYPE_ARRAY_NODE_IS_(q, p, n)); \
        &((p)->val.n);                             \
    })
#define JSON_TYPE_ARRAY_NODE_AS(p, n) \
    JSON_TYPE_ARRAY_NODE_AS_(, p, n)
#define JSON_TYPE_ARRAY_NODE_AS_CONST(p, n) \
    JSON_TYPE_ARRAY_NODE_AS_(const, p, n)

#define JSON_TYPE_ARRAY_NODE_AS_IF_(q, p, n) \
    (                                        \
        JSON_TYPE_ARRAY_NODE_IS_(q, p, n)    \
        ? &((p)->val.n) : NULL               \
    )
#define JSON_TYPE_ARRAY_NODE_AS_IF(p, n) \
    JSON_TYPE_ARRAY_NODE_AS_IF_(, p, n)
#define JSON_TYPE_ARRAY_NODE_AS_IF_CONST(p, n) \
    JSON_TYPE_ARRAY_NODE_AS_IF_(const, p, n)

#define JSON_TYPE_TYPEOF_IS_NODE_(q, p) \
    TYPEOF_IS(p, q struct json_type_node_t*)

#define JSON_TYPE_TYPEOF_IS_NODE(p) \
    JSON_TYPE_TYPEOF_IS_NODE_(, p)

#define JSON_TYPE_TYPEOF_IS_NODE_CONST(p) \
    JSON_TYPE_TYPEOF_IS_NODE_(const, p)

#define JSON_TYPE_NODE_CONST_CAST(p) \
    CONST_CAST(p, struct json_type_node_t)

#define JSON_TYPE_NODE_IS_(q, p, n)                \
    (                                              \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_(q, p)),   \
        (p)->type == json_type_ ## n ## _node_type \
    )
#define JSON_TYPE_NODE_IS(p, n) \
    JSON_TYPE_NODE_IS_(, p, n)
#define JSON_TYPE_NODE_IS_CONST(p, n) \
    JSON_TYPE_NODE_IS_(const, p, n)

#define JSON_TYPE_NODE_AS_(q, p, n)          \
    ({                                       \
        ASSERT(JSON_TYPE_NODE_IS_(q, p, n)); \
        &((p)->node.n);                      \
    })
#define JSON_TYPE_NODE_AS(p, n) \
    JSON_TYPE_NODE_AS_(, p, n)
#define JSON_TYPE_NODE_AS_CONST(p, n) \
    JSON_TYPE_NODE_AS_(const, p, n)

#define JSON_TYPE_NODE_AS_IF_(q, p, n) \
    (                                  \
        JSON_TYPE_NODE_IS_(q, p, n)    \
        ? &((p)->node.n) : NULL        \
    )
#define JSON_TYPE_NODE_AS_IF(p, n) \
    JSON_TYPE_NODE_AS_IF_(, p, n)
#define JSON_TYPE_NODE_AS_IF_CONST(p, n) \
    JSON_TYPE_NODE_AS_IF_(const, p, n)

#undef  CASE
#define CASE(n) [json_type_any_ ## n ## _type] = #n

static const char* const json_type_any_node_type_names[] = {
    CASE(type),
    CASE(null),
    CASE(boolean),
    CASE(number),
    CASE(string),
    CASE(object),
    CASE(array),
};

#undef  CASE
#define CASE(n) [json_type_plain_ ## n ## _type] = #n

static const char* const json_type_plain_node_type_names[] = {
    CASE(null),
    CASE(boolean),
    CASE(number),
    CASE(string)
};

#undef  CASE
#define CASE(n) [json_type_object_sym_ ## n ## _type] = #n

static const char* const json_type_object_sym_names[] = {
    CASE(null),
    CASE(name),
    CASE(node)
};

#ifdef JSON_DEBUG

#undef  CASE
#define CASE(n) [json_type_ ## n ## _node_type] = #n

static const char* const json_type_node_type_names[] = {
    CASE(any),
    CASE(plain),
    CASE(object),
    CASE(array),
    CASE(list),
};

static void json_type_node_print_debug(
    const struct json_type_node_t*, FILE*);

static void json_type_string_len_print_debug(
    const uchar_t* str, size_t len, bool quotes, FILE* file)
{
    pretty_print_string(file, str, len,
        quotes ? pretty_print_string_quotes : 0);
}

static void json_type_string_print_debug(
    const uchar_t* str, FILE* file)
{
    json_type_string_len_print_debug(
        str, strulen(str), true, file);
}

static void json_type_any_node_print_debug(
    const struct json_type_any_node_t* node, FILE* file)
{
    fprintf(file, "{.type=%s}",
        ARRAY_NON_NULL_ELEM(
            json_type_any_node_type_names,
            node->type));
}

static void json_type_plain_node_print_debug(
    const struct json_type_plain_node_t* node, FILE* file)
{
    fprintf(file, "{.type=%s",
        ARRAY_NON_NULL_ELEM(
            json_type_plain_node_type_names,
            node->type));

    switch (node->type) {
    case json_type_plain_null_type:
        break;
    case json_type_plain_boolean_type:
        fprintf(file, " .val.boolean=%d",
            node->val.boolean);
        break;
    case json_type_plain_number_type:
    case json_type_plain_string_type: {
        const uchar_t* s;
        fprintf(file, " .val.%s=",
            node->type == json_type_plain_number_type
            ? "number" : "string");
        s = node->type == json_type_plain_number_type
            ? node->val.number
            : node->val.string;
        json_type_string_print_debug(s, file);
        break;
    }
    default:
        UNEXPECT_VAR("%d", node->type);
    }

    fputc('}', file);
}

static void json_type_object_node_print_debug(
    const struct json_type_object_node_t* node, FILE* file)
{
    const struct json_type_object_node_arg_t *p, *e;

    fputs("{.args=[", file);

    for (p = node->args,
         e = p + node->size;
         p < e;
         p ++ ) {
        fputs("{.name=", file);
        json_type_string_print_debug(p->name, file);
        fputs(" .type=", file);
        json_type_node_print_debug(p->type, file);
        fputc('}', file);
        if (p < e - 1)
            fputc(' ', file);
    }

    fprintf(file, "] .size=%zu}", node->size);
}

static void json_type_object_node_null_print_debug(
    const struct json_type_object_node_t* node, FILE* file)
{
    if (node != NULL)
        json_type_object_node_print_debug(node, file);
    else
        fputs("(nil)", file);
}

static void json_type_open_array_node_print_debug(
    const struct json_type_open_array_node_t* node, FILE* file)
{
    fputs("{.arg=", file);
    json_type_node_print_debug(node->arg, file);
    fputc('}', file);
}

static void json_type_args_print_debug(
    const struct json_type_node_t** args, size_t size, FILE* file)
{
    const struct json_type_node_t **p, **e;

    fputs("{.args=[", file);

    for (p = args,
         e = p + size;
         p < e;
         p ++ ) {
        json_type_node_print_debug(*p, file);
        if (p < e - 1)
            fputc(' ', file);
    }

    fprintf(file, "] .size=%zu}", size);
}

static void json_type_closed_array_node_print_debug(
    const struct json_type_closed_array_node_t* node, FILE* file)
{
    json_type_args_print_debug(node->args, node->size, file);
}

static void json_type_array_node_print_debug(
    const struct json_type_array_node_t* node, FILE* file)
{
#undef  CASE
#define CASE(n) [json_type_ ## n ## _array_node_type] = #n

    static const char* const names[] = {
        CASE(open),
        CASE(closed)
    };

    fprintf(file, "{.type=%s",
        ARRAY_NON_NULL_ELEM(names, node->type));

#undef  CASE
#define CASE(n)                                     \
    case json_type_ ## n ## _array_node_type:       \
        fputs(" .val=", file);                      \
        json_type_ ## n ## _array_node_print_debug( \
            JSON_TYPE_ARRAY_NODE_AS_CONST(node, n), \
            file);                                  \
        break

    switch (node->type) {
    CASE(open);
    CASE(closed);
    default:
        UNEXPECT_VAR(
            "%d", node->type);
    }

    fputc('}', file);
}

static void json_type_array_node_null_print_debug(
    const struct json_type_array_node_t* node, FILE* file)
{
    if (node != NULL)
        json_type_array_node_print_debug(node, file);
    else
        fputs("(nil)", file);
}

static void json_type_list_node_print_debug(
    const struct json_type_list_node_t* node, FILE* file)
{
    json_type_args_print_debug(node->args, node->size, file);
}

static void json_type_list_attr_null_print_debug(
    const struct json_type_list_attr_t*, FILE*);

static void json_type_node_print_debug(
    const struct json_type_node_t* node, FILE* file)
{
    fprintf(file, "{.type=%s",
        ARRAY_NON_NULL_ELEM(
            json_type_node_type_names,
            node->type));

#undef  CASE
#define CASE(n)                                      \
    case json_type_ ## n ## _node_type:              \
        fputs(" .node=", file);                      \
        json_type_ ## n ## _node_print_debug(        \
            JSON_TYPE_NODE_AS_CONST(node, n), file); \
        break

    switch (node->type) {
    CASE(any);
    CASE(plain);
    CASE(object);
    CASE(array);
    CASE(list);
    default:
        UNEXPECT_VAR(
            "%d", node->type);
    }

    if (JSON_TYPE_NODE_IS_CONST(node, list)) {
        fputs(" .attr=", file);
        json_type_list_attr_null_print_debug(
            node->attr.list, file);
    }

    fputc('}', file);
}

static void json_type_node_null_print_debug(
    const struct json_type_node_t* node, FILE* file)
{
    if (node != NULL)
        json_type_node_print_debug(node, file);
    else
        fputs("(nil)", file);
}

static void json_type_object_sym_print_debug(
    const struct json_type_object_sym_t* sym, FILE* file)
{
    fprintf(file, "{.type=%s",
        ARRAY_NON_NULL_ELEM(
            json_type_object_sym_names,
            sym->type));

    switch (sym->type) {
    case json_type_object_sym_null_type:
        // stev: nop
        break;
    case json_type_object_sym_name_type:
        fputs(" .val.name=", file);
        json_type_string_print_debug(sym->val.name, file);
        break;
    case json_type_object_sym_node_type:
        fputs(" .val.node=", file);
        json_type_node_print_debug(sym->val.node, file);
        break;
    default:
        UNEXPECT_VAR("%d", sym->type);
    }

    fputc('}', file);
}

#endif

#define JSON_TYPE_AST_IS(n)    JSON_AST_NODE_IS(node, n)
#define JSON_TYPE_AST_AS(n)    JSON_AST_NODE_AS(node, n)
#define JSON_TYPE_AST_AS_IF(n) JSON_AST_NODE_AS_IF(node, n)

static void json_type_print_repr(const uchar_t* str, bool quotes,
    FILE* file)
{
    pretty_print_repr(file, str, strulen(str),
        quotes ? pretty_print_repr_print_quotes : 0);
}

union json_type_node_pack_t
{
    const struct json_type_any_node_t*    any;
    const struct json_type_plain_node_t*  plain;
    const struct json_type_object_node_t* object;
    const struct json_type_array_node_t*  array;
    const struct json_type_list_node_t*   list;
};

union json_type_array_node_pack_t
{
    const struct json_type_open_array_node_t*   open;
    const struct json_type_closed_array_node_t* closed;
};

static void json_type_list_attr_print(
    const struct json_type_list_attr_t*, FILE*);

static void json_type_print(const struct json_type_node_t* node,
    FILE* file, bool attr)
{
    union json_type_node_pack_t n;

    if ((n.any = JSON_TYPE_NODE_AS_IF_CONST(node, any))) {
        fprintf(file, "\"%s\"", ARRAY_NON_NULL_ELEM(
            json_type_any_node_type_names, n.any->type));
    }
    else
    if ((n.plain = JSON_TYPE_NODE_AS_IF_CONST(node, plain))) {
        fputs("{\"plain\":", file);

        switch (n.plain->type) {

        case json_type_plain_null_type:
            fputs("null", file);
            break;

        case json_type_plain_boolean_type:
            fputs(n.plain->val.boolean ? "true" : "false", file);
            break;

        case json_type_plain_number_type:
            json_type_print_repr(n.plain->val.number, false, file);
            break;

        case json_type_plain_string_type:
            json_type_print_repr(n.plain->val.string, true, file);
            break;

        default:
            UNEXPECT_VAR("%d", n.plain->type);
        }

        fputc('}', file);
    }
    else
    if ((n.object = JSON_TYPE_NODE_AS_IF_CONST(node, object))) {
        const struct json_type_object_node_arg_t *p, *e;

        fputs("{\"type\":\"object\",\"args\":[", file);
        for (p = n.object->args,
             e = p + n.object->size;
             p < e;
             p ++) {
            fputs("{\"name\":", file);
            json_type_print_repr(p->name, true, file);
            fputs(",\"type\":", file);
            json_type_print(p->type, file, attr);
            fputc('}', file);
            if (p < e - 1)
                fputc(',', file);
        }
        fputs("]}", file);
    }
    else
    if ((n.array = JSON_TYPE_NODE_AS_IF_CONST(node, array))) {
        fputs("{\"type\":\"array\",\"args\":", file);

        switch (n.array->type) {

        case json_type_open_array_node_type:
            json_type_print(n.array->val.open.arg, file, attr);
            break;

        case json_type_closed_array_node_type: {
            const struct json_type_node_t **p, **e;

            fputc('[', file);
            for (p = n.array->val.closed.args,
                 e = p + n.array->val.closed.size;
                 p < e;
                 p ++) {
                json_type_print(*p, file, attr);
                if (p < e - 1)
                    fputc(',', file);
            }
            fputc(']', file);
            break;
        }

        default:
            UNEXPECT_VAR("%d", n.array->type);
        }

        fputc('}', file);
    }
    else
    if ((n.list = JSON_TYPE_NODE_AS_IF_CONST(node, list))) {
        const struct json_type_node_t **p, **e;

        fputs("{\"type\":\"list\",\"args\":[", file);
        for (p = n.list->args,
             e = p + n.list->size;
             p < e;
             p ++) {
            json_type_print(*p, file, attr);
            if (p < e - 1)
                fputc(',', file);
        }
        fputc(']', file);
        if (attr) {
            fputs(",\"attr\":", file);
            json_type_list_attr_print(node->attr.list, file);
        }
        fputc('}', file);
    }
    else
        UNEXPECT_VAR("%d", node->type);
}

static void json_type_node_print(
    const struct json_type_node_t* node,
    FILE* file)
{
    json_type_print(node, file, false);
}

static void json_type_node_print_complete(
    const struct json_type_node_t* node,
    FILE* file)
{
    fputc('`', file);
    json_type_node_print(node, file);
    fputc('\'', file);
}

static void json_type_list_attr_print_base(
    const struct json_type_list_attr_t* attr,
    FILE* file);

static void json_type_node_print_base(
    const struct json_type_node_t* node,
    FILE* file)
{
    union json_type_node_pack_t p;

    if (JSON_TYPE_NODE_IS_CONST(node, any) ||
        JSON_TYPE_NODE_IS_CONST(node, plain))
        json_type_node_print_complete(node, file);
    else
    if ((p.object = JSON_TYPE_NODE_AS_IF_CONST(node, object))) {
        fputs("`{\"type\":\"object\",\"args\":[", file);
        if (p.object->size) fputs("...", file);
        fputs("]}'", file);
    }
    else
    if ((p.array = JSON_TYPE_NODE_AS_IF_CONST(node, array))) {
        const struct json_type_closed_array_node_t* c;

        c = JSON_TYPE_ARRAY_NODE_AS_IF_CONST(p.array, closed);

        fputs("`{\"type\":\"array\",\"args\":", file);
        if (c) fputc('[', file);
        if (!c || c->size) fputs("...", file);
        if (c) fputc(']', file);
        fputs("}'", file);
    }
    else
    if ((p.list = JSON_TYPE_NODE_AS_IF_CONST(node, list))) {
        if (p.list->size)
            json_type_list_attr_print_base(node->attr.list, file);
        else
            fputs("none", file);
    }
    else
        UNEXPECT_VAR("%d", node->type);
}

#if 0
static void json_type_node_null_print(
    const struct json_type_node_t* node,
    FILE* file)
{
    if (node != NULL)
        json_type_print(node, file, false);
    else
        fputs("(nil)", file);
}
#endif

static void json_type_def_print(const struct json_type_def_t* def,
    FILE* file, bool attr)
{
    if (def->type == json_type_def_node_type)
        json_type_print(def->val.node, file, attr);
    else
    if (def->type == json_type_def_defs_type) {
        const struct json_type_defs_arg_t *p, *e;

        fputc('[', file);
        for (p = def->val.defs->args,
             e = p + def->val.defs->size;
             p < e;
             p ++) {
            fputs("{\"name\":", file);
            json_type_print_repr(p->name, true, file);
            fputs(",\"type\":", file);
            json_type_print(p->type, file, attr);
            fputc('}', file);
            if (p < e - 1)
                fputc(',', file);
        }
        fputc(']', file);
    }
    else
        UNEXPECT_VAR("%d", def->type);
}

struct json_type_attrs_t
{
    size_t ptr_count;
    bool   has_cells;
};

static size_t json_type_list_attr_get_ptr_count(
    const struct json_type_list_attr_t* list);

static struct json_type_attrs_t json_type_node_get_attrs(
    const struct json_type_node_t* node)
{
    union json_type_node_pack_t n;
    struct json_type_attrs_t r = {
        .ptr_count = 1,
        .has_cells = false
    };

    if (JSON_TYPE_NODE_IS_CONST(node, any) ||
        JSON_TYPE_NODE_IS_CONST(node, plain))
        ; // stev: nop
    else
    if ((n.object = JSON_TYPE_NODE_AS_IF_CONST(node, object))) {
        const struct json_type_object_node_arg_t *p, *e;

        for (p = n.object->args,
             e = p + n.object->size;
             p < e;
             p ++) {
            struct json_type_attrs_t a;

            a = json_type_node_get_attrs(p->type);

            r.ptr_count += a.ptr_count;
            r.has_cells |= a.has_cells;
        }

        r.ptr_count ++;
    }
    else
    if ((n.array = JSON_TYPE_NODE_AS_IF_CONST(node, array))) {
        switch (n.array->type) {

        case json_type_open_array_node_type: {
            struct json_type_attrs_t a;

            a = json_type_node_get_attrs(
                    n.array->val.open.arg);

            r.ptr_count += a.ptr_count;
            r.has_cells |= a.has_cells;
            break;
        }

        case json_type_closed_array_node_type: {
            const struct json_type_node_t **p, **e;

            for (p = n.array->val.closed.args,
                 e = p + n.array->val.closed.size;
                 p < e;
                 p ++) {
                struct json_type_attrs_t a;

                a = json_type_node_get_attrs(*p);

                r.ptr_count += a.ptr_count;
                r.has_cells |= a.has_cells;
            }

            r.ptr_count ++;
            break;
        }

        default:
            UNEXPECT_VAR("%d", n.array->type);
        }
    }
    else
    if ((n.list = JSON_TYPE_NODE_AS_IF_CONST(node, list))) {
        const struct json_type_node_t **p, **e;
        size_t c;

        for (p = n.list->args,
             e = p + n.list->size;
             p < e;
             p ++) {
            struct json_type_attrs_t a;

            a = json_type_node_get_attrs(*p);

            r.ptr_count += a.ptr_count;
            r.has_cells |= a.has_cells;
        }
        r.ptr_count += c = json_type_list_attr_get_ptr_count(
            node->attr.list);

        r.has_cells |= c > 1;
        r.ptr_count ++;
    }
    else
        UNEXPECT_VAR("%d", node->type);

    return r;
}

static struct json_type_attrs_t json_type_defs_get_attrs(
    const struct json_type_defs_t* defs)
{
    const struct json_type_defs_arg_t *p, *e;
    struct json_type_attrs_t r = {
        .ptr_count = 2,
        .has_cells = false
    };

    for (p = defs->args,
         e = p + defs->size;
         p < e;
         p ++) {
        struct json_type_attrs_t a;

        a = json_type_node_get_attrs(p->type);

        r.ptr_count += a.ptr_count;
        r.has_cells |= a.has_cells;
    }

    return r;
}

static struct json_type_attrs_t json_type_def_get_attrs(
    const struct json_type_def_t* def)
{
    if (def->type == json_type_def_node_type)
        return json_type_node_get_attrs(def->val.node);
    else
    if (def->type == json_type_def_defs_type)
        return json_type_defs_get_attrs(def->val.defs);
    else
        UNEXPECT_VAR("%d", def->type);
}

struct json_type_ptr_space_t;

#define TREAP_NAME             json_type_ptr
#define TREAP_KEY_TYPE         const void*
#define TREAP_VAL_TYPE         size_t
#define TREAP_KEY_IS_EQ(x, y)  ((x) == (y))
#define TREAP_KEY_CMP(x, y) \
    (                       \
          (x) == (y)        \
        ? treap_key_cmp_eq  \
        : (x) < (y)         \
        ? treap_key_cmp_lt  \
        : treap_key_cmp_gt  \
    )
#define TREAP_KEY_ASSIGN(x, y) ((x) = (y)) 
#define TREAP_ALLOC_OBJ_TYPE struct json_type_ptr_space_t

#include "treap-impl.h"

struct json_type_ptr_space_t
{
    struct pool_alloc_t pool;
    struct json_type_ptr_treap_t map;
    size_t index;
};

static struct json_type_ptr_treap_node_t*
    json_type_ptr_space_new_treap_node(
        struct json_type_ptr_space_t* space)
{
    struct json_type_ptr_treap_node_t* n;

    if ((n = pool_alloc_allocate(&space->pool,
        sizeof(struct json_type_ptr_treap_node_t),
        MEM_ALIGNOF(struct json_type_ptr_treap_node_t))) == NULL)
        fatal_error("ptr-space pool alloc failed");

    memset(n, 0, sizeof(*n));

    return n;
}

static void json_type_ptr_space_init(
    struct json_type_ptr_space_t* space,
    size_t size)
{
    const size_t n = sizeof(
        struct json_type_ptr_treap_node_t);

    memset(space, 0, sizeof(
        struct json_type_ptr_space_t));

    ASSERT_SIZE_MUL_NO_OVERFLOW(size, n);
    pool_alloc_init(&space->pool, size * n);

    json_type_ptr_treap_init(
        &space->map,
        json_type_ptr_space_new_treap_node,
        space);
}

static void json_type_ptr_space_done(
    struct json_type_ptr_space_t* space)
{
    pool_alloc_done(&space->pool);
}

#define JSON_TYPE_PTR_SPACE_VALIDATE(p)                \
    ({                                                 \
        struct json_type_ptr_treap_node_t* __r = NULL; \
        bool __b = json_type_ptr_treap_lookup_key(     \
            &space->map, p, &__r);                     \
        if (!__b || !__r)                              \
            *result = (p);                             \
        __b && __r;                                    \
    })
#define JSON_TYPE_PTR_SPACE_VALIDATE_NON_NULL(p) \
    (                                            \
        (p) == NULL ||                           \
        JSON_TYPE_PTR_SPACE_VALIDATE(p)          \
    )

#define JSON_TYPE_PTR_SPACE_VALIDATE_INSERT_(p)        \
    do {                                               \
        struct json_type_ptr_treap_node_t* __r = NULL; \
        bool __b = json_type_ptr_treap_insert_key(     \
            &space->map, p, &__r);                     \
        ASSERT(__r != NULL);                           \
        if (!__b) {                                    \
            *result = (p);                             \
            return false;                              \
        }                                              \
        __r->val = space->index ++;                    \
    } while (0)

#define JSON_TYPE_PTR_SPACE_INSERT \
    JSON_TYPE_PTR_SPACE_VALIDATE_INSERT_

static bool json_type_object_sym_validate(
    const struct json_type_object_sym_t* sym,
    struct json_type_ptr_space_t* space,
    const void** result)
{
    ASSERT(sym != NULL);

    if (sym->type == json_type_object_sym_null_type ||
        sym->type == json_type_object_sym_name_type)
        ; // stev: nop
    else
    if (sym->type == json_type_object_sym_node_type) {
        if (!JSON_TYPE_PTR_SPACE_VALIDATE(sym->val.node))
            return false;
    }
    else
        UNEXPECT_VAR("%d", sym->type);

    *result = NULL;
    return true;
}

static bool json_type_list_attr_validate(
    const struct json_type_list_attr_t*,
    struct json_type_ptr_space_t*,
    const void** result);

static bool json_type_node_validate(
    const struct json_type_node_t* node,
    struct json_type_ptr_space_t* space,
    const void** result)
{
    union json_type_node_pack_t n;

    ASSERT(node != NULL);

    JSON_TYPE_PTR_SPACE_INSERT(node);

    if (JSON_TYPE_NODE_IS_CONST(node, any) ||
        JSON_TYPE_NODE_IS_CONST(node, plain))
        ; // stev: nop
    else
    if ((n.object = JSON_TYPE_NODE_AS_IF_CONST(node, object))) {
        const struct json_type_object_node_arg_t *p, *e;

        if (n.object->args == NULL) {
            *result = &n.object->args;
            return false;
        }
        JSON_TYPE_PTR_SPACE_INSERT(n.object->args);

        for (p = n.object->args,
             e = p + n.object->size;
             p < e;
             p ++) {
            if (p->type == NULL) {
                *result = &p->type;
                return false;
            }
            if (!json_type_node_validate(
                    p->type, space, result))
                return false;
        }
    }
    else
    if ((n.array = JSON_TYPE_NODE_AS_IF_CONST(node, array))) {
        switch (n.array->type) {

        case json_type_open_array_node_type:
            if (n.array->val.open.arg == NULL) {
                *result = &n.array->val.open.arg;
                return false;
            }
            if (!json_type_node_validate(
                    n.array->val.open.arg,
                    space, result))
                return false;
            break;

        case json_type_closed_array_node_type: {
            const struct json_type_node_t **p, **e;

            if (n.array->val.closed.args == NULL) {
                *result = &n.array->val.closed.args;
                return false;
            }
            JSON_TYPE_PTR_SPACE_INSERT(n.array->val.closed.args);

            for (p = n.array->val.closed.args,
                 e = p + n.array->val.closed.size;
                 p < e;
                 p ++) {
                if (*p == NULL) {
                    *result = n.array->val.closed.args;
                    return false;
                }
                if (!json_type_node_validate(
                        *p, space, result))
                    return false;
            }
            break;
        }

        default:
            UNEXPECT_VAR("%d", n.array->type);
        }
    }
    else
    if ((n.list = JSON_TYPE_NODE_AS_IF_CONST(node, list))) {
        const struct json_type_node_t **p, **e;

        if (n.list->args == NULL) {
            *result = &n.list->args;
            return false;
        }
        JSON_TYPE_PTR_SPACE_INSERT(n.list->args);

        for (p = n.list->args,
             e = p + n.list->size;
             p < e;
             p ++) {
            if (*p == NULL) {
                *result = p;
                return false;
            }
            if (!json_type_node_validate(
                    *p, space, result))
                return false;
        }

        if (!json_type_list_attr_validate(
                node->attr.list,
                space, result))
            return false;
    }
    else
        UNEXPECT_VAR("%d", node->type);

    *result = NULL;
    return true;
}

struct json_type_args_dict_t;

#define JSON_TYPE_ARGS_DICT_TRIE_SYM_IS_NULL(s) \
    (                                           \
        STATIC(TYPEOF_IS(s, uchar_t)),          \
        (s) == 0                                \
    )
#define JSON_TYPE_ARGS_DICT_TRIE_SYM_CMP(x, y)  \
    (                                           \
        STATIC(TYPEOF_IS(x, uchar_t)),          \
        STATIC(TYPEOF_IS(y, uchar_t)),          \
          (x) == (y)                            \
        ? trie_sym_cmp_eq                       \
        : (x) < (y)                             \
        ? trie_sym_cmp_lt                       \
        : trie_sym_cmp_gt                       \
    )

#define TRIE_NAME           json_type_args_dict
#define TRIE_SYM_TYPE       uchar_t
#define TRIE_VAL_TYPE       const struct json_type_defs_arg_t*
#define TRIE_SYM_IS_NULL    JSON_TYPE_ARGS_DICT_TRIE_SYM_IS_NULL
#define TRIE_SYM_CMP        JSON_TYPE_ARGS_DICT_TRIE_SYM_CMP
#define TRIE_KEY_TYPE       const uchar_t*
#define TRIE_KEY_INC(k)     ((k) ++)
#define TRIE_KEY_DEREF(k)   ((*k))
#undef  TRIE_ALLOC_OBJ_TYPE
#define TRIE_ALLOC_OBJ_TYPE struct json_type_args_dict_t

#define TRIE_NEED_INSERT_KEY
#include "trie-impl.h"
#undef  TRIE_NEED_INSERT_KEY

#define JSON_TYPE_ARGS_DICT_TRIE_NODE_AS_VAL(p) \
    TRIE_NODE_AS_VAL(                           \
        json_type_args_dict, p,                 \
        JSON_TYPE_ARGS_DICT_TRIE_SYM_IS_NULL)
#define JSON_TYPE_ARGS_DICT_TRIE_NODE_AS_VAL_REF(p)    \
    ({                                                 \
        const struct json_type_defs_arg_t* const* __r; \
        __r = TRIE_NODE_AS_VAL_REF(                    \
            json_type_args_dict, p,                    \
            JSON_TYPE_ARGS_DICT_TRIE_SYM_IS_NULL);     \
        CONST_CAST(__r,                                \
            const struct json_type_defs_arg_t*);       \
    })

struct json_type_args_dict_t
{
    struct pool_alloc_t pool;
    struct json_type_args_dict_trie_t map;
};

static struct json_type_args_dict_trie_node_t*
    json_type_args_dict_new_trie_node(
        struct json_type_args_dict_t* dict)
{
    struct json_type_args_dict_trie_node_t* n;

    if ((n = pool_alloc_allocate(&dict->pool,
        sizeof(struct json_type_args_dict_trie_node_t),
        MEM_ALIGNOF(struct json_type_args_dict_trie_node_t))) == NULL)
        fatal_error("args-dict pool alloc failed");

    memset(n, 0, sizeof(*n));

    return n;
}

static void json_type_args_dict_init(
    struct json_type_args_dict_t* dict,
    size_t size)
{
    const size_t n =
        sizeof(struct json_type_args_dict_trie_node_t);

    ASSERT(size > 0);

    memset(dict, 0, sizeof(struct json_type_args_dict_t));

    ASSERT_SIZE_MUL_NO_OVERFLOW(size, n);
    pool_alloc_init(&dict->pool, size * n);

    json_type_args_dict_trie_init(
        &dict->map, json_type_args_dict_new_trie_node, dict);
}

static void json_type_args_dict_done(
    struct json_type_args_dict_t* dict)
{
    pool_alloc_done(&dict->pool);
}

static bool json_type_defs_check_dup_keys(
    const struct json_type_defs_t* defs,
    const struct json_type_defs_arg_t** arg1,
    const struct json_type_defs_arg_t** arg2)
{
    const struct json_type_defs_arg_t *p, *e;
    struct json_type_args_dict_t d;
    bool r = true;
    size_t n = 0;

    *arg1 = NULL;
    *arg2 = NULL;

    for (p = defs->args,
         e = p + defs->size;
         p < e;
         p ++) {
        size_t l = strulen(p->name);

        ASSERT_SIZE_INC_NO_OVERFLOW(l);
        l ++;

        ASSERT_SIZE_ADD_NO_OVERFLOW(n, l);
        n += l;
    }

    // stev: assume 'n' to be an upper bound for
    // the number of nodes in the trie 'd' -- which
    // has to contain all the type names in 'defs'
    json_type_args_dict_init(&d, n);

    for (p = defs->args,
         e = p + defs->size;
         p < e;
         p ++) {
        const struct json_type_args_dict_trie_node_t* n = NULL;

        if (!json_type_args_dict_trie_insert_key(&d.map, p->name, &n)) {
            *arg2 = JSON_TYPE_ARGS_DICT_TRIE_NODE_AS_VAL(n);
            *arg1 = p;
            r = false;
            break;
        }

        *JSON_TYPE_ARGS_DICT_TRIE_NODE_AS_VAL_REF(n) = p;
    }

    json_type_args_dict_done(&d);

    return r;
}

static bool json_type_defs_validate(
    const struct json_type_defs_t* defs,
    struct json_type_ptr_space_t* space,
    const void** result)
{
    const struct json_type_defs_arg_t *p, *e;
    const struct json_type_defs_arg_t *a, *b;

    ASSERT(defs != NULL);

    JSON_TYPE_PTR_SPACE_INSERT(defs);

    if (defs->args == NULL) {
        *result = &defs->args;
        return false;
    }
    JSON_TYPE_PTR_SPACE_INSERT(defs->args);

    for (p = defs->args,
         e = p + defs->size;
         p < e;
         p ++) {
        if (p->type == NULL) {
            *result = &p->type;
            return false;
        }
        if (!json_type_node_validate(
                p->type, space, result))
            return false;
    }

    a = b = NULL;
    if (!json_type_defs_check_dup_keys(defs, &a, &b)) {
        ASSERT(a != NULL);
        ASSERT(b != NULL);
        *result = a;
        return false;
    }

    *result = NULL;
    return true;
}

#define JSON_TYPE_DEF_VALIDATE(n)       \
    ({                                  \
        if (def->val.n == NULL) {       \
            *result = &def->val.n;      \
            return false;               \
        }                               \
        json_type_ ## n ## _validate(   \
            def->val.n, space, result); \
    })

static bool json_type_def_validate(
    const struct json_type_def_t* def,
    struct json_type_ptr_space_t* space,
    const void** result)
{
    if (def->type == json_type_def_node_type)
        return JSON_TYPE_DEF_VALIDATE(node);
    else
    if (def->type == json_type_def_defs_type)
        return JSON_TYPE_DEF_VALIDATE(defs);
    else
        UNEXPECT_VAR("%d", def->type);
}

#define JSON_TYPE_PTR_SPACE_LOOKUP(p)                  \
    ({                                                 \
        struct json_type_ptr_treap_node_t* __r = NULL; \
        bool __b = json_type_ptr_treap_lookup_key(     \
            &space->map, p, &__r);                     \
        ASSERT(__b);                                   \
        ASSERT(__r);                                   \
        __r;                                           \
    })

#define JSON_TYPE_PTR_SPACE_GEN_DEF_INSERT_(p)         \
    ({                                                 \
        struct json_type_ptr_treap_node_t* __r = NULL; \
        bool __b = json_type_ptr_treap_insert_key(     \
            &space->map, p, &__r);                     \
        ASSERT(__b);                                   \
        ASSERT(__r);                                   \
        __r->val = space->index ++;                    \
        __r;                                           \
    })

#undef  JSON_TYPE_PTR_SPACE_INSERT
#define JSON_TYPE_PTR_SPACE_INSERT \
    JSON_TYPE_PTR_SPACE_GEN_DEF_INSERT_

#undef  CASE
#define CASE(n) \
    [json_type_object_sym_ ## n ## _type] = #n

static void json_type_object_sym_gen_def(
    const struct json_type_object_sym_t* sym,
    struct json_type_ptr_space_t* space,
    FILE* file)
{
    static const char* const types[] = {
        CASE(null),
        CASE(name),
        CASE(node)
    };

    const struct json_type_ptr_treap_node_t* t;
    const char* s;

    s = ARRAY_CHAR_NULL_ELEM(types, sym->type);

    fprintf(file,
        "{\n"
        "        .type = json_type_object_sym_%s_type",
        s);

    if (sym->type == json_type_object_sym_null_type)
        ; // stev: nop
    else
    if (sym->type == json_type_object_sym_name_type) {
        fprintf(file,
            ",\n"
            "        .val.%s = (const uchar_t*) ",
            s);

        json_type_print_repr(sym->val.name, true, file);
    }
    else
    if (sym->type == json_type_object_sym_node_type) {
        t = JSON_TYPE_PTR_SPACE_LOOKUP(sym->val.node);

        fprintf(file,
            ",\n"
            "        .val.%s = &__%zu",
            s, t->val);
    }
    else
        UNEXPECT_VAR("%d", sym->type);

    fputs(
        "\n    }",
        file);
}

static void json_type_list_attr_gen_def(
    const struct json_type_list_attr_t*,
    struct json_type_ptr_space_t*,
    FILE*);

// stev: we're quite picky about the names produced by
// '*_gen_def' functions: instead of using the 'treap'
// data structure for to obtain serial numbers naming
// our objects, we could have simply used the integer
// representation of the pointers defining the objects
// themselves; however, using the 'treap' structure as
// `pointer space', we obtain *normalized* type defs
// on output: successive executions of the program --
// and thus of the library -- produce identical output
// when provided with the same input!

static void json_type_node_gen_def(
    const struct json_type_node_t* node,
    struct json_type_ptr_space_t* space,
    FILE* file)
{
    const struct json_type_ptr_treap_node_t *t, *u, *v;
    union json_type_node_pack_t n;
    const char* s;

    if ((n.any = JSON_TYPE_NODE_AS_IF_CONST(node, any))) {
        t = JSON_TYPE_PTR_SPACE_INSERT(node);
        s = ARRAY_CHAR_NULL_ELEM(
                json_type_any_node_type_names,
                n.any->type);
        fprintf(file,
            "static const struct json_type_node_t __%zu = {\n"
            "    .type = json_type_any_node_type,\n"
            "    .node.any = {\n"
            "        .type = json_type_any_%s_type\n"
            "    }\n"
            "};\n\n",
            t->val, s);
    }
    else
    if ((n.plain = JSON_TYPE_NODE_AS_IF_CONST(node, plain))) {
        t = JSON_TYPE_PTR_SPACE_INSERT(node);
        s = ARRAY_CHAR_NULL_ELEM(
                json_type_plain_node_type_names,
                n.plain->type);
        fprintf(file,
            "static const struct json_type_node_t __%zu = {\n"
            "    .type = json_type_plain_node_type,\n"
            "    .node.plain = {\n"
            "        .type = json_type_plain_%s_type,",
            t->val, s);
        if (n.plain->type != json_type_plain_null_type)
            fprintf(file,
                "\n"
                "        .val.%s = ", s);
        if (n.plain->type == json_type_plain_number_type ||
            n.plain->type == json_type_plain_string_type)
            fputs("(const uchar_t*) ", file);

        switch (n.plain->type) {

        case json_type_plain_null_type:
            // stev: nop
            break;

        case json_type_plain_boolean_type:
            fputs(n.plain->val.boolean ? "true" : "false", file);
            break;

        case json_type_plain_number_type:
            json_type_print_repr(n.plain->val.number, true, file);
            break;

        case json_type_plain_string_type:
            json_type_print_repr(n.plain->val.string, true, file);
            break;

        default:
            UNEXPECT_VAR("%d", n.plain->type);
        }
        fputs(
            "\n"
            "    }\n"
            "};\n\n",
            file);
    }
    else
    if ((n.object = JSON_TYPE_NODE_AS_IF_CONST(node, object))) {
        const struct json_type_object_node_arg_t *p, *e;

        for (p = n.object->args,
             e = p + n.object->size;
             p < e;
             p ++) {
            json_type_node_gen_def(p->type, space, file);
        }

        u = JSON_TYPE_PTR_SPACE_INSERT(n.object->args);

        fprintf(file,
            "static const struct json_type_object_node_arg_t "
                "__%zu[] = {\n",
            u->val);

        for (p = n.object->args,
             e = p + n.object->size;
             p < e;
             p ++) {
            fputs(
                "    {\n"
                "        .name = (const uchar_t*) ",
                file);

            json_type_print_repr(p->name, true, file);

            t = JSON_TYPE_PTR_SPACE_LOOKUP(p->type);
            fprintf(file,
                ",\n"
                "        .type = &__%zu\n"
                "    }%s\n",
                t->val, p < e - 1 ? "," : "");
        }
        fputs("};\n\n", file);

        t = JSON_TYPE_PTR_SPACE_INSERT(node);

        fprintf(file,
            "static const struct json_type_node_t __%zu = {\n"
            "    .type = json_type_object_node_type,\n"
            "    .node.object = {\n"
            "        .args = __%zu,\n"
            "        .size = %zu\n"
            "    }\n"
            "};\n\n",
            t->val, u->val, n.object->size);
    }
    else
    if ((n.array = JSON_TYPE_NODE_AS_IF_CONST(node, array))) {
        switch (n.array->type) {

        case json_type_open_array_node_type:
            json_type_node_gen_def(n.array->val.open.arg, space, file);

            u = JSON_TYPE_PTR_SPACE_LOOKUP(n.array->val.open.arg);
            t = JSON_TYPE_PTR_SPACE_INSERT(node);

            fprintf(file,
                "static const struct json_type_node_t __%zu = {\n"
                "    .type = json_type_array_node_type,\n"
                "    .node.array = {\n"
                "        .type = json_type_open_array_node_type,\n"
                "        .val.open.arg = &__%zu\n"
                "    }\n"
                "};\n\n",
                t->val, u->val);
            break;

        case json_type_closed_array_node_type: {
            const struct json_type_node_t **p, **e;

            for (p = n.array->val.closed.args,
                 e = p + n.array->val.closed.size;
                 p < e;
                 p ++) {
                json_type_node_gen_def(*p, space, file);
            }

            u = JSON_TYPE_PTR_SPACE_INSERT(n.array->val.closed.args);
            fprintf(file,
                "static const struct json_type_node_t* __%zu[] = {\n",
                u->val);

            for (p = n.array->val.closed.args,
                 e = p + n.array->val.closed.size;
                 p < e;
                 p ++) {
                t = JSON_TYPE_PTR_SPACE_LOOKUP(*p);
                fprintf(file,
                    "    &__%zu,\n",
                    t->val);
            }
            fputs(
                "    NULL\n"
                "};\n\n",
                file);

            t = JSON_TYPE_PTR_SPACE_INSERT(node);

            fprintf(file,
                "static const struct json_type_node_t __%zu = {\n"
                "    .type = json_type_array_node_type,\n"
                "    .node.array = {\n"
                "        .type = json_type_closed_array_node_type,\n"
                "        .val.closed = {\n"
                "            .args = __%zu,\n"
                "            .size = %zu\n"
                "        }\n"
                "    }\n"
                "};\n\n",
                t->val, u->val, n.array->val.closed.size);
            break;
        }

        default:
            UNEXPECT_VAR("%d", n.array->type);
        }
    }
    else
    if ((n.list = JSON_TYPE_NODE_AS_IF_CONST(node, list))) {
        const struct json_type_node_t **p, **e;

        for (p = n.list->args,
             e = p + n.list->size;
             p < e;
             p ++) {
            json_type_node_gen_def(*p, space, file);
        }

        json_type_list_attr_gen_def(
            node->attr.list, space, file);

        u = JSON_TYPE_PTR_SPACE_INSERT(n.list->args);

        fprintf(file,
            "static const struct json_type_node_t* __%zu[] = {\n",
            u->val);

        for (p = n.list->args,
             e = p + n.list->size;
             p < e;
             p ++) {
            t = JSON_TYPE_PTR_SPACE_LOOKUP(*p);
            fprintf(file,
                "    &__%zu,\n",
                t->val);
        }
        fputs(
            "    NULL\n"
            "};\n\n",
            file);

        t = JSON_TYPE_PTR_SPACE_INSERT(node);
        v = JSON_TYPE_PTR_SPACE_LOOKUP(node->attr.list);

        fprintf(file,
            "static const struct json_type_node_t __%zu = {\n"
            "    .type = json_type_list_node_type,\n"
            "    .attr.list = &__%zu,\n"
            "    .node.list = {\n"
            "        .args = __%zu,\n"
            "        .size = %zu\n"
            "    }\n"
            "};\n\n",
            t->val, v->val, u->val,
            n.list->size);
    }
    else
        UNEXPECT_VAR("%d", node->type);
}

static void json_type_defs_gen_def(
    const struct json_type_defs_t* defs,
    struct json_type_ptr_space_t* space,
    FILE* file)
{
    const struct json_type_defs_arg_t *p, *e;
    struct json_type_ptr_treap_node_t *t, *u;

    for (p = defs->args,
         e = p + defs->size;
         p < e;
         p ++) {
        json_type_node_gen_def(p->type, space, file);
    }

    u = JSON_TYPE_PTR_SPACE_INSERT(defs->args);

    fprintf(file,
        "static const struct json_type_defs_arg_t __%zu[] = {\n",
        u->val);

    for (p = defs->args,
         e = p + defs->size;
         p < e;
         p ++) {
        fputs(
            "    {\n"
            "        .name = (const uchar_t*) ",
            file);

        json_type_print_repr(p->name, true, file);

        t = JSON_TYPE_PTR_SPACE_LOOKUP(p->type);
        fprintf(file,
            ",\n"
            "        .type = &__%zu\n"
            "    }%s\n",
            t->val, p < e - 1 ? "," : "");
    }
    fputs("};\n\n", file);

    t = JSON_TYPE_PTR_SPACE_INSERT(defs);

    fprintf(file,
        "static const struct json_type_defs_t __%zu = {\n"
        "    .args = __%zu,\n"
        "    .size = %zu\n"
        "};\n\n",
        t->val, u->val, defs->size);
}

static bool json_get_current_time(char* buf, size_t len)
{
    struct tm m;
    time_t t;
    size_t s;

    t = time(NULL);
    if (localtime_r(&t, &m) == NULL)
        return false;

    s = strftime(
        buf, len,
        "%a %b %e %H:%M:%S %Z %Y",
        &m);
    ASSERT(s < len);

    return true;
}

#undef  CASE
#define CASE(n) \
    [json_type_def_ ## n ## _type] = #n

static void json_type_def_gen_def(
    const struct json_type_def_t* def,
    struct json_type_ptr_space_t* space,
    FILE* file)
{
    static const char* const def_types[] = {
        CASE(node),
        CASE(defs)
    };
    struct json_type_ptr_treap_node_t* n = NULL;
    struct json_type_attrs_t a;
    const char *d;
    char b[64];

    a = json_type_def_get_attrs(def);

    fprintf(file,
        "//\n"
        "// JSON type objects definitions.\n"
        "//\n"
        "// Product of `%s' library, version %d.%d.%d\n",
        STRINGIFY(PROGRAM),
        JSON_VERSION_MAJOR,
        JSON_VERSION_MINOR,
        JSON_VERSION_PATCH);
    if (json_get_current_time(b, sizeof(b)))
        fprintf(file, "// Generated on %s\n", b);
    fputs(
        "//\n\n",
        file);
    if (a.has_cells)
        fputs(
            "#define EQ  cell.eq\n"
            "#define VAL cell.val\n\n",
            file);

    if (def->type == json_type_def_node_type)
        json_type_node_gen_def(def->val.node, space, file);
    else
    if (def->type == json_type_def_defs_type)
        json_type_defs_gen_def(def->val.defs, space, file);
    else
        UNEXPECT_VAR("%d", def->type);

    n = JSON_TYPE_PTR_SPACE_LOOKUP(
            def->type == json_type_def_node_type
                ? (void*) def->val.node
                : (void*) def->val.defs);
    ASSERT(n != NULL);

    d = ARRAY_CHAR_NULL_ELEM(
        def_types,
        def->type);
    ASSERT(d != NULL);

    fprintf(file,
        "static const struct json_type_def_t MODULE_TYPE_DEF = {\n"
        "    .type = json_type_def_%s_type,\n"
        "    .val.%s = &__%zu\n"
        "};\n\n",
        d, d, n->val);

    ASSERT(a.ptr_count == space->index);
}

#define JSON_TYPE_TRIE_SYM_IS_NULL(s)              \
    (                                              \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_CONST(s)), \
        (s) == NULL                                \
    )
#define JSON_TYPE_TRIE_SYM_AS_IF_NODE(s)           \
    (                                              \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_CONST(s)), \
        (s)                                        \
    )
#define JSON_TYPE_TRIE_SYM_IS_ANY(s)               \
    (                                              \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_CONST(s)), \
        JSON_TYPE_NODE_IS_ANY_CONST(s, type)       \
    )
#define JSON_TYPE_TRIE_SYM_CMP(x, y)               \
    (                                              \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_CONST(x)), \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_CONST(y)), \
        json_type_trie_sym_cmp(x, y)               \
    )

#undef  TRIE_NAME
#define TRIE_NAME           json_type
#undef  TRIE_SYM_TYPE
#define TRIE_SYM_TYPE       const struct json_type_node_t*
#undef  TRIE_NULL_SYM
#define TRIE_NULL_SYM()     (NULL)
#undef  TRIE_SYM_IS_NULL
#define TRIE_SYM_IS_NULL    JSON_TYPE_TRIE_SYM_IS_NULL
#undef  TRIE_SYM_IS_ANY
#define TRIE_SYM_IS_ANY     JSON_TYPE_TRIE_SYM_IS_ANY
#undef  TRIE_SYM_CMP
#define TRIE_SYM_CMP        JSON_TYPE_TRIE_SYM_CMP
#undef  TRIE_SYM_PRINT
#define TRIE_SYM_PRINT      json_type_node_print
#undef  TRIE_SYM_CMP_NAME
#define TRIE_SYM_CMP_NAME   json_type_trie_sym_cmp
#undef  TRIE_VAL_TYPE
#define TRIE_VAL_TYPE       const struct json_type_node_t*
#undef  TRIE_VAL_PRINT
#define TRIE_VAL_PRINT      json_type_node_print
#undef  TRIE_KEY_TYPE
#define TRIE_KEY_TYPE       const struct json_type_node_t**
#undef  TRIE_KEY_INC
#define TRIE_KEY_INC(k)     ((k) ++)
#undef  TRIE_KEY_DEREF
#define TRIE_KEY_DEREF(k)   ((*k))
#undef  TRIE_ALLOC_OBJ_TYPE
#define TRIE_ALLOC_OBJ_TYPE struct json_type_lib_t
#undef  TRIE_PTR_SPACE_TYPE
#define TRIE_PTR_SPACE_TYPE struct json_type_ptr_space_t
#undef  TRIE_PTR_SPACE_NODE_TYPE
#define TRIE_PTR_SPACE_NODE_TYPE \
                            struct json_type_ptr_treap_node_t
#undef  TRIE_VALIDATE_PTR_SPACE_INSERT
#define TRIE_VALIDATE_PTR_SPACE_INSERT \
                            JSON_TYPE_PTR_SPACE_VALIDATE_INSERT_
#undef  TRIE_SYM_VALIDATE
#define TRIE_SYM_VALIDATE   JSON_TYPE_PTR_SPACE_VALIDATE_NON_NULL
#undef  TRIE_VAL_VALIDATE
#define TRIE_VAL_VALIDATE   JSON_TYPE_PTR_SPACE_VALIDATE
#undef  TRIE_GEN_DEF_PTR_SPACE_LOOKUP
#define TRIE_GEN_DEF_PTR_SPACE_LOOKUP \
                            JSON_TYPE_PTR_SPACE_LOOKUP
#undef  TRIE_GEN_DEF_PTR_SPACE_INSERT
#define TRIE_GEN_DEF_PTR_SPACE_INSERT \
                            JSON_TYPE_PTR_SPACE_GEN_DEF_INSERT_

#undef  TRIE_SYM_GEN_DEF
#define TRIE_SYM_GEN_DEF(s)                          \
    do {                                             \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_CONST(s));   \
        if ((s) != NULL) {                           \
            struct json_type_ptr_treap_node_t* __t = \
                JSON_TYPE_PTR_SPACE_LOOKUP(s);       \
            fprintf(file,                            \
                "&__%zu",                            \
                __t->val);                           \
        }                                            \
        else                                         \
            fputs(                                   \
                "NULL",                              \
                file);                               \
    } while (0)

#define TRIE_NEED_PRINT
#define TRIE_NEED_GEN_DEF
#define TRIE_NEED_VALIDATE
#define TRIE_NEED_SIB_ITERATOR
#define TRIE_NEED_LVL_ITERATOR
#define TRIE_NEED_LOOKUP_SYM
#define TRIE_NEED_INSERT_SYM
#define TRIE_NEED_MATCH_SYM
#define TRIE_NEED_NODE_GET_LEAF
#define TRIE_NEED_NODE_GET_SIB_COUNT
#define TRIE_NEED_NODE_GET_SIB_COUNT_IF
#define TRIE_NEED_GET_NODE_COUNT
#define TRIE_NEED_LOOKUP_SYM_NODE
#define TRIE_NEED_LOOKUP_FIRST_NODE
#define TRIE_NEED_NODE_HAS_NULL_SIB
#define TRIE_NEED_NODE_HAS_NON_NULL_SIB
#define TRIE_NEED_IMPL_ONLY
#include "trie-impl.h"
#undef  TRIE_NEED_IMPL_ONLY
#undef  TRIE_NEED_NODE_HAS_NON_NULL_SIB
#undef  TRIE_NEED_NODE_HAS_NULL_SIB
#undef  TRIE_NEED_LOOKUP_FIRST_NODE
#undef  TRIE_NEED_LOOKUP_SYM_NODE
#undef  TRIE_NEED_GET_NODE_COUNT
#undef  TRIE_NEED_NODE_GET_SIB_COUNT_IF
#undef  TRIE_NEED_NODE_GET_SIB_COUNT
#undef  TRIE_NEED_NODE_GET_LEAF
#undef  TRIE_NEED_MATCH_SYM
#undef  TRIE_NEED_INSERT_SYM
#undef  TRIE_NEED_LOOKUP_SYM
#undef  TRIE_NEED_LVL_ITERATOR
#undef  TRIE_NEED_SIB_ITERATOR
#undef  TRIE_NEED_VALIDATE
#undef  TRIE_NEED_GEN_DEF
#undef  TRIE_NEED_PRINT

#define JSON_TYPE_TRIE_IS_EMPTY(t) \
    TRIE_IS_EMPTY_CONST(json_type, t)

#define JSON_TYPE_TRIE_NODE_AS_EQ(p) \
    TRIE_NODE_AS_EQ( \
        json_type, p, \
        JSON_TYPE_TRIE_SYM_IS_NULL)
#define JSON_TYPE_TRIE_NODE_AS_VAL(p) \
    TRIE_NODE_AS_VAL( \
        json_type, p, \
        JSON_TYPE_TRIE_SYM_IS_NULL)

#define JSON_TYPE_TRIE_NODE_AS_VAL_REF(p)          \
    ({                                             \
        const struct json_type_node_t* const* __r; \
        __r = TRIE_NODE_AS_VAL_REF(                \
            json_type, p,                          \
            JSON_TYPE_TRIE_SYM_IS_NULL);           \
        CONST_CAST(__r,                            \
            const struct json_type_node_t*);       \
    })

static enum trie_sym_cmp_t json_type_node_cmp(
    const struct json_type_node_t* x,
    const struct json_type_node_t* y);

static enum trie_sym_cmp_t json_type_any_node_cmp(
    const struct json_type_any_node_t* x,
    const struct json_type_any_node_t* y)
{
    if (x->type == y->type)
        return trie_sym_cmp_eq;
    if (x->type == json_type_any_type_type)
        return trie_sym_cmp_gt;
    if (y->type == json_type_any_type_type)
        return trie_sym_cmp_lt;
    if (x->type < y->type)
        return trie_sym_cmp_lt;
    else
        return trie_sym_cmp_gt;
}

static enum trie_sym_cmp_t trie_str_cmp(
    const uchar_t* x,
    const uchar_t* y)
{
    int r = strucmp(x, y);
    if (r < 0)
        return trie_sym_cmp_lt;
    if (r > 0)
        return trie_sym_cmp_gt;
    else
        return trie_sym_cmp_eq;
}

static enum trie_sym_cmp_t trie_num_cmp(
    const uchar_t* x,
    const uchar_t* y)
{
    size_t n, m;

    ENSURE(
        strupbrk(x, ".eE") == NULL &&
        strupbrk(y, ".eE") == NULL,
        "non-integer numerical comparison "
        "yet not supported");

    // stev: note that JSON numbers are by definition
    // normalized: there are no leading '0's unless the
    // number is '0' in its entirety; also there is no
    // leading '+' sign 

    if (((x[0] == '0' && x[1] == '\0') ||
         (x[0] == '-' && x[1] == '0' && x[2] == '\0')) &&
        ((y[0] == '0' && y[1] == '\0') ||
         (y[0] == '-' && y[1] == '0' && y[2] == '\0')))
        return trie_sym_cmp_eq;

    if (*x == '-' && *y != '-')
        return trie_sym_cmp_lt;
    if (*x != '-' && *y == '-')
        return trie_sym_cmp_gt;

    n = strulen(x);
    ASSERT(n > 0);
    m = strulen(y);
    ASSERT(m > 0);

    if (n < m)
        return trie_sym_cmp_lt;
    if (n > m)
        return trie_sym_cmp_gt;
    else
        return trie_str_cmp(x, y);
}

static enum trie_sym_cmp_t json_type_plain_node_cmp(
    const struct json_type_plain_node_t* x,
    const struct json_type_plain_node_t* y)
{
    if (x->type < y->type)
        return trie_sym_cmp_lt;
    if (x->type > y->type)
        return trie_sym_cmp_gt;
    if (x->type == json_type_plain_null_type)
        return trie_sym_cmp_eq;
    if (x->type == json_type_plain_boolean_type) {
        if (x->val.boolean == y->val.boolean)
            return trie_sym_cmp_eq;
        if (!x->val.boolean)
            return trie_sym_cmp_lt;
        else
            return trie_sym_cmp_gt;
    }
    if (x->type == json_type_plain_number_type)
        return trie_num_cmp(x->val.number, y->val.number);
    if (x->type == json_type_plain_string_type)
        return trie_str_cmp(x->val.string, y->val.string);
    else
        UNEXPECT_VAR("%d", x->type);
}

static enum trie_sym_cmp_t json_type_object_node_cmp(
    const struct json_type_object_node_t* x,
    const struct json_type_object_node_t* y)
{
    const struct json_type_object_node_arg_t *p, *q, *e;
    const size_t n = x->size < y->size ? x->size : y->size;
    enum trie_sym_cmp_t c;

    for (p = x->args,
         q = y->args,
         e = p + n;
         p < e;
         p ++,
         q ++) {
        c = trie_str_cmp(p->name, q->name);
        if (c != trie_sym_cmp_eq)
            return c;
        c = json_type_node_cmp(p->type, q->type);
        if (c != trie_sym_cmp_eq)
            return c;
    }
    if (x->size == y->size)
        return trie_sym_cmp_eq;
    if (x->size < y->size)
        return trie_sym_cmp_lt;
    else
        return trie_sym_cmp_gt;
}

static enum trie_sym_cmp_t json_type_open_array_node_cmp(
    const struct json_type_open_array_node_t* x,
    const struct json_type_open_array_node_t* y)
{
    return json_type_node_cmp(x->arg, y->arg);
}

static enum trie_sym_cmp_t json_type_closed_array_node_cmp(
    const struct json_type_closed_array_node_t* x,
    const struct json_type_closed_array_node_t* y)
{
    const struct json_type_node_t **p, **q, **e;
    const size_t n = x->size < y->size ? x->size : y->size;
    enum trie_sym_cmp_t c;

    for (p = x->args,
         q = y->args,
         e = p + n;
         p < e;
         p ++,
         q ++) {
        c = json_type_node_cmp(*p, *q);
        if (c != trie_sym_cmp_eq)
            return c;
    }
    if (x->size == y->size)
        return trie_sym_cmp_eq;
    if (x->size < y->size)
        return trie_sym_cmp_lt;
    else
        return trie_sym_cmp_gt;
}

static enum trie_sym_cmp_t json_type_array_node_cmp(
    const struct json_type_array_node_t* x,
    const struct json_type_array_node_t* y)
{
    if (x->type < y->type)
        return trie_sym_cmp_lt;
    if (x->type > y->type)
        return trie_sym_cmp_gt;
    if (x->type == json_type_open_array_node_type)
        return json_type_open_array_node_cmp(
            &x->val.open, &y->val.open);
    if (x->type == json_type_closed_array_node_type)
        return json_type_closed_array_node_cmp(
            &x->val.closed, &y->val.closed);
    else
        UNEXPECT_VAR("%d", x->type);
}

static enum trie_sym_cmp_t json_type_list_node_cmp(
    const struct json_type_list_node_t* x,
    const struct json_type_list_node_t* y)
{
    const struct json_type_node_t **p, **q, **e;
    const size_t n = x->size < y->size ? x->size : y->size;
    enum trie_sym_cmp_t c;

    for (p = x->args,
         q = y->args,
         e = p + n;
         p < e;
         p ++,
         q ++) {
        c = json_type_node_cmp(*p, *q);
        if (c != trie_sym_cmp_eq)
            return c;
    }
    if (x->size == y->size)
        return trie_sym_cmp_eq;
    if (x->size < y->size)
        return trie_sym_cmp_lt;
    else
        return trie_sym_cmp_gt;
}

static enum trie_sym_cmp_t json_type_node_cmp(
    const struct json_type_node_t* x,
    const struct json_type_node_t* y)
{
    if (x == y)
        return trie_sym_cmp_eq;
    if (x->type < y->type)
        return trie_sym_cmp_lt;
    if (x->type > y->type)
        return trie_sym_cmp_gt;
    if (x->type == json_type_any_node_type)
        return json_type_any_node_cmp(
            &x->node.any, &y->node.any);
    if (x->type == json_type_plain_node_type)
        return json_type_plain_node_cmp(
            &x->node.plain, &y->node.plain);
    if (x->type == json_type_object_node_type)
        return json_type_object_node_cmp(
            &x->node.object, &y->node.object);
    if (x->type == json_type_array_node_type)
        return json_type_array_node_cmp(
            &x->node.array, &y->node.array);
    if (x->type == json_type_list_node_type)
        return json_type_list_node_cmp(
            &x->node.list, &y->node.list);
    else
        UNEXPECT_VAR("%d", x->type);
}

static enum trie_sym_cmp_t json_type_trie_sym_cmp(
    const struct json_type_node_t* x,
    const struct json_type_node_t* y)
{
    // stev: note that NULL is a legal symbol
    // in 'json_type_trie_t': it is playing
    // the role of 'end of key' marker!
    if (x == NULL && y == NULL)
        return trie_sym_cmp_eq;
    if (x == NULL)
        return trie_sym_cmp_lt;
    if (y == NULL)
        return trie_sym_cmp_gt;
    else
        return json_type_node_cmp(x, y);
}

#undef  CASE1
#define CASE1(n)                                          \
    case json_type_any_ ## n ## _type:                    \
        return                                            \
            (p = JSON_TYPE_NODE_AS_IF_CONST(t, plain)) && \
            (p->type == a->type - 1)

#undef  CASE2
#define CASE2(n)                             \
    case json_type_any_ ## n ## _type:       \
        return JSON_TYPE_NODE_IS_CONST(t, n)               

static bool json_type_node_match(
    const struct json_type_node_t* x,
    const struct json_type_node_t* y)
{
    const struct json_type_any_node_t *a, *b;
    const struct json_type_plain_node_t* p;
    const struct json_type_node_t* t;

    STATIC(
        json_type_plain_null_type + 1 ==
        json_type_any_null_type &&

        json_type_plain_boolean_type + 1 ==
        json_type_any_boolean_type &&

        json_type_plain_number_type + 1 ==
        json_type_any_number_type &&

        json_type_plain_string_type + 1 ==
        json_type_any_string_type);

    ASSERT(x != NULL);
    ASSERT(y != NULL);

    if (x == y)
        return true;

    if (!(a = JSON_TYPE_NODE_AS_IF_CONST(x, any)))
          a = JSON_TYPE_NODE_AS_IF_CONST(y, any);

    if (a == NULL)
        return json_type_node_cmp(x, y) ==
            trie_sym_cmp_eq;

    if (a->type == json_type_any_type_type)
        return true;

    t = JSON_TYPE_TO_NODE_CONST(a, any);
    ASSERT(t == x || t == y);
    t = t == x ? y : x;

    if ((b = JSON_TYPE_NODE_AS_IF_CONST(t, any)))
        return
            b->type == json_type_any_type_type ||
            b->type == a->type;

    switch (a->type) {
    CASE1(null);
    CASE1(boolean);
    CASE1(number);
    CASE1(string);
    CASE2(object);
    CASE2(array);
    default:
        UNEXPECT_VAR("%d", a->type);
    }
}

struct json_type_object_sym_key_t
{
    const struct json_type_object_node_t* object;
    size_t n;
};

static enum trie_sym_cmp_t json_type_object_sym_cmp(
    const struct json_type_object_sym_t* x,
    const struct json_type_object_sym_t* y)
{
    // stev: comparison with 'null' sym are allowed:
    if (x->type == json_type_object_sym_null_type &&
        y->type == json_type_object_sym_null_type)
        return trie_sym_cmp_eq;
    if (x->type == json_type_object_sym_null_type)
        return trie_sym_cmp_lt;
    if (y->type == json_type_object_sym_null_type)
        return trie_sym_cmp_gt;

    // stev: comparing non-'null' syms of different
    // types shouldn't happen; if did happen, then
    // we got into an internal error situation
    ENSURE(x->type == y->type,
        "invalid object trie: comparing symbols"
        " of different types");

    if (x->type == json_type_object_sym_name_type)
        return trie_str_cmp(x->val.name, y->val.name);
    if (x->type == json_type_object_sym_node_type)
        return json_type_node_cmp(x->val.node, y->val.node);
    else
        UNEXPECT_VAR("%d", x->type);
}

static struct json_type_object_sym_t
    json_type_object_sym_key_deref(
        const struct json_type_object_sym_key_t* k)
{
    static const struct json_type_object_sym_t
        null_sym = {
            .type = json_type_object_sym_null_type
        };

    struct json_type_object_sym_t r;
    size_t i = k->n / 2;

    ASSERT(i <= k->object->size); 
    if (i == k->object->size)
        return null_sym;

    if ((k->n % 2) == 0) {
        r.type = json_type_object_sym_name_type;
        r.val.name = k->object->args[i].name;
    }
    else {
        r.type = json_type_object_sym_node_type;
        r.val.node = k->object->args[i].type;
    }
    return r;
}

static void json_type_object_sym_print(
    const struct json_type_object_sym_t sym, FILE* file)
{
    switch (sym.type) {
    case json_type_object_sym_null_type:
        fputs("null", file);
        break;
    case json_type_object_sym_name_type:
        fputs("{\"name\":", file);
        json_type_print_repr(sym.val.name, true, file);
        fputc('}', file);
        break;
    case json_type_object_sym_node_type:
        fputs("{\"node\":", file);
        json_type_node_print(sym.val.node, file);
        fputc('}', file);
        break;
    default:
        UNEXPECT_VAR("%d", sym.type);
    }
}

#define JSON_TYPE_TYPEOF_IS_OBJECT_SYM(p) \
    TYPEOF_IS(p, struct json_type_object_sym_t)

#define JSON_TYPE_TYPEOF_IS_OBJECT_SYM_KEY(v) \
    TYPEOF_IS(v, struct json_type_object_sym_key_t)

#define JSON_TYPE_OBJECT_TRIE_SYM_IS(s, n)              \
    (                                                   \
        STATIC(JSON_TYPE_TYPEOF_IS_OBJECT_SYM(s)),      \
        (s).type == json_type_object_sym_ ## n ## _type \
    )
#define JSON_TYPE_OBJECT_TRIE_SYM_IS_NULL(s) \
    JSON_TYPE_OBJECT_TRIE_SYM_IS(s, null)

#define JSON_TYPE_OBJECT_TRIE_SYM_AS(s, n)          \
    ({                                              \
        ASSERT(JSON_TYPE_OBJECT_TRIE_SYM_IS(s, n)); \
        (s).val.n;                                  \
    })
#define JSON_TYPE_OBJECT_TRIE_SYM_AS_IF(s, n) \
    ({                                        \
        JSON_TYPE_OBJECT_TRIE_SYM_IS(s, n)    \
        ? (s).val.n : NULL;                   \
    })
#define JSON_TYPE_OBJECT_TRIE_SYM_AS_IF_NODE(s) \
    JSON_TYPE_OBJECT_TRIE_SYM_AS_IF(s, node)

#define JSON_TYPE_OBJECT_TRIE_SYM_CMP(x, y)      \
    ({                                           \
        struct json_type_object_sym_t __p = (x); \
        struct json_type_object_sym_t __q = (y); \
        json_type_object_sym_cmp(&__p, &__q);    \
    })

#define JSON_TYPE_OBJECT_TRIE_NULL_SYM() \
    ({                                              \
        struct json_type_object_sym_t __s = {       \
            .type = json_type_object_sym_null_type, \
        };                                          \
        __s;                                        \
    })

#undef  TRIE_NAME
#define TRIE_NAME json_type_object

#undef  TRIE_SYM_TYPE
#define TRIE_SYM_TYPE struct json_type_object_sym_t

#undef  TRIE_NULL_SYM
#define TRIE_NULL_SYM JSON_TYPE_OBJECT_TRIE_NULL_SYM

#undef  TRIE_SYM_IS_NULL
#define TRIE_SYM_IS_NULL JSON_TYPE_OBJECT_TRIE_SYM_IS_NULL

#undef  TRIE_SYM_IS_ANY

#undef  TRIE_SYM_CMP
#define TRIE_SYM_CMP JSON_TYPE_OBJECT_TRIE_SYM_CMP

#undef  TRIE_SYM_PRINT
#define TRIE_SYM_PRINT json_type_object_sym_print

#undef  TRIE_SYM_CMP_NAME

#undef  TRIE_KEY_TYPE
#define TRIE_KEY_TYPE struct json_type_object_sym_key_t

#undef  TRIE_KEY_INC
#define TRIE_KEY_INC(k)                                \
    do {                                               \
        STATIC(JSON_TYPE_TYPEOF_IS_OBJECT_SYM_KEY(k)); \
        (k).n ++;                                      \
    } while (0)

#undef  TRIE_KEY_DEREF
#define TRIE_KEY_DEREF(k) \
    (                                                  \
        STATIC(JSON_TYPE_TYPEOF_IS_OBJECT_SYM_KEY(k)), \
        json_type_object_sym_key_deref(&(k))           \
    )

#undef  TRIE_SYM_VALIDATE
#define TRIE_SYM_VALIDATE(s) \
    json_type_object_sym_validate(&(s), space, result)

#undef  TRIE_SYM_GEN_DEF
#define TRIE_SYM_GEN_DEF(s) \
    json_type_object_sym_gen_def(&(s), space, file)

#define TRIE_NEED_PRINT
#define TRIE_NEED_GEN_DEF
#define TRIE_NEED_VALIDATE
#define TRIE_NEED_SIB_ITERATOR
#define TRIE_NEED_LVL_ITERATOR
#define TRIE_NEED_NODE_GET_LEAF
#define TRIE_NEED_NODE_GET_SIB_COUNT
#define TRIE_NEED_GET_NODE_COUNT
#define TRIE_NEED_LOOKUP_SYM_NODE
#define TRIE_NEED_LOOKUP_FIRST_NODE
#define TRIE_NEED_NODE_HAS_NULL_SIB
#define TRIE_NEED_NODE_HAS_NON_NULL_SIB
#define TRIE_NEED_IMPL_ONLY
#include "trie-impl.h"
#undef  TRIE_NEED_IMPL_ONLY
#undef  TRIE_NEED_NODE_HAS_NON_NULL_SIB
#undef  TRIE_NEED_NODE_HAS_NULL_SIB
#undef  TRIE_NEED_LOOKUP_FIRST_NODE
#undef  TRIE_NEED_LOOKUP_SYM_NODE
#undef  TRIE_NEED_GET_NODE_COUNT
#undef  TRIE_NEED_NODE_GET_SIB_COUNT
#undef  TRIE_NEED_NODE_GET_LEAF
#undef  TRIE_NEED_SIB_ITERATOR
#undef  TRIE_NEED_LVL_ITERATOR
#undef  TRIE_NEED_VALIDATE
#undef  TRIE_NEED_GEN_DEF
#undef  TRIE_NEED_PRINT

#define JSON_TYPE_OBJECT_TRIE_IS_EMPTY(t) \
    TRIE_IS_EMPTY_CONST(json_type_object, t)

#define JSON_TYPE_OBJECT_TRIE_NODE_AS_EQ(p) \
    TRIE_NODE_AS_EQ(                        \
        json_type_object, p,                \
        JSON_TYPE_OBJECT_TRIE_SYM_IS_NULL)
#define JSON_TYPE_OBJECT_TRIE_NODE_AS_VAL(p) \
    TRIE_NODE_AS_VAL(                        \
        json_type_object, p,                 \
        JSON_TYPE_OBJECT_TRIE_SYM_IS_NULL)

#define JSON_TYPE_OBJECT_TRIE_NODE_AS_VAL_REF(p)   \
    ({                                             \
        const struct json_type_node_t* const* __r; \
        __r = TRIE_NODE_AS_VAL_REF(                \
            json_type_object, p,                   \
            JSON_TYPE_OBJECT_TRIE_SYM_IS_NULL);    \
        CONST_CAST(__r,                            \
            const struct json_type_node_t*);       \
    })

#undef  CASE
#define CASE(n)                                       \
static const struct json_type_node_t                  \
    json_type_any_ ## n ## _node = {                  \
        .type = json_type_any_node_type,              \
        .node.any.type = json_type_any_ ## n ## _type \
    }

CASE(type);
CASE(null);
CASE(boolean);
CASE(number);
CASE(string);
CASE(object);
CASE(array);

struct json_type_plain_node_bits_t
{
    bits_t null: 1;
    bits_t boolean: 1;
    bits_t number: 1;
    bits_t string: 1;
};

#undef  CASE
#define CASE(n) \
    case json_type_plain_ ## n ## _type: return !bits->n;

static bool json_type_trie_node_is_not_plain_bit(
    const struct json_type_plain_node_bits_t* bits,
    const struct json_type_trie_node_t* node)
{
    const struct json_type_plain_node_t* p;

    ASSERT(node != NULL);

    ASSERT(node->sym != NULL);
    if (!(p = JSON_TYPE_NODE_AS_IF_CONST(node->sym, plain)))
        return true;

    switch (p->type) {
    CASE(null);
    CASE(boolean);
    CASE(number);
    CASE(string);
    default:
        UNEXPECT_VAR("%d", p->type);
    }
}

#define JSON_TYPE_PRINT_BASE_FIRST_SEP(c, f, l) \
    (                                           \
        !(f) ? NULL                             \
             : (c) > 1 || !(l)                  \
             ? json_type_print_base_comma_sep   \
             : json_type_print_base_or_sep      \
    )
#define JSON_TYPE_PRINT_BASE_LAST_SEP(l)      \
    (                                         \
        !(l) ? json_type_print_base_comma_sep \
             : json_type_print_base_or_sep    \
    )

static const char json_type_print_base_comma_sep[] = ", ";
static const char json_type_print_base_or_sep[] = " or ";

static void json_type_trie_print_base(
    const struct json_type_trie_t* trie,
    size_t count, bool first_sep, bool last_sep,
    json_type_trie_node_cond_func_t cond, void* cond_obj,
    FILE* file)
{
    struct json_type_trie_sib_iterator_t i;
    const struct json_type_trie_node_t* t;
    const char* s;
    size_t n = 0;

    ASSERT(trie != NULL);

    if (count && (s = JSON_TYPE_PRINT_BASE_FIRST_SEP(
            count, first_sep, last_sep)))
        fputs(s, file);

    ASSERT(trie->root != NULL);
    json_type_trie_sib_iterator_init(&i, trie->root, 0);

    while (!json_type_trie_sib_iterator_at_end(&i)) {
        t = json_type_trie_sib_iterator_deref(&i);
        ASSERT(t != NULL);

        if (!cond || cond(cond_obj, t)) {
            ASSERT(t->sym != NULL);
            json_type_node_print_base(t->sym, file);

            if ((s = ++ n < SIZE_DEC(count)
                    ? json_type_print_base_comma_sep
                    : n < count
                    ? JSON_TYPE_PRINT_BASE_LAST_SEP(
                        last_sep)
                    : NULL)) {
                fputs(s, file);
            }
        }

        json_type_trie_sib_iterator_inc(&i);
    }

    json_type_trie_sib_iterator_done(&i);

    ASSERT(n == count);
}

#define JSON_TYPE_TRIE_LOOKUP_SYM_NODE_ANY(t, n)    \
    (                                               \
        json_type_trie_lookup_sym_node(             \
            t, &json_type_any_ ## n ## _node, NULL) \
    )

static void json_type_list_attr_print_base(
    const struct json_type_list_attr_t* attr, FILE* file)
{
    struct json_type_plain_node_bits_t a;
    struct {
        size_t any;
        size_t plain;
        size_t object;
        size_t open_array;
        size_t closed_array;
    } c;
    struct {
        bits_t object: 1;
        bits_t array: 1;
    } t, l;
    const char* s;

    ASSERT(attr != NULL);

    memset(&a, 0, sizeof(a));
    memset(&t, 0, sizeof(t));
    memset(&c, 0, sizeof(c));

    if (attr->any != NULL) {
        ASSERT(attr->any->root != NULL);
        c.any = json_type_trie_node_get_sib_count(
            attr->any->root);
    }

    if (c.any) {
        if (JSON_TYPE_TRIE_LOOKUP_SYM_NODE_ANY(
                attr->any->root,
                type)) {
            json_type_node_print_complete(
                &json_type_any_type_node,
                file);
            return;
        }

        a.null = JSON_TYPE_TRIE_LOOKUP_SYM_NODE_ANY(
            attr->any->root,
            null);
        a.boolean = JSON_TYPE_TRIE_LOOKUP_SYM_NODE_ANY(
            attr->any->root,
            boolean);
        a.number = JSON_TYPE_TRIE_LOOKUP_SYM_NODE_ANY(
            attr->any->root,
            number);
        a.string = JSON_TYPE_TRIE_LOOKUP_SYM_NODE_ANY(
            attr->any->root,
            string);
    }

    if (attr->plain != NULL) {
        ASSERT(attr->plain->root != NULL);
        c.plain = json_type_trie_node_get_sib_count_if(
            attr->plain->root,
            (json_type_trie_node_cond_func_t)
            json_type_trie_node_is_not_plain_bit,
            &a);
    }

    if (attr->object != NULL) {
        ASSERT(attr->object->root != NULL);
        c.object = json_type_object_trie_node_get_sib_count(
            attr->object->root);
    }

    if (attr->open_array != NULL) {
        ASSERT(attr->open_array->root != NULL);
        c.open_array = json_type_trie_node_get_sib_count(
            attr->open_array->root);
    }

    if (attr->closed_array != NULL) {
        ASSERT(attr->closed_array->root != NULL);
        c.closed_array = json_type_trie_node_get_sib_count(
            attr->closed_array->root);
    }

    l.object = c.object > 0;
    l.array = c.open_array > 0 || c.closed_array > 0;

    if (c.any && l.object)
        t.object = JSON_TYPE_TRIE_LOOKUP_SYM_NODE_ANY(
            attr->any->root,
            object);

    if (c.any && l.array)
        t.array = JSON_TYPE_TRIE_LOOKUP_SYM_NODE_ANY(
            attr->any->root,
            array);

    if (l.object)
        l.object = !t.object;
    if (l.array)
        l.array = !t.array;

    if (c.any)
        json_type_trie_print_base(
            attr->any, c.any, false,
            !c.plain && !l.object && !l.array,
            NULL, NULL, file);

    if (c.plain)
        json_type_trie_print_base(
            attr->plain,
            c.plain, c.any,
            !l.object && !l.array,
            (json_type_trie_node_cond_func_t)
            json_type_trie_node_is_not_plain_bit,
            &a, file);

    if (l.object) {
        if ((s = JSON_TYPE_PRINT_BASE_FIRST_SEP(
                1, c.any || c.plain, !l.array)))
            fputs(s, file);
        fputs("`{\"type\":\"object\",...}'", file);
    }

    if (l.array) {
        if ((s = JSON_TYPE_PRINT_BASE_FIRST_SEP(
                1, c.any || c.plain || l.object, true)))
            fputs(s, file);
        fputs("`{\"type\":\"array\",...}'", file);
    }
}

static void json_type_list_attr_print(
    const struct json_type_list_attr_t* attr, FILE* file)
{
    fputc('{', file);

    fputs("\"any\":", file);
    json_type_trie_print(attr->any, file);
    fputc(',', file);

    fputs("\"plain\":", file);
    json_type_trie_print(attr->plain, file);
    fputc(',', file);

    fputs("\"object\":", file);
    json_type_object_trie_print(attr->object, file);
    fputc(',', file);

    fputs("\"array\":{", file);

    fputs("\"open\":", file);
    json_type_trie_print(attr->open_array, file);
    fputc(',', file);

    fputs("\"closed\":", file);
    json_type_trie_print(attr->closed_array, file);

    fputc('}', file);

    fputc('}', file);
}

#ifdef JSON_DEBUG
static void json_type_list_attr_print_debug(
    const struct json_type_list_attr_t* attr, FILE* file)
{
    json_type_list_attr_print(attr, file);
}

static void json_type_list_attr_null_print_debug(
    const struct json_type_list_attr_t* attr, FILE* file)
{
    if (attr != NULL)
        json_type_list_attr_print_debug(attr, file);
    else
        fputs("(nil)", file);
}
#endif

#define JSON_TYPE_LIST_ATTR_TRIE_GET_PTR_COUNT_(t, n) \
    (                                                 \
        list->n != NULL                               \
        ? 1 + json_type ## t ## trie_get_node_count(  \
            list->n)                                  \
        : 0                                           \
    )
#define JSON_TYPE_LIST_ATTR_TRIE_GET_PTR_COUNT(n) \
    JSON_TYPE_LIST_ATTR_TRIE_GET_PTR_COUNT_(_, n)
#define JSON_TYPE_LIST_ATTR_OBJ_TRIE_GET_PTR_COUNT(n) \
    JSON_TYPE_LIST_ATTR_TRIE_GET_PTR_COUNT_(_object_, n)

static size_t json_type_list_attr_get_ptr_count(
    const struct json_type_list_attr_t* list)
{
    return
        1 +
        JSON_TYPE_LIST_ATTR_TRIE_GET_PTR_COUNT(any) +
        JSON_TYPE_LIST_ATTR_TRIE_GET_PTR_COUNT(plain) +
        JSON_TYPE_LIST_ATTR_OBJ_TRIE_GET_PTR_COUNT(object) +
        JSON_TYPE_LIST_ATTR_TRIE_GET_PTR_COUNT(open_array) +
        JSON_TYPE_LIST_ATTR_TRIE_GET_PTR_COUNT(closed_array);
}

#undef  JSON_TYPE_PTR_SPACE_INSERT
#define JSON_TYPE_PTR_SPACE_INSERT \
    JSON_TYPE_PTR_SPACE_VALIDATE_INSERT_

#define JSON_TYPE_LIST_ATTR_TRIE_VALIDATE_(t, n) \
    (                                            \
        list->n == NULL ||                       \
        json_type ## t ## trie_validate(         \
            list->n, space, result)              \
    )
#define JSON_TYPE_LIST_ATTR_TRIE_VALIDATE(n) \
    JSON_TYPE_LIST_ATTR_TRIE_VALIDATE_(_, n)
#define JSON_TYPE_LIST_ATTR_OBJ_TRIE_VALIDATE(n) \
    JSON_TYPE_LIST_ATTR_TRIE_VALIDATE_(_object_, n)

static bool json_type_list_attr_validate(
    const struct json_type_list_attr_t* list,
    struct json_type_ptr_space_t* space,
    const void** result)
{
    JSON_TYPE_PTR_SPACE_INSERT(list);

    if (!JSON_TYPE_LIST_ATTR_TRIE_VALIDATE(any) ||
        !JSON_TYPE_LIST_ATTR_TRIE_VALIDATE(plain) ||
        !JSON_TYPE_LIST_ATTR_OBJ_TRIE_VALIDATE(object) ||
        !JSON_TYPE_LIST_ATTR_TRIE_VALIDATE(open_array) ||
        !JSON_TYPE_LIST_ATTR_TRIE_VALIDATE(closed_array))
        return false;

    *result = NULL;
    return true;
}

#undef  JSON_TYPE_PTR_SPACE_INSERT
#define JSON_TYPE_PTR_SPACE_INSERT \
    JSON_TYPE_PTR_SPACE_GEN_DEF_INSERT_

#define JSON_TYPE_LIST_ATTR_TRIE_GEN_DEF_(t, n) \
    do {                                        \
        if (list->n != NULL) {                  \
            json_type ## t ## trie_gen_def(     \
                list->n, space, file);          \
            b ++;                               \
        }                                       \
    } while (0)
#define JSON_TYPE_LIST_ATTR_TRIE_GEN_DEF(n) \
    JSON_TYPE_LIST_ATTR_TRIE_GEN_DEF_(_, n)
#define JSON_TYPE_LIST_ATTR_OBJ_TRIE_GEN_DEF(n) \
    JSON_TYPE_LIST_ATTR_TRIE_GEN_DEF_(_object_, n)

#define JSON_TYPE_LIST_ATTR_GEN_DEF(n)                 \
    do {                                               \
        const struct json_type_ptr_treap_node_t* __v;  \
        if (list->n != NULL) {                         \
            __v = JSON_TYPE_PTR_SPACE_LOOKUP(list->n); \
            fprintf(file,                              \
                "    ." #n " = &__%zu%s\n",            \
                __v->val, ++ c < b ? "," : "");        \
        }                                              \
    } while (0)

static void json_type_list_attr_gen_def(
    const struct json_type_list_attr_t* list,
    struct json_type_ptr_space_t* space,
    FILE* file)
{
    const struct json_type_ptr_treap_node_t* t;
    size_t b = 0, c = 0;

    JSON_TYPE_LIST_ATTR_TRIE_GEN_DEF(any);
    JSON_TYPE_LIST_ATTR_TRIE_GEN_DEF(plain);
    JSON_TYPE_LIST_ATTR_OBJ_TRIE_GEN_DEF(object);
    JSON_TYPE_LIST_ATTR_TRIE_GEN_DEF(open_array);
    JSON_TYPE_LIST_ATTR_TRIE_GEN_DEF(closed_array);

    t = JSON_TYPE_PTR_SPACE_INSERT(list);

    fprintf(file,
        "static const struct json_type_list_attr_t __%zu%s",
        t->val, b ? " = {\n" : "");

    JSON_TYPE_LIST_ATTR_GEN_DEF(any);
    JSON_TYPE_LIST_ATTR_GEN_DEF(plain);
    JSON_TYPE_LIST_ATTR_GEN_DEF(object);
    JSON_TYPE_LIST_ATTR_GEN_DEF(open_array);
    JSON_TYPE_LIST_ATTR_GEN_DEF(closed_array);

    fprintf(file,
        "%s;\n\n",
        b ? "}" : "");
}

enum json_type_lib_obj_type_t
{
    json_type_lib_obj_arg_type,
    json_type_lib_obj_node_type,
    json_type_lib_obj_defs_type
};

struct json_type_lib_obj_t
{
    enum json_type_lib_obj_type_t type;
    union {
        struct json_type_object_node_arg_t arg;
        const struct json_type_node_t*     node;
        const struct json_type_defs_t*     defs;
    };
};

#undef  STACK_NAME
#define STACK_NAME             json_type_lib
#undef  STACK_ELEM_TYPE
#define STACK_ELEM_TYPE        struct json_type_lib_obj_t
#undef  STACK_ELEM_EQUAL
#define STACK_ELEM_EQUAL(x, y) ((x)->type == (y)->type)

#define STACK_NEED_MAX_SIZE
#define STACK_NEED_CREATE_DESTROY
#include "stack-impl.h"
#undef  STACK_NEED_CREATE_DESTROY
#undef  STACK_NEED_MAX_SIZE

#define JSON_TYPE_LIB_STACK_OP(o, ...)         \
    ({                                         \
        struct json_type_lib_text_impl_t* __i; \
        __i = JSON_TYPE_LIB_IMPL_AS(text);     \
        o(__i->stack, ## __VA_ARGS__);         \
    })
#define JSON_TYPE_LIB_STACK_PUSH(t, n)               \
    do {                                             \
        struct json_type_lib_obj_t __o;              \
        __o.type = json_type_lib_obj_ ## t ## _type; \
        __o.t = n;                                   \
        JSON_TYPE_LIB_STACK_OP(STACK_PUSH, __o);     \
    } while (0)

#define JSON_TYPE_LIB_STACK_PUSH_NODE(n) JSON_TYPE_LIB_STACK_PUSH(node, n)
#define JSON_TYPE_LIB_STACK_PUSH_DEFS(d) JSON_TYPE_LIB_STACK_PUSH(defs, d)
#define JSON_TYPE_LIB_STACK_PUSH_ARG(a)  JSON_TYPE_LIB_STACK_PUSH(arg, a)

#define JSON_TYPE_LIB_STACK_SIZE()       JSON_TYPE_LIB_STACK_OP(STACK_SIZE)
#define JSON_TYPE_LIB_STACK_POP()        JSON_TYPE_LIB_STACK_OP(STACK_POP)
#define JSON_TYPE_LIB_STACK_POP_N(n)     JSON_TYPE_LIB_STACK_OP(STACK_POP_N, n)
#define JSON_TYPE_LIB_STACK_TOP_REF()    JSON_TYPE_LIB_STACK_OP(STACK_TOP_REF)

#define JSON_TYPE_LIB_TYPEOF_IS_OBJ(p) \
    TYPEOF_IS(p, struct json_type_lib_obj_t*)

#define JSON_TYPE_LIB_OBJ_IS(p, n)                    \
    (                                                 \
        STATIC(JSON_TYPE_LIB_TYPEOF_IS_OBJ(p)),       \
        (p)->type == json_type_lib_obj_ ## n ## _type \
    )

#define JSON_TYPE_LIB_OBJ_AS(o, n)          \
    ({                                      \
        ASSERT(JSON_TYPE_LIB_OBJ_IS(o, n)); \
        (o)->n;                             \
    })

#define JSON_TYPE_LIB_OBJ_AS_IF(o, n) \
    (                                 \
        JSON_TYPE_LIB_OBJ_IS(o, n)    \
        ? (o)->n : NULL               \
    )

#define JSON_TYPE_LIB_STACK_POP_AS(n)    \
    ({                                   \
        struct json_type_lib_obj_t __o;  \
        __o = JSON_TYPE_LIB_STACK_POP(); \
        JSON_TYPE_LIB_OBJ_AS(&__o, n);   \
    })

#define JSON_TYPE_LIB_STACK_POP_AS_ARG()  JSON_TYPE_LIB_STACK_POP_AS(arg)
#define JSON_TYPE_LIB_STACK_POP_AS_NODE() JSON_TYPE_LIB_STACK_POP_AS(node)

#define JSON_TYPE_LIB_OBJ_IS_ARG(o)  JSON_TYPE_LIB_OBJ_IS(o, arg)
#define JSON_TYPE_LIB_OBJ_IS_NODE(o) JSON_TYPE_LIB_OBJ_IS(o, node)

#define JSON_TYPE_LIB_OBJ_AS_ARG(o)  JSON_TYPE_LIB_OBJ_AS(o, arg)
#define JSON_TYPE_LIB_OBJ_AS_NODE(o) JSON_TYPE_LIB_OBJ_AS(o, node)

#define JSON_TYPE_LIB_OBJ_AS_IF_NODE(o) JSON_TYPE_LIB_OBJ_AS_IF(o, node)
#define JSON_TYPE_LIB_OBJ_AS_IF_DEFS(o) JSON_TYPE_LIB_OBJ_AS_IF(o, defs)

#undef  PRINT_DEBUG_COND
#define PRINT_DEBUG_COND lib->debug

struct json_type_lib_text_impl_t
{
    bits_t                        destroy_ast: 1;
    bits_t                        done_parse: 1;
    bits_t                        done_build: 1;
    struct pool_alloc_t           pool;
    struct json_type_lib_stack_t* stack;
    struct json_ast_t*            ast;
    struct json_type_def_t        def;
};

struct json_type_lib_sobj_impl_t
{
    const char* lib_name;
    void*       sobj;
};

enum json_type_lib_impl_type_t
{
    json_type_lib_text_impl_type,
    json_type_lib_sobj_impl_type
};

typedef enum json_parse_status_t (*json_type_lib_impl_parse_func_t)(
    struct json_type_lib_t*,
    void*, const uchar_t*,
    size_t);
typedef enum json_parse_status_t (*json_type_lib_impl_parse_done_func_t)(
    struct json_type_lib_t*,
    void*);
typedef const struct json_type_def_t* (*json_type_lib_impl_build_func_t)(
    struct json_type_lib_t*,
    void*);
typedef bool (*json_type_lib_impl_validate_func_t)(
    struct json_type_lib_t*,
    void*);

struct json_type_lib_impl_t
{
    enum json_type_lib_impl_type_t type;
    union {
        struct json_type_lib_text_impl_t text;
        struct json_type_lib_sobj_impl_t sobj;
    };

    json_type_lib_impl_parse_func_t      parse;
    json_type_lib_impl_parse_done_func_t parse_done;
    json_type_lib_impl_build_func_t      build;
    json_type_lib_impl_validate_func_t   validate;
};

union json_type_lib_impl_pack_t
{
    struct json_type_lib_text_impl_t* text;
    struct json_type_lib_sobj_impl_t* sobj;
};

struct json_type_lib_t
{
#ifdef JSON_DEBUG
    bits_t                            debug:
                                      debug_bits;
#endif
    struct json_type_lib_sizes_t      sizes;
    struct json_type_lib_impl_t       impl;
    const struct json_type_def_t*     def;
    enum json_parse_status_t          stat;
    struct json_type_lib_error_info_t error;
};

#define JSON_TYPE_LIB_SYS_BUF_ERROR(b)                  \
    do {                                                \
        ASSERT(b.error_info.type > 0);                  \
        lib->error.type = json_type_lib_error_sys;      \
        lib->error.sys.context = b.error_info.type - 1; \
        lib->error.sys.error = b.error_info.sys_error;  \
        lib->error.file_info.name = lib_name;           \
        lib->error.file_info.buf = NULL;                \
        lib->error.file_info.size = 0;                  \
        lib->stat = json_parse_status_error;            \
    } while (0)

#define JSON_TYPE_LIB_SYS_ERROR(c, e)              \
    do {                                           \
        lib->error.type = json_type_lib_error_sys; \
        lib->error.sys.context = c;                \
        lib->error.sys.error = e;                  \
        lib->error.file_info.name = lib_name;      \
        lib->error.file_info.buf = NULL;           \
        lib->error.file_info.size = 0;             \
        lib->stat = json_parse_status_error;       \
    } while (0)

#define JSON_TYPE_LIB_LIB_ERROR(e, l)              \
    do {                                           \
        lib->error.type = json_type_lib_error_lib; \
        lib->error.lib = e;                        \
        lib->error.file_info.name = l;             \
        lib->error.file_info.buf = NULL;           \
        lib->error.file_info.size = 0;             \
        lib->stat = json_parse_status_error;       \
    } while (0)

#define JSON_TYPE_LIB_AST_ERROR()                             \
    do {                                                      \
        ASSERT(json_ast_get_is_error(impl->ast));             \
        lib->error.type = json_type_lib_error_ast;            \
        lib->error.ast = *json_ast_get_error_info(impl->ast); \
        lib->error.file_info.name = lib_name;                 \
        lib->error.file_info.buf = NULL;                      \
        lib->error.file_info.size = 0;                        \
        lib->stat = json_parse_status_error;                  \
    } while (0)

#define JSON_TYPE_LIB_IMPL_IS(n) \
    (lib->impl.type == json_type_lib_ ## n ## _impl_type)
#define JSON_TYPE_LIB_IMPL_AS(n)          \
    ({                                    \
        ASSERT(JSON_TYPE_LIB_IMPL_IS(n)); \
        &(lib->impl.n);                   \
    })
#define JSON_TYPE_LIB_IMPL_AS_IF(n) \
    (                               \
        JSON_TYPE_LIB_IMPL_IS(n)    \
        ? &(lib->impl.n) : NULL     \
    )

static void json_type_lib_text_impl_init(
    struct json_type_lib_text_impl_t* impl,
    size_t pool_size)
{
    pool_alloc_init(&impl->pool, pool_size);
}

static void json_type_lib_sobj_impl_init(
    struct json_type_lib_sobj_impl_t* impl,
    const char* lib_name)
{
    impl->lib_name = lib_name;
}

static void json_type_lib_init_base(
    struct json_type_lib_t* lib,
    enum json_type_lib_impl_type_t type,
    const struct json_type_lib_sizes_t* sizes)
{
    memset(lib, 0, sizeof(struct json_type_lib_t));

#ifdef JSON_DEBUG
    lib->debug = SIZE_TRUNC_BITS(
        json_debug_get_level(json_debug_type_lib_class),
        debug_bits);
#endif

    if (type == json_type_lib_text_impl_type ||
        type == json_type_lib_sobj_impl_type)
        lib->impl.type = type;
    else
        UNEXPECT_VAR("%d", type);

    lib->sizes = *sizes;
}

static enum json_parse_status_t json_type_lib_text_parse(
    struct json_type_lib_t*,
    struct json_type_lib_text_impl_t*,
    const uchar_t*, size_t);

static enum json_parse_status_t json_type_lib_text_parse_done(
    struct json_type_lib_t*,
    struct json_type_lib_text_impl_t*);

static const struct json_type_def_t* json_type_lib_text_build(
    struct json_type_lib_t*,
    struct json_type_lib_text_impl_t*);

static bool json_type_lib_text_validate(
    struct json_type_lib_t*,
    struct json_type_lib_text_impl_t*);

static enum json_parse_status_t json_type_lib_sobj_parse(
    struct json_type_lib_t*,
    struct json_type_lib_sobj_impl_t*,
    const uchar_t*, size_t);

static enum json_parse_status_t json_type_lib_sobj_parse_done(
    struct json_type_lib_t*,
    struct json_type_lib_sobj_impl_t*);

static const struct json_type_def_t* json_type_lib_sobj_build(
    struct json_type_lib_t*,
    struct json_type_lib_sobj_impl_t*);

static bool json_type_lib_sobj_validate(
    struct json_type_lib_t*,
    struct json_type_lib_sobj_impl_t*);

#define JSON_TYPE_LIB_IMPL_FUNC_INIT(f, n) \
    lib->impl.f = (json_type_lib_impl_ ## f ## _func_t) \
        json_type_lib_ ## n ## _ ## f

#define JSON_TYPE_LIB_INIT_IMPL(n, ...)              \
    do {                                             \
        JSON_TYPE_LIB_IMPL_FUNC_INIT(parse, n);      \
        JSON_TYPE_LIB_IMPL_FUNC_INIT(parse_done, n); \
        JSON_TYPE_LIB_IMPL_FUNC_INIT(build, n);      \
        JSON_TYPE_LIB_IMPL_FUNC_INIT(validate, n);   \
        json_type_lib_ ## n ## _impl_init(           \
            JSON_TYPE_LIB_IMPL_AS(n),                \
            ## __VA_ARGS__);                         \
    } while (0)

static void json_type_lib_init_from_source_text(
    struct json_type_lib_t* lib,
    const struct json_type_lib_sizes_t* sizes)
{
    json_type_lib_init_base(
        lib, json_type_lib_text_impl_type, sizes);

    JSON_TYPE_LIB_INIT_IMPL(text, lib->sizes.pool_size);
}

static void json_type_lib_init_from_shared_obj(
    struct json_type_lib_t* lib,
    const char* lib_name,
    const struct json_type_lib_sizes_t* sizes)
{
    json_type_lib_init_base(
        lib, json_type_lib_sobj_impl_type, sizes);

    JSON_TYPE_LIB_INIT_IMPL(sobj, lib_name);
}

static void json_type_lib_text_impl_done(
    struct json_type_lib_text_impl_t* impl)
{
    if (impl->ast != NULL)
        json_ast_destroy(impl->ast);
    if (impl->stack != NULL)
        json_type_lib_stack_destroy(impl->stack);
    pool_alloc_done(&impl->pool);
}

static void json_type_lib_sobj_impl_done(
    struct json_type_lib_sobj_impl_t* impl)
{
    if (impl->sobj != NULL)
        dlclose(impl->sobj);
}

static void json_type_lib_done(struct json_type_lib_t* lib)
{
    union json_type_lib_impl_pack_t p;

    if ((p.text = JSON_TYPE_LIB_IMPL_AS_IF(text)))
        json_type_lib_text_impl_done(p.text);
    else
    if ((p.sobj = JSON_TYPE_LIB_IMPL_AS_IF(sobj)))
        json_type_lib_sobj_impl_done(p.sobj);
    else
        UNEXPECT_VAR("%d", lib->impl.type);
}

static struct json_type_lib_t* json_type_lib_create_base()
{
    struct json_type_lib_t* lib;

    lib = malloc(sizeof(struct json_type_lib_t));
    ENSURE(lib != NULL, "malloc failed");

    return lib;
}

struct json_type_lib_t* json_type_lib_create_from_source_text(
    const struct json_type_lib_sizes_t* sizes)
{
    struct json_type_lib_t* lib;

    JSON_TYPE_LIB_SIZES_VALIDATE(sizes);

    lib = json_type_lib_create_base();
    json_type_lib_init_from_source_text(lib, sizes);

    return lib;
}

static const struct json_type_def_t* json_type_lib_sobj_load(
    struct json_type_lib_t* lib,
    struct json_type_lib_sobj_impl_t* impl);

struct json_type_lib_t* json_type_lib_create_from_shared_obj(
    const char* lib_name,
    const struct json_type_lib_sizes_t* sizes)
{
    struct json_type_lib_t* lib;

    JSON_TYPE_LIB_SIZES_VALIDATE(sizes);

    lib = json_type_lib_create_base();
    json_type_lib_init_from_shared_obj(lib, lib_name, sizes);
    json_type_lib_sobj_load(lib, JSON_TYPE_LIB_IMPL_AS(sobj));

    return lib;
}

void json_type_lib_destroy(struct json_type_lib_t* lib)
{
    json_type_lib_done(lib);
    free(lib);
}

#define JSON_TYPE_LIB_IMPL_PTR()                       \
    ({                                                 \
        void* __r;                                     \
        if (!(__r = JSON_TYPE_LIB_IMPL_AS_IF(text)) && \
            !(__r = JSON_TYPE_LIB_IMPL_AS_IF(sobj)))   \
            UNEXPECT_VAR("%d", lib->impl.type);        \
        __r;                                           \
    })

#define JSON_TYPE_LIB_SOBJ_OPER_ERROR(n)           \
    ({                                             \
        lib->error.type =                          \
            json_type_lib_error_oper;              \
        lib->error.oper.type =                     \
            json_type_lib_error_oper_ ## n;        \
        lib->error.oper.context =                  \
            json_type_lib_error_oper_sobj_context; \
        lib->error.file_info.name = NULL;          \
        lib->error.file_info.buf = NULL;           \
        lib->error.file_info.size = 0;             \
        lib->stat = json_parse_status_error;       \
    })

static enum json_parse_status_t json_type_lib_sobj_parse(
    struct json_type_lib_t* lib,
    struct json_type_lib_sobj_impl_t* impl UNUSED,
    const uchar_t* buf UNUSED,
    size_t len UNUSED)
{
    return JSON_TYPE_LIB_SOBJ_OPER_ERROR(parse);
}

static enum json_parse_status_t json_type_lib_sobj_parse_done(
    struct json_type_lib_t* lib,
    struct json_type_lib_sobj_impl_t* impl UNUSED)
{
    return JSON_TYPE_LIB_SOBJ_OPER_ERROR(parse_done);
}

enum json_parse_status_t json_type_lib_parse(
    struct json_type_lib_t* lib, const uchar_t* buf, size_t len)
{
    return lib->impl.parse(lib, JSON_TYPE_LIB_IMPL_PTR(), buf, len);
}

enum json_parse_status_t json_type_lib_parse_done(
    struct json_type_lib_t* lib)
{
    return lib->impl.parse_done(lib, JSON_TYPE_LIB_IMPL_PTR());
}

#define JSON_STATUS_IS(s) \
    (lib->stat == json_parse_status_ ## s)

#define JSON_CURRENT_STATUS() (lib->stat)

#define JSON_TYPE_LIB_RETURN(e)             \
    ({                                      \
        enum json_parse_status_t __r = (e); \
        if (__r != json_parse_status_ok)    \
            JSON_TYPE_LIB_AST_ERROR();      \
        __r;                                \
    })

static enum json_parse_status_t json_type_lib_text_do_parse(
    struct json_type_lib_t* lib,
    struct json_type_lib_text_impl_t* impl,
    const uchar_t* buf, size_t len,
    const char* lib_name)
{
    if (impl->ast == NULL) {
        impl->ast = json_ast_create(&lib->sizes.ast);

        json_ast_config_set_param(
            impl->ast,
            json_allow_literal_value_config,
            true);
    }

    if (JSON_STATUS_IS(ok))
        return JSON_TYPE_LIB_RETURN(
            json_ast_parse(impl->ast, buf, len));
    else
        return JSON_CURRENT_STATUS();
}

static enum json_parse_status_t json_type_lib_text_parse(
    struct json_type_lib_t* lib,
    struct json_type_lib_text_impl_t* impl,
    const uchar_t* buf, size_t len)
{
    return json_type_lib_text_do_parse(
        lib, impl, buf, len, NULL);
}

static enum json_parse_status_t json_type_lib_text_do_parse_done(
    struct json_type_lib_t* lib,
    struct json_type_lib_text_impl_t* impl,
    const char* lib_name)
{
    if (impl->done_parse)
        return JSON_CURRENT_STATUS();

    if (impl->ast == NULL)
        goto done;

    if (!JSON_STATUS_IS(ok))
        goto destroy;

    JSON_TYPE_LIB_RETURN(
        json_ast_parse_done(impl->ast));

destroy:
    if (impl->destroy_ast) {
        json_ast_destroy(impl->ast);
        impl->ast = NULL;
    }

done:
    impl->done_parse = true;

    return JSON_CURRENT_STATUS();
}

enum json_parse_status_t json_type_lib_text_parse_done(
    struct json_type_lib_t* lib,
    struct json_type_lib_text_impl_t* impl)
{
    return json_type_lib_text_do_parse_done(lib, impl, NULL);
}

static void json_type_lib_text_parse_buf(
    struct json_type_lib_t* lib,
    struct json_type_lib_text_impl_t* impl,
    const uchar_t* buf, size_t len,
    const char* lib_name)
{
    json_type_lib_text_do_parse(
        lib, impl,
        buf, len,
        lib_name);
    json_type_lib_text_do_parse_done(
        lib, impl,
        lib_name); 
}

static void json_type_lib_text_parse_file(
    struct json_type_lib_t* lib,
    struct json_type_lib_text_impl_t* impl,
    const char* lib_name)
{
    struct file_buf_t buf;

    STATIC(
        json_type_lib_error_file_open_context == 
             file_buf_error_file_open - 1 &&

        json_type_lib_error_file_stat_context == 
             file_buf_error_file_stat - 1 &&

        json_type_lib_error_file_read_context == 
             file_buf_error_file_read - 1 &&

        json_type_lib_error_file_close_context == 
             file_buf_error_file_close - 1);

    file_buf_init(&buf, lib_name, lib->sizes.text_max_size);

    if (buf.error_info.type != file_buf_error_none)
        JSON_TYPE_LIB_SYS_BUF_ERROR(buf);
    else
        json_type_lib_text_parse_buf(
            lib, impl,
            buf.ptr, buf.size,
            lib_name);

    file_buf_done(&buf);
}

#define JSON_TYPE_LIB_POOL() \
    (JSON_TYPE_LIB_IMPL_AS(text)->pool)

static void* json_type_lib_allocate(
    struct json_type_lib_t* lib, size_t size, size_t align)
{
    void* n;

    if ((n = pool_alloc_allocate(
            &JSON_TYPE_LIB_POOL(), size, align)) == NULL)
        fatal_error("type-lib pool alloc failed");

    memset(n, 0, size);

    return n;
}

#define JSON_TYPE_LIB_ALLOCATE_STRUCT(t) \
    (                                    \
        json_type_lib_allocate(          \
            lib, sizeof(struct t),       \
            MEM_ALIGNOF(struct t))       \
    )

static struct json_type_trie_node_t* json_type_lib_new_trie_node(
    struct json_type_lib_t* lib)
{
    return JSON_TYPE_LIB_ALLOCATE_STRUCT(json_type_trie_node_t);
}

static struct json_type_trie_t* json_type_lib_new_trie(
    struct json_type_lib_t* lib)
{
    struct json_type_trie_t* t;

    t = JSON_TYPE_LIB_ALLOCATE_STRUCT(json_type_trie_t);
    json_type_trie_init(t, json_type_lib_new_trie_node, lib);

    return t;
}

static struct json_type_object_trie_node_t* json_type_lib_new_object_trie_node(
    struct json_type_lib_t* lib)
{
    return JSON_TYPE_LIB_ALLOCATE_STRUCT(json_type_object_trie_node_t);
}

static struct json_type_object_trie_t* json_type_lib_new_object_trie(
    struct json_type_lib_t* lib)
{
    struct json_type_object_trie_t* t;

    t = JSON_TYPE_LIB_ALLOCATE_STRUCT(json_type_object_trie_t);
    json_type_object_trie_init(t, json_type_lib_new_object_trie_node, lib);

    return t;
}

static struct json_type_list_attr_t* json_type_lib_new_list_attr(
    struct json_type_lib_t* lib)
{
    return JSON_TYPE_LIB_ALLOCATE_STRUCT(json_type_list_attr_t);
}

#define JSON_TYPE_LIB_ALLOCATE_ARRAY(t, n)           \
    ({                                               \
        STATIC(TYPEOF_IS_SIZET(n));                  \
        ASSERT_SIZE_MUL_NO_OVERFLOW((n), sizeof(t)); \
        json_type_lib_allocate(lib, (n) * sizeof(t), \
            MEM_ALIGNOF(t[0]));                      \
    })

static const uchar_t* json_type_lib_new_str(
    struct json_type_lib_t* lib, const uchar_t* str)
{
    uchar_t* p;
    size_t n;

    n = strulen(str);
    ASSERT_SIZE_INC_NO_OVERFLOW(n);
    n ++;

    p = JSON_TYPE_LIB_ALLOCATE_ARRAY(uchar_t, n);
    memcpy(p, str, n);

    return p;
}

static struct json_type_node_t* json_type_lib_new_node(
    struct json_type_lib_t* lib, const struct json_text_pos_t* pos)
{
    struct json_type_node_t* n;

    n = JSON_TYPE_LIB_ALLOCATE_STRUCT(json_type_node_t);
    n->pos = *pos;

    return n;
}

static struct json_type_node_t* json_type_lib_new_any_node(
    struct json_type_lib_t* lib, const struct json_text_pos_t* pos,
    enum json_type_any_node_type_t type)
{
    struct json_type_node_t* n;

    n = json_type_lib_new_node(lib, pos);
    ASSERT(n != NULL);

    n->type = json_type_any_node_type;
    n->node.any.type = type;

    return n;
}

static struct json_type_node_t* json_type_lib_new_plain_node(
    struct json_type_lib_t* lib, const struct json_text_pos_t* pos)
{
    struct json_type_node_t* n;

    n = json_type_lib_new_node(lib, pos);
    ASSERT(n != NULL);

    n->type = json_type_plain_node_type;
    // stev: 'n->node.any' is to be set up
    // later in JSON_TYPE_LIB_NEW_PLAIN_NODE

    return n;
}

#define JSON_TYPE_LIB_NEW_PLAIN_NODE(n, v)  \
    ({                                      \
        struct json_type_node_t* __n;       \
        __n = json_type_lib_new_plain_node( \
            lib, pos);                      \
        __n->node.plain.type =              \
            json_type_plain_ ## n ## _type; \
        __n->node.plain.val.n = v;          \
        __n;                                \
    })

#define JSON_TYPE_LIB_NEW_PLAIN_NULL_NODE() \
    ({                                      \
        struct json_type_node_t* __n;       \
        __n = json_type_lib_new_plain_node( \
            lib, pos);                      \
        __n->node.plain.type =              \
            json_type_plain_null_type;      \
        __n;                                \
    })

#define JSON_TYPE_LIB_NEW_PLAIN_BOOLEAN_NODE(v) \
    JSON_TYPE_LIB_NEW_PLAIN_NODE(boolean, v)

#define JSON_TYPE_LIB_NEW_PLAIN_NUMBER_NODE(v) \
    JSON_TYPE_LIB_NEW_PLAIN_NODE(number, v)

#define JSON_TYPE_LIB_NEW_PLAIN_STRING_NODE(v) \
    JSON_TYPE_LIB_NEW_PLAIN_NODE(string, v)

static struct json_type_node_t* json_type_lib_new_object_node(
    struct json_type_lib_t* lib, const struct json_text_pos_t* pos,
    size_t size)
{
    struct json_type_object_node_arg_t* a;
    struct json_type_node_t* n;

    a = JSON_TYPE_LIB_ALLOCATE_ARRAY(
            struct json_type_object_node_arg_t,
            size);

    n = json_type_lib_new_node(lib, pos);
    ASSERT(n != NULL);

    n->type = json_type_object_node_type;
    n->node.object.size = size;
    n->node.object.args = a;
    // stev: 'n->node.object.args' to be setup immediately

    return n;
}

static struct json_type_node_t* json_type_lib_new_open_array_node(
    struct json_type_lib_t* lib, const struct json_text_pos_t* pos)
{
    struct json_type_node_t* n;

    n = json_type_lib_new_node(lib, pos);
    ASSERT(n != NULL);

    n->type = json_type_array_node_type;
    n->node.array.type = json_type_open_array_node_type;
    // stev: 'n->node.array.open.arg' to be setup immediately

    return n;
}

static struct json_type_node_t* json_type_lib_new_closed_array_node(
    struct json_type_lib_t* lib, const struct json_text_pos_t* pos,
    size_t size)
{
    const struct json_type_node_t** a;
    struct json_type_node_t* n;

    ASSERT_SIZE_INC_NO_OVERFLOW(size);
    a = JSON_TYPE_LIB_ALLOCATE_ARRAY(
            struct json_type_node_t*, size + 1);
    a[size] = NULL;

    n = json_type_lib_new_node(lib, pos);
    ASSERT(n != NULL);

    n->type = json_type_array_node_type;
    n->node.array.type = json_type_closed_array_node_type;
    n->node.array.val.closed.size = size;
    n->node.array.val.closed.args = a;
    // stev: 'n->node.array.val.closed.args' to be setup immediately

    return n;
}

static struct json_type_node_t* json_type_lib_new_list_node(
    struct json_type_lib_t* lib, const struct json_text_pos_t* pos,
    size_t size)
{
    const struct json_type_node_t** a;
    struct json_type_node_t* n;

    ASSERT_SIZE_INC_NO_OVERFLOW(size);
    a = JSON_TYPE_LIB_ALLOCATE_ARRAY(
            struct json_type_node_t*, size + 1);
    a[size] = NULL;

    n = json_type_lib_new_node(lib, pos);
    ASSERT(n != NULL);

    n->type = json_type_list_node_type;
    n->node.list.size = size;
    n->node.list.args = a;
    // stev: 'n->node.list.args' to be setup immediately

    return n;
}

static struct json_type_defs_t* json_type_lib_new_defs(
    struct json_type_lib_t* lib, const struct json_text_pos_t* pos,
    size_t size)
{
    struct json_type_defs_arg_t* a;
    struct json_type_defs_t* d;

    a = JSON_TYPE_LIB_ALLOCATE_ARRAY(
            struct json_type_defs_arg_t,
            size);

    d = JSON_TYPE_LIB_ALLOCATE_STRUCT(json_type_defs_t);
    ASSERT(d != NULL);

    d->pos = *pos;
    d->size = size;
    d->args = a;
    // stev: 'd->args' to be setup immediately

    return d;
}

static void json_type_lib_rules_basic_type(
    struct json_type_lib_t* lib,
    const struct json_text_pos_t* pos,
    enum json_type_ruler_basic_type_t val)
{
    const struct json_type_node_t* n;

    STATIC(
        json_type_any_type_type + 0 ==
        json_type_ruler_basic_type_type &&

        json_type_any_null_type + 0 ==
        json_type_ruler_basic_null_type &&

        json_type_any_boolean_type + 0 ==
        json_type_ruler_basic_boolean_type &&

        json_type_any_number_type + 0 ==
        json_type_ruler_basic_number_type &&

        json_type_any_string_type + 0 ==
        json_type_ruler_basic_string_type &&

        json_type_any_object_type + 0 ==
        json_type_ruler_basic_object_type &&

        json_type_any_array_type + 0 ==
        json_type_ruler_basic_array_type);

    n = json_type_lib_new_any_node(lib, pos, val);

    JSON_TYPE_LIB_STACK_PUSH_NODE(n);
}

static void json_type_lib_rules_plain_null(
    struct json_type_lib_t* lib,
    const struct json_text_pos_t* pos)
{
    const struct json_type_node_t* n;

    n = JSON_TYPE_LIB_NEW_PLAIN_NULL_NODE();

    JSON_TYPE_LIB_STACK_PUSH_NODE(n);
}

static void json_type_lib_rules_plain_bool(
    struct json_type_lib_t* lib,
    const struct json_text_pos_t* pos,
    bool val)
{
    const struct json_type_node_t* n;

    n = JSON_TYPE_LIB_NEW_PLAIN_BOOLEAN_NODE(val);

    JSON_TYPE_LIB_STACK_PUSH_NODE(n);
}

static void json_type_lib_rules_plain_num(
    struct json_type_lib_t* lib,
    const struct json_text_pos_t* pos,
    const uchar_t* val)
{
    const struct json_type_node_t* n;

    n = JSON_TYPE_LIB_NEW_PLAIN_NUMBER_NODE(val);

    JSON_TYPE_LIB_STACK_PUSH_NODE(n);
}

static void json_type_lib_rules_plain_str(
    struct json_type_lib_t* lib,
    const struct json_text_pos_t* pos,
    const uchar_t* val)
{
    const struct json_type_node_t* n;

    n = JSON_TYPE_LIB_NEW_PLAIN_STRING_NODE(val);

    JSON_TYPE_LIB_STACK_PUSH_NODE(n);
}

static void json_type_lib_rules_arg_name(
    struct json_type_lib_t* lib,
    const struct json_text_pos_t* pos,
    const uchar_t* val)
{
    struct json_type_object_node_arg_t a;

    a.type = JSON_TYPE_LIB_STACK_POP_AS_NODE();
    a.name = json_type_lib_new_str(lib, val);
    a.pos = *pos;

    JSON_TYPE_LIB_STACK_PUSH_ARG(a);
}

static void json_type_lib_rules_args_object(
    struct json_type_lib_t* lib,
    const struct json_text_pos_t* pos,
    size_t size)
{
    struct json_type_node_t* n;

    n = json_type_lib_new_object_node(lib, pos, size);
    if (size > 0) {
        struct json_type_lib_obj_t *b, *p;
        struct json_type_object_node_t* o;
        struct json_type_object_node_arg_t* q;

        o = JSON_TYPE_NODE_AS(n, object);

        ASSERT(o->size == size);

        ASSERT(JSON_TYPE_LIB_STACK_SIZE() >= size);
        for (b = JSON_TYPE_LIB_STACK_TOP_REF(),
             p = b - SIZE_AS_INT(size - 1),
             q = CONST_CAST(o->args,
                struct json_type_object_node_arg_t);
             p <= b;
             p ++,
             q ++) {
             *q = JSON_TYPE_LIB_OBJ_AS_ARG(p);
        }
        JSON_TYPE_LIB_STACK_POP_N(size - 1);
    }

    JSON_TYPE_LIB_STACK_PUSH_NODE(n);
}

static void json_type_lib_rules_mono_array(
    struct json_type_lib_t* lib,
    const struct json_text_pos_t* pos)
{
    struct json_type_node_t* n;
    struct json_type_array_node_t* a;
    struct json_type_open_array_node_t* o;

    n = json_type_lib_new_open_array_node(lib, pos);

    a = JSON_TYPE_NODE_AS(n, array);
    o = JSON_TYPE_ARRAY_NODE_AS(a, open);

    o->arg = JSON_TYPE_LIB_STACK_POP_AS_NODE();

    JSON_TYPE_LIB_STACK_PUSH_NODE(n);
}

static void json_type_lib_rules_args_array(
    struct json_type_lib_t* lib,
    const struct json_text_pos_t* pos,
    size_t size)
{
    struct json_type_node_t* n;

    n = json_type_lib_new_closed_array_node(lib, pos, size);
    if (size > 0) {
        struct json_type_lib_obj_t *b, *p;
        const struct json_type_node_t** q;
        struct json_type_array_node_t* a;
        struct json_type_closed_array_node_t* c;

        a = JSON_TYPE_NODE_AS(n, array);
        c = JSON_TYPE_ARRAY_NODE_AS(a, closed);

        ASSERT(c->size == size);

        ASSERT(JSON_TYPE_LIB_STACK_SIZE() >= size);
        for (b = JSON_TYPE_LIB_STACK_TOP_REF(),
             p = b - SIZE_AS_INT(size - 1),
             q = a->val.closed.args;
             p <= b;
             p ++,
             q ++) {
             *q = JSON_TYPE_LIB_OBJ_AS_NODE(p);
        }
        JSON_TYPE_LIB_STACK_POP_N(size - 1);
    }

    JSON_TYPE_LIB_STACK_PUSH_NODE(n);
}

static void json_type_lib_rules_args_list(
    struct json_type_lib_t* lib,
    const struct json_text_pos_t* pos,
    size_t size)
{
    struct json_type_node_t* n;

    n = json_type_lib_new_list_node(lib, pos, size);
    if (size > 0) {
        struct json_type_lib_obj_t *b, *p;
        struct json_type_list_node_t* l;
        const struct json_type_node_t** q;

        l = JSON_TYPE_NODE_AS(n, list);

        ASSERT(l->size == size);

        ASSERT(JSON_TYPE_LIB_STACK_SIZE() >= size);
        for (b = JSON_TYPE_LIB_STACK_TOP_REF(),
             p = b - SIZE_AS_INT(size - 1),
             q = l->args;
             p <= b;
             p ++,
             q ++) {
             *q = JSON_TYPE_LIB_OBJ_AS_NODE(p);
        }
        JSON_TYPE_LIB_STACK_POP_N(size - 1);
    }

    JSON_TYPE_LIB_STACK_PUSH_NODE(n);
}

static void json_type_lib_rules_args_top(
    struct json_type_lib_t* lib,
    const struct json_text_pos_t* pos,
    size_t size)
{
    struct json_type_defs_t* d;
    struct json_type_lib_obj_t *b, *p;
    struct json_type_defs_arg_t* q;

    ASSERT(size > 0);

    d = json_type_lib_new_defs(lib, pos, size);
    ASSERT(d->size == size);

    ASSERT(JSON_TYPE_LIB_STACK_SIZE() >= size);
    for (b = JSON_TYPE_LIB_STACK_TOP_REF(),
         p = b - SIZE_AS_INT(size - 1),
         q = CONST_CAST(d->args,
            struct json_type_defs_arg_t);
         p <= b;
         p ++,
         q ++) {
         *q = JSON_TYPE_LIB_OBJ_AS_ARG(p);
    }
    JSON_TYPE_LIB_STACK_POP_N(size - 1);

    JSON_TYPE_LIB_STACK_PUSH_DEFS(d);
}

static const struct json_type_ruler_rules_t json_type_lib_rules = {
    .basic_type_rule  = (json_type_ruler_basic_rule_t)
                         json_type_lib_rules_basic_type,

    .plain_null_rule  = (json_type_ruler_empty_rule_t)
                         json_type_lib_rules_plain_null,

    .plain_bool_rule  = (json_type_ruler_boolean_rule_t)
                         json_type_lib_rules_plain_bool,

    .plain_num_rule   = (json_type_ruler_string_rule_t)
                         json_type_lib_rules_plain_num,

    .plain_str_rule   = (json_type_ruler_string_rule_t)
                         json_type_lib_rules_plain_str,

    .arg_name_rule    = (json_type_ruler_string_rule_t)
                         json_type_lib_rules_arg_name,

    .args_object_rule = (json_type_ruler_size_rule_t)
                         json_type_lib_rules_args_object,

    .mono_array_rule  = (json_type_ruler_empty_rule_t)
                         json_type_lib_rules_mono_array,

    .args_array_rule  = (json_type_ruler_size_rule_t)
                         json_type_lib_rules_args_array,

    .args_list_rule   = (json_type_ruler_size_rule_t)
                         json_type_lib_rules_args_list,

    .args_top_rule    = (json_type_ruler_size_rule_t)
                         json_type_lib_rules_args_top,
};

#undef  CASE
#define CASE(E) case json_type_lib_error_attr_ ## E

static void json_type_lib_attr_print_error_desc(
    const struct json_type_lib_error_attr_t* attr, FILE* file)
{
    static const char* const names[] = {
        [0] = "any", [1] = "plain", [2] = "object",
        [3] = "open array", [4] = "closed array"
    };
    static const char* const aggrs[][2] = {
        [0] = {"object", "object"},
        [1] = {"object", "array"},
        [2] = {"object", "list"},
        [3] = {"array", "object"},
        [4] = {"array", "array"},
        [5] = {"array", "list"}
    };
    size_t k = 0;

    switch (attr->type) {
    CASE(invalid_list_dup_any):
        k ++;
    CASE(invalid_list_dup_plain):
        k ++;
    CASE(invalid_list_dup_object):
        k ++;
    CASE(invalid_list_dup_open_array):
        k ++;
    CASE(invalid_list_dup_closed_array): {
        k = SIZE_SUB(SZ(4), k);
        fprintf(file, "invalid \"list\" type object: "
            "duplicated '%s' entry (previous defined at %zu:%zu)",
            ARRAY_NON_NULL_ELEM(names, k),
            attr->pos[1].line,
            attr->pos[1].col);
        break;
    }
    CASE(invalid_list_arrays_ambiguity):
        fprintf(file, "invalid \"list\" type object: "
            "array ambiguity (the other defined at %zu:%zu)",
            attr->pos[1].line,
            attr->pos[1].col);
        break;
    CASE(invalid_list_object_ambiguity):
        k += 3;
    CASE(invalid_list_array_ambiguity): {
        const char* const* p;

        k = SIZE_SUB(SZ(3), k);
        p = ARRAY_NULL_ELEM(aggrs, k);
        ASSERT(p != NULL);

        fprintf(file, "invalid \"list\" type object: "
            "%s argument ambiguity (the other is at %zu:%zu)",
            p[0], attr->pos[1].line, attr->pos[1].col);
        break;
    }
    CASE(invalid_list_sort_of_object_of_object_not_supported):
        k ++;
    CASE(invalid_list_sort_of_object_of_array_not_supported):
        k ++;
    CASE(invalid_list_sort_of_object_of_list_not_supported):
        k ++;
    CASE(invalid_list_sort_of_array_of_object_not_supported):
        k ++;
    CASE(invalid_list_sort_of_array_of_array_not_supported):
        k ++;
    CASE(invalid_list_sort_of_array_of_list_not_supported): {
        const char* const* p;

        k = SIZE_SUB(SZ(5), k);
        p = ARRAY_NULL_ELEM(aggrs, k);
        ASSERT(p != NULL);

        fprintf(file, "invalid \"list\" type object: "
            "sort of %s of %s type not yet supported "
            "(the %sinner %s is at %zu:%zu)",
            p[0], p[1], k != 2 && k != 5
            ? "other " : "", p[1],
            attr->pos[1].line,
            attr->pos[1].col);
        break;
    }
    CASE(invalid_list_elem_is_list):
        fprintf(file, "invalid \"list\" type object: "
            "element is a \"list\" (the outer \"list\""
            " begins at %zu:%zu)",
            attr->pos[1].line,
            attr->pos[1].col);
        break;
    CASE(invalid_defs_dup_name):
        fprintf(file, "invalid array of \"name\" objects: "
            "duplicated type name (previous defined at %zu:%zu)",
            attr->pos[1].line,
            attr->pos[1].col);
        break;
    default:
        UNEXPECT_VAR("%d", attr->type);
    }
}

static void json_type_lib_error_info_print_error_desc(
    const struct json_type_lib_error_info_t* info, FILE* file)
{
    switch (info->type) {
    case json_type_lib_error_none:
        fputc('-', file);
        break;
    case json_type_lib_error_oper:
        fputs("operation error: ", file);
        json_type_lib_error_oper_print_desc(
            &info->oper, file);
        break;
    case json_type_lib_error_sys:
        fputs("system error: ", file);
        if (info->sys.error)
            fprintf(file, "%s: %s",
                json_type_lib_error_sys_context_get_desc(
                    info->sys.context),
                strerror(info->sys.error));
        else
            fprintf(file, "%s",
                json_type_lib_error_sys_context_get_desc(
                    info->sys.context));
        break;
    case json_type_lib_error_lib:
        fputs("library error: ", file);
        if (info->lib.type == json_type_lib_error_generic_lib)
            json_type_lib_generic_lib_print_error_desc(
                info->lib.lib.generic,
                info->lib.name,
                file);
        else
        if (info->lib.type == json_type_lib_error_shared_lib)
            fputs(json_type_lib_shared_lib_error_type_get_desc(
                info->lib.lib.shared), file);
        else
            UNEXPECT_VAR("%d", info->lib.type);
        break;
    case json_type_lib_error_ast:
        fprintf(file, "parse error: %s",
            json_error_info_get_desc(&info->ast));
        break;
    case json_type_lib_error_meta:
        fputs("meta error: ", file);
        json_type_ruler_print_error_desc(
            info->meta.type, file);
        break;
    case json_type_lib_error_attr:
        fputs("attribute error: ", file);
        json_type_lib_attr_print_error_desc(
            &info->attr, file);
        break;
    default:
        UNEXPECT_VAR("%d", info->type);
    }
}

#ifdef JSON_DEBUG

#undef  CASE
#define CASE(E) case json_type_lib_error_ ## E ## _context: return #E

static const char* json_type_lib_error_sys_context_get_name(
    enum json_type_lib_error_sys_context_t context)
{
    switch (context) {
    CASE(file_open);
    CASE(file_stat);
    CASE(file_read);
    CASE(file_close);
    default:
        UNEXPECT_VAR("%d", context);
    }
}

#undef  CASE
#define CASE(E) case json_type_lib_shared_lib_error_ ## E: return #E

static const char* json_type_lib_shared_lib_error_type_get_name(
    enum json_type_lib_shared_lib_error_type_t type)
{
    switch (type) {
    CASE(invalid_name);
    CASE(load_lib_failed);
    CASE(get_lib_version_sym_not_found);
    CASE(get_type_def_sym_not_found);
    CASE(wrong_lib_version);
    CASE(def_is_null);
    CASE(def_not_valid);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

#undef  CASE
#define CASE(E) case json_type_lib_error_attr_ ## E: return #E

static const char* json_type_lib_attr_error_type_get_name(
    enum json_type_lib_error_attr_type_t type)
{
    switch (type) {
    CASE(invalid_list_dup_any);
    CASE(invalid_list_dup_plain);
    CASE(invalid_list_dup_object);
    CASE(invalid_list_dup_open_array);
    CASE(invalid_list_dup_closed_array);
    CASE(invalid_list_arrays_ambiguity);
    CASE(invalid_list_object_ambiguity);
    CASE(invalid_list_array_ambiguity);
    CASE(invalid_list_sort_of_object_of_object_not_supported);
    CASE(invalid_list_sort_of_object_of_array_not_supported);
    CASE(invalid_list_sort_of_array_of_object_not_supported);
    CASE(invalid_list_sort_of_array_of_array_not_supported);
    CASE(invalid_list_elem_is_list);
    CASE(invalid_defs_dup_name);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

static void json_type_lib_error_info_print_error_debug(
    const struct json_type_lib_error_info_t* info, FILE* file)
{
    switch (info->type) {
    case json_type_lib_error_none:
        fputs("# none {}", file);
        break;
    case json_type_lib_error_oper:
        fputs("# oper {", file);
        json_type_lib_error_oper_print_debug(
            &info->oper, file);
        fputc('}', file);
        break;
    case json_type_lib_error_sys:
        fprintf(file, "# sys {.context=%s .error=%d \"%s\"}",
            json_type_lib_error_sys_context_get_name(
                info->sys.context),
            info->sys.error, strerror(info->sys.error));
        break;
    case json_type_lib_error_lib:
        fprintf(file, "# lib {.type=%s}",
            json_type_lib_shared_lib_error_type_get_name(
                info->lib.type));
        break;
    case json_type_lib_error_ast:
        fputs("# ast {", file);
        json_error_info_print_error_debug(
            &info->ast, file);
        fputc('}', file);
        break;
    case json_type_lib_error_meta:
        fprintf(file, "# meta {.type=%s .pos={.line=%zu .col=%zu}}",
            json_type_ruler_error_type_get_name(info->meta.type),
            info->meta.pos.line, info->meta.pos.col);
        break;
    case json_type_lib_error_attr:
        fprintf(file,
            "# attr {.type=%s .pos=[{.line=%zu .col=%zu} {.line=%zu .col=%zu}]}",
            json_type_lib_attr_error_type_get_name(info->attr.type),
            info->attr.pos[0].line, info->attr.pos[0].col,
            info->attr.pos[1].line, info->attr.pos[1].col);
        break;
    default:
        UNEXPECT_VAR("%d", info->type);
    }
}

#endif

bool json_type_lib_get_is_error(struct json_type_lib_t* lib)
{
    return lib->error.type != json_type_lib_error_none;
}

struct json_error_pos_t json_type_lib_get_error_pos(
    struct json_type_lib_t* lib)
{
    return json_type_lib_error_info_get_pos(&lib->error);
}

const struct json_file_info_t* json_type_lib_get_error_file(
    struct json_type_lib_t* lib)
{
    return json_type_lib_error_info_get_file(&lib->error);
}

void json_type_lib_print_error_desc(struct json_type_lib_t* lib,
    FILE* file)
{
    json_type_lib_error_info_print_error_desc(&lib->error, file);
}

#ifdef JSON_DEBUG

void json_type_lib_print_error_debug(struct json_type_lib_t* lib,
    FILE* file)
{
    json_type_lib_error_info_print_error_debug(&lib->error, file);
}

#endif

#undef  JSON_TYPE_LIB_META_ERROR
#define JSON_TYPE_LIB_META_ERROR(m)          \
    ({                                       \
        lib->error.type =                    \
            json_type_lib_error_meta;        \
        lib->error.meta = m;                 \
        lib->error.file_info.name = NULL;    \
        lib->error.file_info.buf = NULL;     \
        lib->error.file_info.size = 0;       \
        lib->stat = json_parse_status_error; \
        false;                               \
    })

#define JSON_TYPE_LIB_ATTR_ERROR(n, p0, p1)  \
    ({                                       \
        lib->error.type =                    \
            json_type_lib_error_attr;        \
        lib->error.attr.type =               \
            json_type_lib_error_attr_ ## n;  \
        lib->error.attr.pos[0] = p0;         \
        lib->error.attr.pos[1] = p1;         \
        lib->error.file_info.name = NULL;    \
        lib->error.file_info.buf = NULL;     \
        lib->error.file_info.size = 0;       \
        lib->stat = json_parse_status_error; \
        false;                               \
    })

#define JSON_TYPE_LIB_NONE_ERROR()        \
    ({                                    \
        lib->error.type =                 \
            json_type_lib_error_none;     \
        lib->error.file_info.name = NULL; \
        lib->error.file_info.buf = NULL;  \
        lib->error.file_info.size = 0;    \
        true;                             \
    })

static bool json_type_lib_text_validate_base(
    struct json_type_lib_t* lib,
    struct json_type_lib_text_impl_t* impl,
    bool valid_only)
{
    struct json_type_ruler_t t;
    struct json_type_lib_error_meta_t e;
    bool r;

    if (!JSON_STATUS_IS(ok) ||
        !impl->done_parse ||
        impl->ast == NULL)
        return false;

    json_type_ruler_init(&t,
        !valid_only ? &json_type_lib_rules : NULL,
        !valid_only ? lib : NULL);
    r = json_type_ruler_eval(&t, impl->ast, &e);
    json_type_ruler_done(&t);

    if (!r)
        return JSON_TYPE_LIB_META_ERROR(e);
    else
        return JSON_TYPE_LIB_NONE_ERROR();
}

static bool json_type_lib_text_validate(
    struct json_type_lib_t* lib,
    struct json_type_lib_text_impl_t* impl)
{
    return json_type_lib_text_validate_base(lib, impl, true);
}

bool json_type_lib_validate(struct json_type_lib_t* lib)
{
    return lib->impl.validate(lib, JSON_TYPE_LIB_IMPL_PTR());
}

#ifdef JSON_DEBUG
#define PRINT_DEBUG_NODE_POS(n)             \
    do {                                    \
        PRINT_DEBUG_STR(" {.node=");        \
        json_type_node_print(n, stderr);    \
        PRINT_DEBUG_FMT(" .pos={%zu %zu}}", \
            n->pos.line, n->pos.col);       \
    } while (0)
#else
#define PRINT_DEBUG_NODE_POS(n) \
    do {} while (0)
#endif

#define JSON_TYPE_LIB_LIST_ATTR_ERROR(e, n0, n1) \
    ({                                           \
        PRINT_DEBUG_BEGIN(                       \
            "error=%s", "invalid_list_" #e);     \
        PRINT_DEBUG_NODE_POS(n0);                \
        PRINT_DEBUG_NODE_POS(n1);                \
        PRINT_DEBUG_END();                       \
        JSON_TYPE_LIB_ATTR_ERROR(                \
            invalid_list_ ## e,                  \
            n0->pos, n1->pos);                   \
    })

static bool json_type_lib_check_match_sym(
    const struct json_type_node_t* sym,
    const struct json_type_trie_node_t* node)
{
    ASSERT(node != NULL);
    return
        node->sym != NULL &&
        json_type_node_match(node->sym, sym);
}

static bool json_type_lib_check_arrays_ambiguities(
    struct json_type_lib_t* lib,
    const struct json_type_trie_t* open,
    const struct json_type_trie_t* closed)
{
    const struct json_type_trie_node_t* t;
    struct json_type_trie_sib_iterator_t i;
    const struct json_type_trie_node_t* k;
    const struct json_type_node_t* m;

    ASSERT(!JSON_TYPE_TRIE_IS_EMPTY(open));
    ASSERT(!JSON_TYPE_TRIE_IS_EMPTY(closed));

    // stev: we're looking up for the following kind of
    // ambiguities between elements of "list" types:
    //   `{ "type": "list", "args": [ ... $open ... $closed ... ] }'
    // where:
    //   $open is `{ "type": "array", "args": $type }'
    //   $closed is `{ "type": "array", "args": [ $args ] }'
    // where:
    //   $type is any type and $args is a list of zero
    //   or more types, each type matching $type modulo
    //   'json_type_node_match'

    json_type_trie_sib_iterator_init(&i, open->root, 0);

    while (!json_type_trie_sib_iterator_at_end(&i)) {
        k = json_type_trie_sib_iterator_deref(&i);

        ASSERT(k != NULL);
#ifdef JSON_DEBUG
        PRINT_DEBUG_BEGIN("k=");
        json_type_trie_print_node(k, stderr);
        PRINT_DEBUG_END();
#endif
        t = NULL;
        if (json_type_trie_match_sym(
                closed,
                (json_type_trie_node_cond_func_t)
                 json_type_lib_check_match_sym,
                k->sym,
                &t)) {
            ASSERT(t != NULL);

            m = JSON_TYPE_TRIE_NODE_AS_VAL(t);
            json_type_trie_sib_iterator_done(&i);

            return JSON_TYPE_LIB_LIST_ATTR_ERROR(
                arrays_ambiguity, k->sym, m);
        }

        json_type_trie_sib_iterator_inc(&i);
    }

    json_type_trie_sib_iterator_done(&i);

    return true;
}

static bool json_type_check_match_object_sym(
    const struct json_type_node_t* sym,
    const struct json_type_object_trie_node_t* node)
{
    const struct json_type_node_t* p;

    ASSERT(node != NULL);
    ASSERT(!JSON_TYPE_OBJECT_TRIE_SYM_IS(node->sym, name));
    return
        (p = JSON_TYPE_OBJECT_TRIE_SYM_AS_IF(node->sym, node)) &&
        (p != sym) && json_type_node_match(p, sym);
}

static bool json_type_lib_check_object_ambiguities(
    struct json_type_lib_t* lib,
    const struct json_type_object_trie_t* object)
{
    struct json_type_object_trie_lvl_iterator_t l;
    struct json_type_object_trie_sib_iterator_t s;
    const struct json_type_object_trie_node_t *t, *u, *p;
    const struct json_type_node_t *v, *q;

    ASSERT(!JSON_TYPE_OBJECT_TRIE_IS_EMPTY(object));

    json_type_object_trie_lvl_iterator_init(&l, object, 0);

    while (!json_type_object_trie_lvl_iterator_at_end(&l)) {
        t = json_type_object_trie_lvl_iterator_deref(&l);
        ASSERT(t != NULL);

#ifdef JSON_DEBUG
        PRINT_DEBUG_BEGIN("t=");
        json_type_object_trie_print_node(t, stderr);
        PRINT_DEBUG_END();
#endif
        json_type_object_trie_sib_iterator_init(&s, t, 0);

        while (!json_type_object_trie_sib_iterator_at_end(&s)) {
            u = json_type_object_trie_sib_iterator_deref(&s);
            ASSERT(u != NULL);

#ifdef JSON_DEBUG
            PRINT_DEBUG_BEGIN("u=");
            json_type_object_trie_print_node(u, stderr);
            PRINT_DEBUG_END();
#endif
            p = NULL;
            if ((v = JSON_TYPE_OBJECT_TRIE_SYM_AS_IF(u->sym, node)) && 
                json_type_object_trie_lookup_first_node(
                    t, (json_type_object_trie_node_cond_func_t)
                        json_type_check_match_object_sym,
                    v, &p)) {
                ASSERT(p != NULL);
                q = JSON_TYPE_OBJECT_TRIE_SYM_AS(p->sym, node);

                json_type_object_trie_sib_iterator_done(&s);
                json_type_object_trie_lvl_iterator_done(&l);

                return JSON_TYPE_LIB_LIST_ATTR_ERROR(
                    object_ambiguity, q, v);
            }

            json_type_object_trie_sib_iterator_inc(&s);
        }

        json_type_object_trie_sib_iterator_done(&s);
        json_type_object_trie_lvl_iterator_inc(&l);
    }

    json_type_object_trie_lvl_iterator_done(&l);

    return true;
}

static bool json_type_check_match_array_sym(
    const struct json_type_node_t* sym,
    const struct json_type_trie_node_t* node)
{
    ASSERT(node != NULL);
    return
        node->sym != NULL && node->sym != sym &&
        json_type_node_match(node->sym, sym);
}

static bool json_type_lib_check_array_ambiguities(
    struct json_type_lib_t* lib,
    const struct json_type_trie_t* array)
{
    struct json_type_trie_lvl_iterator_t l;
    struct json_type_trie_sib_iterator_t s;
    const struct json_type_trie_node_t *t, *u, *p;

    ASSERT(!JSON_TYPE_TRIE_IS_EMPTY(array));

    json_type_trie_lvl_iterator_init(&l, array, 0);

    while (!json_type_trie_lvl_iterator_at_end(&l)) {
        t = json_type_trie_lvl_iterator_deref(&l);
        ASSERT(t != NULL);

#ifdef JSON_DEBUG
        PRINT_DEBUG_BEGIN("t=");
        json_type_trie_print_node(t, stderr);
        PRINT_DEBUG_END();
#endif
        json_type_trie_sib_iterator_init(&s, t, 0);

        while (!json_type_trie_sib_iterator_at_end(&s)) {
            u = json_type_trie_sib_iterator_deref(&s);
            ASSERT(u != NULL);

#ifdef JSON_DEBUG
            PRINT_DEBUG_BEGIN("u=");
            json_type_trie_print_node(u, stderr);
            PRINT_DEBUG_END();
#endif
            p = NULL;
            if (u->sym != NULL && 
                json_type_trie_lookup_first_node(
                    t, (json_type_trie_node_cond_func_t)
                        json_type_check_match_array_sym,
                    u->sym, &p)) {
                ASSERT(p != NULL);
                ASSERT(p->sym != NULL);

                json_type_trie_sib_iterator_done(&s);
                json_type_trie_lvl_iterator_done(&l);

                return JSON_TYPE_LIB_LIST_ATTR_ERROR(
                    array_ambiguity, p->sym, u->sym);
            }

            json_type_trie_sib_iterator_inc(&s);
        }

        json_type_trie_sib_iterator_done(&s);
        json_type_trie_lvl_iterator_inc(&l);
    }

    json_type_trie_lvl_iterator_done(&l);

    return true;
}

#define JSON_TYPE_LIB_CHECK_IS_SYM_FUNC(t)                    \
static bool json_type_lib_check_is_ ## t ## _sym(             \
    const void* obj UNUSED,                                   \
    const struct json_type_trie_node_t* node)                 \
{                                                             \
    const struct json_type_any_node_t* a;                     \
                                                              \
    ASSERT(node != NULL);                                     \
    return                                                    \
        node->sym != NULL                                     \
        && (((a = JSON_TYPE_NODE_AS_IF_CONST(node->sym, any)) \
                && JSON_TYPE_ANY_NODE_IS_CONST(a, t))         \
            || JSON_TYPE_NODE_IS_CONST(node->sym, t));        \
}

JSON_TYPE_LIB_CHECK_IS_SYM_FUNC(object)
JSON_TYPE_LIB_CHECK_IS_SYM_FUNC(array)

#define JSON_TYPE_LIB_CHECK_ARRAYS_NOT_SUPP(t, p, q)  \
    ({                                                \
        bool __r;                                     \
        if ((__r = (p = q = NULL,                     \
            json_type_trie_lookup_first_node(         \
                open->root,                           \
                json_type_lib_check_is_ ## t ## _sym, \
                NULL,                                 \
                &p) &&                                \
            json_type_trie_lookup_first_node(         \
                closed->root,                         \
                json_type_lib_check_is_ ## t ## _sym, \
                NULL,                                 \
                &q)))) {                              \
            ASSERT(p != NULL);                        \
            ASSERT(q != NULL);                        \
            ASSERT(p->sym != NULL);                   \
            ASSERT(q->sym != NULL);                   \
        }                                             \
        __r;                                          \
    })

#define JSON_TYPE_LIB_LIST_ATTR_NOT_SUPP_ERROR(e, n0, n1) \
    JSON_TYPE_LIB_LIST_ATTR_ERROR(e ## _not_supported, n0, n1)

static bool json_type_lib_check_arrays_not_supported(
    struct json_type_lib_t* lib,
    const struct json_type_trie_t* open,
    const struct json_type_trie_t* closed)
{
    const struct json_type_trie_node_t *t, *u;

    ASSERT(!JSON_TYPE_TRIE_IS_EMPTY(open));
    ASSERT(!JSON_TYPE_TRIE_IS_EMPTY(closed));

    if (JSON_TYPE_LIB_CHECK_ARRAYS_NOT_SUPP(object, t, u))
        return JSON_TYPE_LIB_LIST_ATTR_NOT_SUPP_ERROR(
            sort_of_array_of_object, t->sym, u->sym);

    if (JSON_TYPE_LIB_CHECK_ARRAYS_NOT_SUPP(array, t, u))
        return JSON_TYPE_LIB_LIST_ATTR_NOT_SUPP_ERROR(
            sort_of_array_of_array, t->sym, u->sym);

    return true;
}

static bool json_type_lib_check_object_not_supported(
    struct json_type_lib_t* lib,
    const struct json_type_object_trie_t* object)
{
    struct json_type_object_trie_lvl_iterator_t l;
    struct json_type_object_trie_sib_iterator_t s;
    const struct json_type_object_trie_node_t *t, *u, *w;
    const struct json_type_node_t *p, *q, *v, *m;

    ASSERT(!JSON_TYPE_OBJECT_TRIE_IS_EMPTY(object));

    json_type_object_trie_lvl_iterator_init(&l, object, 0);

    while (!json_type_object_trie_lvl_iterator_at_end(&l)) {
        t = json_type_object_trie_lvl_iterator_deref(&l);

        ASSERT(t != NULL);
#ifdef JSON_DEBUG
        PRINT_DEBUG_BEGIN("t=");
        json_type_object_trie_print_node(t, stderr);
        PRINT_DEBUG_END();
#endif
        p = q = NULL;
        json_type_object_trie_sib_iterator_init(&s, t, 0);

        while (!json_type_object_trie_sib_iterator_at_end(&s)) {
            u = json_type_object_trie_sib_iterator_deref(&s);

#ifdef JSON_DEBUG
            PRINT_DEBUG_BEGIN("u=");
            json_type_object_trie_print_node(u, stderr);
            PRINT_DEBUG_END();
#endif
            if ((v = JSON_TYPE_OBJECT_TRIE_SYM_AS_IF(u->sym, node))) {
                if (v != p && JSON_TYPE_NODE_IS_CONST(v, object)) {
                    if (p != NULL) {
                        json_type_object_trie_sib_iterator_done(&s);
                        json_type_object_trie_lvl_iterator_done(&l);

                        return JSON_TYPE_LIB_LIST_ATTR_NOT_SUPP_ERROR(
                            sort_of_object_of_object, p, v);
                    }
                    p = v;
                }
                if (v != q && JSON_TYPE_NODE_IS_CONST(v, array)) {
                    if (q != NULL) {
                        json_type_object_trie_sib_iterator_done(&s);
                        json_type_object_trie_lvl_iterator_done(&l);

                        return JSON_TYPE_LIB_LIST_ATTR_NOT_SUPP_ERROR(
                            sort_of_object_of_array, q, v);
                    }
                    q = v;
                }
                if (JSON_TYPE_NODE_IS_CONST(v, list)) {
                    w = json_type_object_trie_node_get_leaf(u);

                    ASSERT(w != NULL);
                    m = JSON_TYPE_OBJECT_TRIE_NODE_AS_VAL(w);

                    json_type_object_trie_sib_iterator_done(&s);
                    json_type_object_trie_lvl_iterator_done(&l);

                    return JSON_TYPE_LIB_LIST_ATTR_NOT_SUPP_ERROR(
                        sort_of_object_of_list, m, v);
                }
            }

            json_type_object_trie_sib_iterator_inc(&s);
        }

        json_type_object_trie_sib_iterator_done(&s);
        json_type_object_trie_lvl_iterator_inc(&l);
    }

    json_type_object_trie_lvl_iterator_done(&l);

    return true;
}

static bool json_type_lib_check_array_not_supported(
    struct json_type_lib_t* lib,
    const struct json_type_trie_t* array)
{
    struct json_type_trie_lvl_iterator_t l;
    struct json_type_trie_sib_iterator_t s;
    const struct json_type_trie_node_t *t, *u, *w;
    const struct json_type_node_t *p, *q, *m;

    ASSERT(!JSON_TYPE_TRIE_IS_EMPTY(array));

    json_type_trie_lvl_iterator_init(&l, array, 0);

    while (!json_type_trie_lvl_iterator_at_end(&l)) {
        t = json_type_trie_lvl_iterator_deref(&l);
        ASSERT(t != NULL);

#ifdef JSON_DEBUG
        PRINT_DEBUG_BEGIN("t=");
        json_type_trie_print_node(t, stderr);
        PRINT_DEBUG_END();
#endif
        p = q = NULL;
        json_type_trie_sib_iterator_init(&s, t, 0);

        while (!json_type_trie_sib_iterator_at_end(&s)) {
            u = json_type_trie_sib_iterator_deref(&s);
            ASSERT(u != NULL);

#ifdef JSON_DEBUG
            PRINT_DEBUG_BEGIN("u=");
            json_type_trie_print_node(u, stderr);
            PRINT_DEBUG_END();
#endif
            if (u->sym != NULL && u->sym != p &&
                JSON_TYPE_NODE_IS_CONST(u->sym, object)) {
                if (p != NULL) {
                    json_type_trie_sib_iterator_done(&s);
                    json_type_trie_lvl_iterator_done(&l);

                    return JSON_TYPE_LIB_LIST_ATTR_NOT_SUPP_ERROR(
                        sort_of_array_of_object, p, u->sym);
                }
                p = u->sym;
            }
            if (u->sym != NULL && u->sym != q &&
                JSON_TYPE_NODE_IS_CONST(u->sym, array)) {
                if (q != NULL) {
                    json_type_trie_sib_iterator_done(&s);
                    json_type_trie_lvl_iterator_done(&l);

                    return JSON_TYPE_LIB_LIST_ATTR_NOT_SUPP_ERROR(
                        sort_of_array_of_array, q, u->sym);
                }
                q = u->sym;
            }
            if (u->sym != NULL &&
                JSON_TYPE_NODE_IS_CONST(u->sym, list)) {
                w = json_type_trie_node_get_leaf(u);

                ASSERT(w != NULL);
                m = JSON_TYPE_TRIE_NODE_AS_VAL(w);

                json_type_trie_sib_iterator_done(&s);
                json_type_trie_lvl_iterator_done(&l);

                return JSON_TYPE_LIB_LIST_ATTR_NOT_SUPP_ERROR(
                    sort_of_array_of_list, m, u->sym);
            }

            json_type_trie_sib_iterator_inc(&s);
        }

        json_type_trie_sib_iterator_done(&s);
        json_type_trie_lvl_iterator_inc(&l);
    }

    json_type_trie_lvl_iterator_done(&l);

    return true;
}

#define JSON_TYPE_TRIE_CONST_CAST(p) \
    CONST_CAST(p, struct json_type_trie_t)
#define JSON_TYPE_OBJECT_TRIE_CONST_CAST(p) \
    CONST_CAST(p, struct json_type_object_trie_t)

static bool json_type_lib_gen_list_attrs(
    struct json_type_lib_t* lib,
    const struct json_type_list_node_t* list,
    const struct json_type_list_attr_t** result)
{
    const struct json_type_node_t **p, **e, *m;
    const struct json_type_trie_node_t* t;
    struct json_type_list_attr_t* r;

    struct {
        size_t any;
        size_t plain;
        size_t object;
        size_t open_array;
        size_t closed_array;
    } c;

    memset(&c, 0, sizeof(c));

    for (p = list->args,
         e = p + list->size;
         p < e;
         p ++) {
        const struct json_type_node_t* n = *p;
        if (n->type == json_type_any_node_type)
            c.any ++;
        else
        if (n->type == json_type_plain_node_type)
            c.plain ++;
        else
        if (n->type == json_type_object_node_type)
            c.object ++;
        else
        if (n->type == json_type_array_node_type) {
            if (n->node.array.type ==
                    json_type_open_array_node_type)
                c.open_array ++;
            else
            if (n->node.array.type ==
                    json_type_closed_array_node_type)
                c.closed_array ++;
            else
                UNEXPECT_VAR("%d", n->node.array.type);
        }
        else
        if (n->type == json_type_list_node_type) {
            const struct json_type_node_t* m =
                JSON_TYPE_TO_NODE_CONST(
                    list, list);
            return JSON_TYPE_LIB_ATTR_ERROR(
                invalid_list_elem_is_list,
                n->pos, m->pos);
        }
        else
            UNEXPECT_VAR("%d", n->type);
    }

    r = json_type_lib_new_list_attr(lib);

    r->any = c.any
        ? json_type_lib_new_trie(lib)
        : NULL;
    r->plain = c.plain
        ? json_type_lib_new_trie(lib)
        : NULL;
    r->object = c.object
        ? json_type_lib_new_object_trie(lib)
        : NULL;
    r->open_array = c.open_array
        ? json_type_lib_new_trie(lib)
        : NULL;
    r->closed_array = c.closed_array
        ? json_type_lib_new_trie(lib)
        : NULL;

    for (p = list->args,
         e = p + list->size;
         p < e;
         p ++) {
        const struct json_type_node_t* n = *p;
        union json_type_node_pack_t q;

        if ((q.any = JSON_TYPE_NODE_AS_IF_CONST(n, any))) {
            ASSERT(r->any != NULL);
            if (!json_type_trie_insert_sym(
                    JSON_TYPE_TRIE_CONST_CAST(r->any), n, &t)) {
                m = JSON_TYPE_TRIE_NODE_AS_VAL(t);
                return JSON_TYPE_LIB_ATTR_ERROR(
                    invalid_list_dup_any,
                    n->pos, m->pos);
            }
            *JSON_TYPE_TRIE_NODE_AS_VAL_REF(t) = n;
        }
        else
        if ((q.plain = JSON_TYPE_NODE_AS_IF_CONST(n, plain))) {
            ASSERT(r->plain != NULL);
            if (!json_type_trie_insert_sym(
                    JSON_TYPE_TRIE_CONST_CAST(r->plain), n, &t)) {
                m = JSON_TYPE_TRIE_NODE_AS_VAL(t);
                return JSON_TYPE_LIB_ATTR_ERROR(
                    invalid_list_dup_plain,
                    n->pos, m->pos);
            }
            *JSON_TYPE_TRIE_NODE_AS_VAL_REF(t) = n;
        }
        else
        if ((q.object = JSON_TYPE_NODE_AS_IF_CONST(n, object))) {
            const struct json_type_object_trie_node_t* t;
            struct json_type_object_sym_key_t k = {
                .n = 0, .object = q.object
            };

            ASSERT(r->object != NULL);
            if (!json_type_object_trie_insert_key(
                    JSON_TYPE_OBJECT_TRIE_CONST_CAST(r->object), k, &t)) {
                m = JSON_TYPE_OBJECT_TRIE_NODE_AS_VAL(t);
                return JSON_TYPE_LIB_ATTR_ERROR(
                    invalid_list_dup_object,
                    n->pos, m->pos);
            }
            *JSON_TYPE_OBJECT_TRIE_NODE_AS_VAL_REF(t) = n;
        }
        else
        if ((q.array = JSON_TYPE_NODE_AS_IF_CONST(n, array))) {
            union json_type_array_node_pack_t a;

            if ((a.open =
                    JSON_TYPE_ARRAY_NODE_AS_IF_CONST(q.array, open))) {
                ASSERT(r->open_array != NULL);
                if (!json_type_trie_insert_sym(
                        JSON_TYPE_TRIE_CONST_CAST(r->open_array),
                        a.open->arg, &t)) {
                    m = JSON_TYPE_TRIE_NODE_AS_VAL(t);
                    return JSON_TYPE_LIB_ATTR_ERROR(
                        invalid_list_dup_open_array,
                        n->pos, m->pos);
                }
                *JSON_TYPE_TRIE_NODE_AS_VAL_REF(t) = n;
            }
            else
            if ((a.closed =
                    JSON_TYPE_ARRAY_NODE_AS_IF_CONST(q.array, closed))) {
                ASSERT(r->closed_array != NULL);
                if (!json_type_trie_insert_key(
                        JSON_TYPE_TRIE_CONST_CAST(r->closed_array),
                        a.closed->args, &t)) {
                    m = JSON_TYPE_TRIE_NODE_AS_VAL(t);
                    return JSON_TYPE_LIB_ATTR_ERROR(
                        invalid_list_dup_closed_array,
                        n->pos, m->pos);
                }
                *JSON_TYPE_TRIE_NODE_AS_VAL_REF(t) = n;
            }
            else
                UNEXPECT_VAR("%d", q.array->type);
        }
        else
        if (n->type != json_type_list_node_type)
            UNEXPECT_VAR("%d", n->type);
    }

    if (r->any != NULL)
        ASSERT(r->any->root != NULL);
    if (r->plain != NULL)
        ASSERT(r->plain->root != NULL);
    if (r->object != NULL)
        ASSERT(r->object->root != NULL);
    if (r->open_array != NULL)
        ASSERT(r->open_array->root != NULL);
    if (r->closed_array != NULL)
        ASSERT(r->closed_array->root != NULL);

    *result = r;
    return true;
}

static bool json_type_lib_check_list_attrs(
    struct json_type_lib_t* lib,
    const struct json_type_list_attr_t* attr)
{
    if (attr->open_array != NULL &&
        attr->closed_array != NULL &&
        !json_type_lib_check_arrays_ambiguities(
            lib, attr->open_array, attr->closed_array))
        return false;

    if (attr->object != NULL &&
        !json_type_lib_check_object_ambiguities(
            lib, attr->object))
        return false;

    if (attr->open_array != NULL &&
        !json_type_lib_check_array_ambiguities(
            lib, attr->open_array))
        return false;

    if (attr->closed_array != NULL &&
        !json_type_lib_check_array_ambiguities(
            lib, attr->closed_array))
        return false;

    if (attr->open_array != NULL &&
        attr->closed_array != NULL &&
        !json_type_lib_check_arrays_not_supported(
            lib, attr->open_array, attr->closed_array))
        return false;

    if (attr->object != NULL &&
        !json_type_lib_check_object_not_supported(
            lib, attr->object))
        return false;

    if (attr->open_array != NULL &&
        !json_type_lib_check_array_not_supported(
            lib, attr->open_array))
        return false;

    if (attr->closed_array != NULL &&
        !json_type_lib_check_array_not_supported(
            lib, attr->closed_array))
        return false;

    return true;
}

static bool json_type_lib_gen_node_attrs(
    struct json_type_lib_t* lib, const struct json_type_node_t* node)
{
    union json_type_node_pack_t n;

    if (JSON_TYPE_NODE_IS_CONST(node, any) ||
        JSON_TYPE_NODE_IS_CONST(node, plain)) 
        ; // stev: nop
    else
    if ((n.object = JSON_TYPE_NODE_AS_IF_CONST(node, object))) {
        const struct json_type_object_node_arg_t *p, *e;

        for (p = n.object->args,
             e = p + n.object->size;
             p < e;
             p ++) {
            if (!json_type_lib_gen_node_attrs(lib, p->type))
                return false;
        }
    }
    else
    if ((n.array = JSON_TYPE_NODE_AS_IF_CONST(node, array))) {
        union json_type_array_node_pack_t a;

        if ((a.open =
                JSON_TYPE_ARRAY_NODE_AS_IF_CONST(n.array, open))) {
            if (!json_type_lib_gen_node_attrs(lib, a.open->arg))
                return false;
        }
        else
        if ((a.closed =
                JSON_TYPE_ARRAY_NODE_AS_IF_CONST(n.array, closed))) {
            const struct json_type_node_t **p, **e;

            for (p = a.closed->args,
                 e = p + a.closed->size;
                 p < e;
                 p ++) {
                if (!json_type_lib_gen_node_attrs(lib, *p))
                    return false;
            }
        }
        else
            UNEXPECT_VAR("%d", n.array->type);
    }
    else
    if ((n.list = JSON_TYPE_NODE_AS_IF_CONST(node, list))) {
        const struct json_type_node_t **p, **e;
        const struct json_type_list_attr_t** a;

        a = &JSON_TYPE_NODE_CONST_CAST(node)->attr.list;
        if (!json_type_lib_gen_list_attrs(lib, n.list, a))
            return false;

        for (p = n.list->args,
             e = p + n.list->size;
             p < e;
             p ++) {
            if (!json_type_lib_gen_node_attrs(lib, *p))
                return false;
        }
    }
    else
        UNEXPECT_VAR("%d", node->type);

    return true;
}

static bool json_type_lib_check_node_attrs(
    struct json_type_lib_t* lib, const struct json_type_node_t* node)
{
    union json_type_node_pack_t n;

    if (JSON_TYPE_NODE_IS_CONST(node, any) ||
        JSON_TYPE_NODE_IS_CONST(node, plain)) 
        ; // stev: nop
    else
    if ((n.object = JSON_TYPE_NODE_AS_IF_CONST(node, object))) {
        const struct json_type_object_node_arg_t *p, *e;

        for (p = n.object->args,
             e = p + n.object->size;
             p < e;
             p ++) {
            if (!json_type_lib_check_node_attrs(lib, p->type))
                return false;
        }
    }
    else
    if ((n.array = JSON_TYPE_NODE_AS_IF_CONST(node, array))) {
        union json_type_array_node_pack_t a;

        if ((a.open =
                JSON_TYPE_ARRAY_NODE_AS_IF_CONST(n.array, open))) {
            if (!json_type_lib_check_node_attrs(lib, a.open->arg))
                return false;
        }
        else
        if ((a.closed =
                JSON_TYPE_ARRAY_NODE_AS_IF_CONST(n.array, closed))) {
            const struct json_type_node_t **p, **e;

            for (p = a.closed->args,
                 e = p + a.closed->size;
                 p < e;
                 p ++) {
                if (!json_type_lib_check_node_attrs(lib, *p))
                    return false;
            }
        }
        else
            UNEXPECT_VAR("%d", n.array->type);
    }
    else
    if ((n.list = JSON_TYPE_NODE_AS_IF_CONST(node, list))) {
        const struct json_type_node_t **p, **e;

        if (!json_type_lib_check_list_attrs(lib, node->attr.list))
            return false;

        for (p = n.list->args,
             e = p + n.list->size;
             p < e;
             p ++) {
            if (!json_type_lib_check_node_attrs(lib, *p))
                return false;
        }
    }
    else
        UNEXPECT_VAR("%d", node->type);

    return true;
}

static bool json_type_lib_gen_defs_attrs(
    struct json_type_lib_t* lib, const struct json_type_defs_t* defs)
{
    const struct json_type_defs_arg_t *p, *e;

    for (p = defs->args,
         e = p + defs->size;
         p < e;
         p ++) {
        if (!json_type_lib_gen_node_attrs(lib, p->type))
            return false;
    }

    return true;
}

static bool json_type_lib_check_defs_attrs(
    struct json_type_lib_t* lib, const struct json_type_defs_t* defs)
{
    const struct json_type_defs_arg_t *a = NULL, *b = NULL;
    const struct json_type_defs_arg_t *p, *e;

    if (!json_type_defs_check_dup_keys(defs, &a, &b))
        return JSON_TYPE_LIB_ATTR_ERROR(
            invalid_defs_dup_name,
            a->pos, b->pos);

    for (p = defs->args,
         e = p + defs->size;
         p < e;
         p ++) {
        if (!json_type_lib_check_node_attrs(lib, p->type))
            return false;
    }

    return true;
}

static bool json_type_lib_gen_def_attrs(
    struct json_type_lib_t* lib, const struct json_type_def_t* def)
{
    if (def->type == json_type_def_node_type)
        return json_type_lib_gen_node_attrs(lib, def->val.node);
    if (def->type == json_type_def_defs_type)
        return json_type_lib_gen_defs_attrs(lib, def->val.defs);
    else
        UNEXPECT_VAR("%d", def->type);
}

static bool json_type_lib_check_def_attrs(
    struct json_type_lib_t* lib, const struct json_type_def_t* def)
{
    if (def->type == json_type_def_node_type)
        return json_type_lib_check_node_attrs(lib, def->val.node);
    if (def->type == json_type_def_defs_type)
        return json_type_lib_check_defs_attrs(lib, def->val.defs);
    else
        UNEXPECT_VAR("%d", def->type);
}

static const struct json_type_def_t*
    json_type_lib_text_build(
        struct json_type_lib_t* lib,
        struct json_type_lib_text_impl_t* impl)
{
    const struct json_type_node_t* n;
    const struct json_type_defs_t* d;
    struct json_type_lib_obj_t o;
    size_t s;

    if (lib->error.type != json_type_lib_error_none)
        return NULL;

    if (impl->done_build)
        return lib->def;

    ASSERT(lib->def == NULL);

    if (impl->stack == NULL)
        impl->stack = json_type_lib_stack_create(
            lib->sizes.ast.obj.stack_max,
            lib->sizes.ast.obj.stack_init);

    if (!json_type_lib_text_validate_base(lib, impl, false))
        goto done;

    s = JSON_TYPE_LIB_STACK_SIZE();
    ENSURE(s == 1,
        "invalid type-lib rules stack: size is %zu", s);

    o = JSON_TYPE_LIB_STACK_POP();

    if ((n = JSON_TYPE_LIB_OBJ_AS_IF_NODE(&o))) {
        impl->def.type = json_type_def_node_type;
        impl->def.val.node = n;
    }
    else
    if ((d = JSON_TYPE_LIB_OBJ_AS_IF_DEFS(&o))) {
        impl->def.type = json_type_def_defs_type;
        impl->def.val.defs = d;
    }
    else
        ENSURE(false,
            "invalid type-lib rules stack: "
            "top element is not 'node' neither "
            "is an array of \"name\" objects");

    if (json_type_lib_gen_def_attrs(lib, &impl->def))
        lib->def = &impl->def;

done:
    json_type_lib_stack_destroy(impl->stack);
    impl->stack = NULL;

    impl->done_build = true;

    return lib->def;
}

static const struct json_type_def_t*
    json_type_lib_sobj_build(
        struct json_type_lib_t* lib,
        struct json_type_lib_sobj_impl_t* impl UNUSED)
{
    return lib->error.type == json_type_lib_error_none
        ? lib->def : NULL;
}

static const struct json_type_def_t*
    json_type_lib_build(
        struct json_type_lib_t* lib)
{
    return lib->impl.build(lib, JSON_TYPE_LIB_IMPL_PTR());
}

enum json_type_lib_spec_type_t
{
    json_type_lib_text_spec_type,
    json_type_lib_file_spec_type,
};

struct json_type_lib_text_spec_t
{
    const uchar_t* def;
};

struct json_type_lib_file_spec_t
{
    const char* name;
};

struct json_type_lib_spec_t
{
    enum json_type_lib_spec_type_t type;
    union {
        struct json_type_lib_text_spec_t text;
        struct json_type_lib_file_spec_t file;
    };
    const char* type_name;
};

static const char json_type_text_name[] = "<text>";

static const struct json_type_def_t*
    json_type_lib_text_load(
        struct json_type_lib_t* lib,
        struct json_type_lib_text_impl_t* impl,
        const struct json_type_lib_spec_t* spec)
{
    const struct json_type_def_t* d;
    const char* n;

    if (spec->type == json_type_lib_text_spec_type)
        json_type_lib_text_parse_buf(
            lib, impl,
            spec->text.def,
            strulen(spec->text.def),
            n = json_type_text_name);
    else
    if (spec->type == json_type_lib_file_spec_type)
        json_type_lib_text_parse_file(
            lib, impl, n = spec->file.name);
    else
        UNEXPECT_VAR("%d", spec->type);

    if (!(d = json_type_lib_text_build(lib, impl)) ||
        !json_type_lib_check_def_attrs(lib, d)) {
        lib->error.file_info.name = n;
        return NULL;
    }

    return d;
}

#define JSON_TYPE_LIB_LIB_ERROR__(t, e, n, l)          \
    ({                                                 \
        struct json_type_lib_error_lib_t __e = {       \
            .type = json_type_lib_error_ ## t ## _lib, \
            .lib.t = e,                                \
            .name = n                                  \
        };                                             \
        JSON_TYPE_LIB_LIB_ERROR(__e, l);               \
        NULL;                                          \
    })
#define JSON_TYPE_LIB_LIB_ERROR_(t, e, n, l) \
    JSON_TYPE_LIB_LIB_ERROR__(t, \
        json_type_lib_ ## t ## _lib_error_ ## e, n, l)
#define JSON_TYPE_LIB_GENERIC_LIB_ERROR(e, l) \
    JSON_TYPE_LIB_LIB_ERROR_(generic, e, spec->type_name, l)
#define JSON_TYPE_LIB_SHARED_LIB_ERROR_CODE(e) \
    JSON_TYPE_LIB_LIB_ERROR__(shared, e, NULL, impl->lib_name)
#define JSON_TYPE_LIB_SHARED_LIB_ERROR(e) \
    JSON_TYPE_LIB_LIB_ERROR_(shared, e, NULL, impl->lib_name)

#define ISALPHA(c) (isascii(c) && isalpha(c))
#define ISALNUM(c) (isascii(c) && isalnum(c))

static const struct json_type_def_t*
    json_type_lib_sobj_load(
        struct json_type_lib_t* lib,
        struct json_type_lib_sobj_impl_t* impl)
{
    struct shared_lib_t
    {
        size_t (*get_lib_version)(void);
        const struct json_type_def_t*
            (*get_type_def)(void);
    };
    struct shared_lib_entry_t
    {
        const char* name;
        size_t offset;
        size_t error;
    };
#undef  CASE
#define CASE(n)                                     \
    {                                               \
        .name = #n,                                 \
        .offset = offsetof(struct shared_lib_t, n), \
        .error = json_type_lib_shared_lib_error_ ## \
            n ## _sym_not_found                     \
    }
    static const struct shared_lib_entry_t entries[] = {
        CASE(get_lib_version),
        CASE(get_type_def)
    };
    // stev: 'M' is max length of:
    //   'get_lib_version' and
    //   'get_type_def'
    enum { M = 15, N = 1024 };
    const struct shared_lib_entry_t *p, *e;
    struct json_lib_version_t r;
    struct shared_lib_t l;
    char b[N], c[N], *d;
    const char* q;
    size_t n, u, v;
    char *x, *y;
    void* s;

    ASSERT(lib->def == NULL);
    ASSERT(impl->sobj == NULL);

    impl->sobj = dlopen(impl->lib_name, RTLD_NOW);
    if (impl->sobj == NULL) {
        PRINT_DEBUG(
            "dlopen failed: %s", dlerror());
        return JSON_TYPE_LIB_SHARED_LIB_ERROR(
            load_lib_failed);
    }

    q = strrchr(impl->lib_name, '/');
    if (q == NULL)
        q = impl->lib_name;
    else
        q ++;

    n = strlen(q);
    if (n == 0 || n >= N)
        return JSON_TYPE_LIB_SHARED_LIB_ERROR(
            invalid_name);

    memcpy(b, q, n + 1);
    d = strchr(b, '.');
    if (d != NULL)
        *d = 0;

    n = strlen(b);
    STATIC(N > M);
    if (n == 0 || n >= N - M || !ISALPHA(*b))
        return JSON_TYPE_LIB_SHARED_LIB_ERROR(
            invalid_name);

    for (x = b + 1, y = b + n; x < y; x ++) {
        if (!ISALNUM(*x) && *x != '-')
            return JSON_TYPE_LIB_SHARED_LIB_ERROR(
                invalid_name);
        if (*x == '-') *x = '_';
    }

    for (p = entries,
         e = p + ARRAY_SIZE(entries);
         p < e;
         p ++) {
        const char* e;
        int r;

        u = n + strlen(p->name) + 1;
        ASSERT(u < N);
        r = snprintf(c, N, "%s_%s", b, p->name);
        v = INT_AS_SIZE(r);
        ASSERT(u == v);

        // stev: clear previous error conditions
        // (conforming to dlsym(3) man page)
        dlerror();

        s = dlsym(impl->sobj, c);
        e = dlerror();
        if (s == NULL || e != NULL) {
            if (e != NULL)
                PRINT_DEBUG(
                    "dlsym failed: %s", e);
            else
                PRINT_DEBUG(
                    "dlsym failed: symbol %s is NULL", c);
            return JSON_TYPE_LIB_SHARED_LIB_ERROR_CODE(
                p->error);
        }

        *(void**) ((char*) (&l) + p->offset) = s;
    }

    r = json_lib_version(
            l.get_lib_version());
    if (r.major != JSON_VERSION_MAJOR ||
        r.minor != JSON_VERSION_MINOR) {
        PRINT_DEBUG("lib version: %d.%d.%d",
            r.major, r.minor, r.patch);
        return JSON_TYPE_LIB_SHARED_LIB_ERROR(
            wrong_lib_version);
    }

    lib->def = l.get_type_def();
    if (lib->def == NULL)
        return JSON_TYPE_LIB_SHARED_LIB_ERROR(
            def_is_null);

    return lib->def;
}

static bool json_type_lib_sobj_validate(
    struct json_type_lib_t* lib,
    struct json_type_lib_sobj_impl_t* impl)
{
    const struct json_type_def_t* d;
    struct json_type_ptr_space_t s;
    const void* p = NULL;
    bool r;

    d = json_type_lib_sobj_build(lib, impl);
    if (d == NULL)
        return false;

    json_type_ptr_space_init(&s,
        lib->sizes.ptr_space_size);
    r = json_type_def_validate(d, &s, &p);
    json_type_ptr_space_done(&s);

    if (!r) {
        ASSERT(p != NULL);

#ifdef JSON_DEBUG
        if (PRINT_DEBUG_COND) {
            Dl_info i;
            int r;

            memset(&i, 0, sizeof(i));
            r = dladdr(p, &i);

            PRINT_DEBUG_BEGIN_UNCOND(
                "validation failed on %p", p);
            if (r && i.dli_fname != NULL &&
                !strcmp(impl->lib_name, i.dli_fname) &&
                i.dli_fbase != NULL)
                print_debug_fmt(
                    " (offset=0x%tx)", p - i.dli_fbase);
            PRINT_DEBUG_END_UNCOND();
        }
#endif

        return JSON_TYPE_LIB_SHARED_LIB_ERROR(
            def_not_valid);
    }

    return true;
}

static bool json_type_lib_is_shared_obj(
    struct json_type_lib_t* lib,
    const char* lib_name)
{
    struct json_file_error_t e;
    bool r;

    STATIC(
        json_type_lib_error_file_open_context + 1 ==
        json_file_open_error &&

        json_type_lib_error_file_stat_context + 1 ==
        json_file_stat_error &&

        json_type_lib_error_file_read_context + 1 ==
        json_file_read_error &&

        json_type_lib_error_file_close_context + 1 ==
        json_file_close_error);

    r = json_file_is_shared_obj(lib_name, &e);

    if (e.type != json_file_none_error) {
        JSON_TYPE_LIB_SYS_ERROR(
            e.type - 1,
            e.sys);
        return false;
    }

    return r;
}

static const struct json_type_node_t*
    json_type_lib_init_from_spec(
        struct json_type_lib_t* lib,
        const struct json_type_lib_spec_t* spec,
        const struct json_type_lib_sizes_t* sizes)
{
    const struct json_type_def_t* d;
#ifdef JSON_DEBUG
    struct timeval t0, t1;
#endif
    const char* l = NULL;
    bool e = false;

    if (spec->type == json_type_lib_text_spec_type)
        l = json_type_text_name;
    else
    if (spec->type == json_type_lib_file_spec_type) {
        l = spec->file.name;

        e = json_type_lib_is_shared_obj(lib, l);
        if (lib->error.type != json_type_lib_error_none)
            return NULL;
    }
    else
        UNEXPECT_VAR("%d", spec->type);

    if (!e)
        json_type_lib_init_from_source_text(
            lib, sizes);
    else
        json_type_lib_init_from_shared_obj(
            lib, l, sizes);

#ifdef JSON_DEBUG
    if (lib->debug > 1)
        json_timeval_get_time(&t0);
#endif

    d = !e
        ? json_type_lib_text_load(
            lib, JSON_TYPE_LIB_IMPL_AS(text), spec)
        : json_type_lib_sobj_load(
            lib, JSON_TYPE_LIB_IMPL_AS(sobj));

#ifdef JSON_DEBUG
    if (lib->debug > 1) {
        json_timeval_get_time(&t1);
        PRINT_DEBUG_UNCOND(
            "type-lib loaded in %zu usecs",
            json_timeval_subtract(&t1, &t0));
    }
#endif

    if (d == NULL)
        return NULL;

    if (d->type == json_type_def_node_type) {
        if (spec->type_name != NULL)
            return JSON_TYPE_LIB_GENERIC_LIB_ERROR(
                type_name_not_available, l);

        return d->val.node;
    }
    else
    if (d->type == json_type_def_defs_type) {
        const struct json_type_defs_arg_t *p, *e;

        if (spec->type_name == NULL)
            return JSON_TYPE_LIB_GENERIC_LIB_ERROR(
                type_name_not_specified, l);

        ASSERT(d->val.defs->size > 0);

        for (p = d->val.defs->args,
             e = p + d->val.defs->size;
             p < e;
             p ++) {
            if (!strucmp(p->name, spec->type_name))
                return p->type;
        }

        return JSON_TYPE_LIB_GENERIC_LIB_ERROR(
            type_def_not_found, l);
    }
    else
        UNEXPECT_VAR("%d", d->type);
}

static bool json_type_lib_print_base(
    struct json_type_lib_t* lib,
    bool attr)
{
    const struct json_type_def_t* d;

    d = json_type_lib_build(lib);
    if (d == NULL)
        return false;

    json_type_def_print(d, stdout, attr);
    fputc('\n', stdout);

    return true;
}

bool json_type_lib_print(struct json_type_lib_t* lib)
{
    return json_type_lib_print_base(lib, false);
}

bool json_type_lib_print_attr(struct json_type_lib_t* lib)
{
    return json_type_lib_print_base(lib, true);
}

bool json_type_lib_check_attr(struct json_type_lib_t* lib)
{
    const struct json_type_def_t* d;

    d = json_type_lib_build(lib);
    if (d == NULL)
        return false;

    return json_type_lib_check_def_attrs(lib, d);
}

bool json_type_lib_gen_def(struct json_type_lib_t* lib)
{
    const struct json_type_def_t* d;
    struct json_type_ptr_space_t s;

    d = json_type_lib_build(lib);
    if (d == NULL)
        return false;

    if (!JSON_TYPE_LIB_IMPL_IS(sobj) &&
        !json_type_lib_check_def_attrs(lib, d))
        return false;

    json_type_ptr_space_init(&s, lib->sizes.ptr_space_size);
    json_type_def_gen_def(d, &s, stdout);
    json_type_ptr_space_done(&s);

    return true;
}

typedef void (*json_type_node_printer_func_t)(
    const struct json_type_node_t*, FILE*);

static json_type_node_printer_func_t
    json_type_check_error_get_node_printer()
{
    static const json_type_node_printer_func_t
        printers[]= {
            json_type_node_print_base,
            json_type_node_print_complete,
        };
    json_type_node_printer_func_t r;
    const char* s;
    size_t c;

    s = getenv("JSON_TYPE_CHECK_ERROR_COMPLETE_TYPES");
    c = s && !strcmp(s, "yes");

    r = ARRAY_NULL_ELEM(printers, c);
    ASSERT(r != NULL);

    return r;
}

struct json_type_check_error_printer_t
{
    FILE* file;
    json_type_node_printer_func_t print;
    size_t nulls;
    size_t names;
    size_t nodes;
    size_t count;
    size_t i;
};

static void json_type_check_error_printer_init(
    struct json_type_check_error_printer_t* prt,
    FILE* file)
{
    memset(prt, 0, sizeof(
        struct json_type_check_error_printer_t));

    prt->print = json_type_check_error_get_node_printer();
    prt->file = file;
}

static void json_type_check_error_printer_done(
    struct json_type_check_error_printer_t* prt)
{
    ASSERT(prt->nulls < 2);
    if (prt->nulls) prt->i ++;
    ASSERT(prt->i == prt->count);
}

static void json_type_check_error_printer_print_sep(
    struct json_type_check_error_printer_t* prt)
{
    if (prt->i > 0)
        fputs(prt->i < SIZE_DEC(prt->count)
            ? json_type_print_base_comma_sep
            : json_type_print_base_or_sep,
            prt->file);
}

static void json_type_check_error_printer_print_node(
    struct json_type_check_error_printer_t* prt,
    const struct json_type_node_t* node)
{
    ASSERT(!prt->names);
    json_type_check_error_printer_print_sep(prt);
    prt->print(node, prt->file);
    prt->nodes ++;
    prt->i ++;
}

static void json_type_check_error_printer_print_node_null(
    struct json_type_check_error_printer_t* prt,
    const struct json_type_node_t* node)
{
    if (node != NULL)
        json_type_check_error_printer_print_node(
            prt, node);
    else {
        ASSERT(prt->i == 0);
        prt->nulls ++;
    }
}

static void json_type_check_error_printer_print_name(
    struct json_type_check_error_printer_t* prt,
    const uchar_t* name)
{
    ASSERT(!prt->nodes);
    json_type_check_error_printer_print_sep(prt);
    json_type_print_repr(name, true, prt->file);
    prt->names ++;
    prt->i ++;
}

static void json_type_check_error_printer_print_name_null(
    struct json_type_check_error_printer_t* prt,
    const uchar_t* name)
{
    if (name != NULL) {
        json_type_check_error_printer_print_name(
            prt, name);
    }
    else {
        ASSERT(prt->i == 0);
        prt->nulls ++;
    }
}

static void json_type_check_error_printer_print_null(
    struct json_type_check_error_printer_t* prt)
{
    ASSERT(!prt->nodes || !prt->names);
    fprintf(prt->file, " or%s %s at all",
        prt->i > 1 ? ", otherwise," : "",
        prt->nodes ? "no value" : "none");
}

static void json_type_check_error_print_object_trie_key_arg(
    const struct json_type_object_trie_node_t* node,
    FILE* file)
{
    struct json_type_object_trie_sib_iterator_t s;
    const struct json_type_object_trie_node_t* t;
    struct json_type_check_error_printer_t p;
    const uchar_t* q;

    ASSERT(node != NULL);
    ASSERT(!JSON_TYPE_OBJECT_TRIE_SYM_IS(node->sym, node));

    json_type_check_error_printer_init(&p, file);
    p.count = json_type_object_trie_node_get_sib_count(node);

    json_type_object_trie_sib_iterator_init(&s, node, 0);

    while (!json_type_object_trie_sib_iterator_at_end(&s)) {
        t = json_type_object_trie_sib_iterator_deref(&s);
        ASSERT(t != NULL);

        if ((q = NULL, JSON_TYPE_OBJECT_TRIE_SYM_IS(t->sym, null)) ||
            (q = JSON_TYPE_OBJECT_TRIE_SYM_AS_IF(t->sym, name)))
            json_type_check_error_printer_print_name_null(&p, q);
        else
        if (JSON_TYPE_OBJECT_TRIE_SYM_IS(t->sym, node))
            ASSERT(false);
        else
            UNEXPECT_VAR("%d", t->sym.type);

        json_type_object_trie_sib_iterator_inc(&s);
    }

    ASSERT(p.nulls < 2);

    if (p.nulls)
        json_type_check_error_printer_print_null(&p);

    json_type_object_trie_sib_iterator_done(&s);

    json_type_check_error_printer_done(&p);
}

static void json_type_check_error_key_arg_print_error_desc(
    const struct json_type_check_error_key_arg_t* arg,
    FILE* file)
{
    static const char head[] = ": expected ";

    switch (arg->type) {

    case json_type_check_error_key_arg_name_type:
        if (arg->name != NULL) {
            fputs(head, file);
            json_type_print_repr(arg->name, true, file);
        }
        break;

    case json_type_check_error_key_arg_object_trie_type:
        if (arg->object_trie != NULL) {
            fputs(head, file);
            json_type_check_error_print_object_trie_key_arg(
                arg->object_trie, file);
        }
        break;

    default:
        UNEXPECT_VAR("%d", arg->type);
    }
}

static void json_type_check_error_print_object_trie_val_arg(
    const struct json_type_object_trie_node_t* node,
    FILE* file)
{
    struct json_type_object_trie_sib_iterator_t s;
    const struct json_type_object_trie_node_t* t;
    struct json_type_check_error_printer_t p;
    const struct json_type_node_t* q;

    ASSERT(node != NULL);
    ASSERT(JSON_TYPE_OBJECT_TRIE_SYM_IS(node->sym, node));

    json_type_check_error_printer_init(&p, file);
    p.count = json_type_object_trie_node_get_sib_count(node);

    json_type_object_trie_sib_iterator_init(&s, node, 0);

    while (!json_type_object_trie_sib_iterator_at_end(&s)) {
        t = json_type_object_trie_sib_iterator_deref(&s);
        ASSERT(t != NULL);

        if (JSON_TYPE_OBJECT_TRIE_SYM_IS(t->sym, null) ||
            JSON_TYPE_OBJECT_TRIE_SYM_IS(t->sym, name))
            ASSERT(false);
        else
        if ((q = JSON_TYPE_OBJECT_TRIE_SYM_AS_IF(t->sym, node)))
            json_type_check_error_printer_print_node(&p, q);
        else
            UNEXPECT_VAR("%d", t->sym.type);

        json_type_object_trie_sib_iterator_inc(&s);
    }

    json_type_object_trie_sib_iterator_done(&s);

    json_type_check_error_printer_done(&p);
}

struct json_type_check_error_trie_diff_t
{
    const struct json_type_trie_node_t* node;
    size_t eq;
};

static bool json_type_check_error_trie_diff(
    struct json_type_check_error_trie_diff_t* diff,
    const struct json_type_trie_node_t* node)
{
    ASSERT(node != NULL);

    if (json_type_trie_lookup_sym_node(
            diff->node, node->sym,
            NULL)) {
        diff->eq ++;
        return false;
    }
    return true;
}

static void json_type_check_error_print_array_trie_val_arg(
    const struct json_type_check_error_array_trie_val_arg_t* arg,
    FILE* file)
{
    struct json_type_check_error_trie_diff_t d0, d1;
    struct json_type_trie_sib_iterator_t s0, s1;
    const struct json_type_trie_node_t *t0, *t1;
    struct json_type_check_error_printer_t p;

    ASSERT(arg->n_arg == 0);
    ASSERT(arg->open != NULL || arg->closed != NULL);

    json_type_check_error_printer_init(&p, file);

    memset(&d0, 0, sizeof(d0));
    memset(&d1, 0, sizeof(d1));

    if (arg->open) {
        d0.node = arg->closed;
        p.count += json_type_trie_node_get_sib_count_if(
            arg->open,
            (json_type_trie_node_cond_func_t)
            json_type_check_error_trie_diff,
            &d0);

        json_type_trie_sib_iterator_init(
            &s0, arg->open, 0);
    }
    if (arg->closed) {
        d1.node = arg->open;
        p.count += json_type_trie_node_get_sib_count_if(
            arg->closed,
            (json_type_trie_node_cond_func_t)
            json_type_check_error_trie_diff,
            &d1);

        json_type_trie_sib_iterator_init(
            &s1, arg->closed, 0);
    }

    ASSERT(d0.eq == d1.eq);
    p.count += d0.eq;

    while (arg->open
                && !json_type_trie_sib_iterator_at_end(&s0) &&
           arg->closed
                && !json_type_trie_sib_iterator_at_end(&s1)) {
        const struct json_type_trie_node_t* t;
        enum trie_sym_cmp_t c;

        t0 = json_type_trie_sib_iterator_deref(&s0);
        ASSERT(t0 != NULL);
        t1 = json_type_trie_sib_iterator_deref(&s1);
        ASSERT(t1 != NULL);

        c = json_type_trie_sym_cmp(t0->sym, t1->sym);
        t = c != trie_sym_cmp_gt ? t0 : t1;

        json_type_check_error_printer_print_node_null(
            &p, t->sym);

        if (c != trie_sym_cmp_gt)
            json_type_trie_sib_iterator_inc(&s0);
        if (c != trie_sym_cmp_lt)
            json_type_trie_sib_iterator_inc(&s1);
    }

    while (arg->open
            && !json_type_trie_sib_iterator_at_end(&s0)) {
        const struct json_type_trie_node_t* t;

        t = json_type_trie_sib_iterator_deref(&s0);
        ASSERT(t != NULL);

        json_type_check_error_printer_print_node_null(
            &p, t->sym);

        json_type_trie_sib_iterator_inc(&s0);
    }

    while (arg->closed
            && !json_type_trie_sib_iterator_at_end(&s1)) {
        const struct json_type_trie_node_t* t;

        t = json_type_trie_sib_iterator_deref(&s1);
        ASSERT(t != NULL);

        json_type_check_error_printer_print_node_null(
            &p, t->sym);

        json_type_trie_sib_iterator_inc(&s1);
    }

    ASSERT(p.nulls < 2);

    if ((arg->open != NULL && arg->n_arg == 0) || p.nulls)
        json_type_check_error_printer_print_null(&p);

    if (arg->closed)
        json_type_trie_sib_iterator_done(&s1);
    if (arg->open)
        json_type_trie_sib_iterator_done(&s0);

    json_type_check_error_printer_done(&p);
}

static void json_type_check_error_print_array_trie_val_arg_n(
    const struct json_type_check_error_array_trie_val_arg_t* arg,
    FILE* file)
{
    struct json_type_check_error_printer_t p;
    struct json_type_trie_sib_iterator_t s;
    bool e = true;

    ASSERT(arg->n_arg > 0);
    ASSERT(arg->open != NULL || arg->closed != NULL);

    json_type_check_error_printer_init(&p, file);

    if (arg->open)
        p.count ++;
    if (arg->closed) {
        p.count += json_type_trie_node_get_sib_count(
            arg->closed);

        json_type_trie_sib_iterator_init(
            &s, arg->closed, 0);
    }
    if (arg->open && arg->closed &&
        json_type_trie_lookup_sym_node(
            arg->closed, arg->open->sym, NULL)) {
        p.count --;
        e = false;
    }

    while (arg->open && e && arg->closed
                && !json_type_trie_sib_iterator_at_end(&s)) {
        const struct json_type_trie_node_t *t, *u;
        enum trie_sym_cmp_t c;

        t = json_type_trie_sib_iterator_deref(&s);
        ASSERT(t != NULL);

        c = json_type_trie_sym_cmp(arg->open->sym, t->sym);
        u = c != trie_sym_cmp_gt ? arg->open : t;

        json_type_check_error_printer_print_node_null(
            &p, u->sym);

        if (c != trie_sym_cmp_gt)
            e = false;
        if (c != trie_sym_cmp_lt)
            json_type_trie_sib_iterator_inc(&s);
    }

    if (arg->open && e)
        json_type_check_error_printer_print_node_null(
            &p, arg->open->sym);

    while (arg->closed
            && !json_type_trie_sib_iterator_at_end(&s)) {
        const struct json_type_trie_node_t* t;

        t = json_type_trie_sib_iterator_deref(&s);
        ASSERT(t != NULL);

        json_type_check_error_printer_print_node_null(
            &p, t->sym);

        json_type_trie_sib_iterator_inc(&s);
    }

    ASSERT(p.nulls < 2);

    if (p.nulls)
        json_type_check_error_printer_print_null(&p);

    if (arg->closed)
        json_type_trie_sib_iterator_done(&s);

    json_type_check_error_printer_done(&p);
}

static void json_type_check_error_val_arg_print_error_desc(
    const struct json_type_check_error_val_arg_t* arg,
    FILE* file)
{
    static const char head[] = ": expected a value of type ";

    switch (arg->type) {

    case json_type_check_error_val_arg_node_type:
        if (arg->node != NULL) {
            json_type_node_printer_func_t print =
                json_type_check_error_get_node_printer();
            fputs(head, file);
            print(arg->node, file);
        }
        break;

    case json_type_check_error_val_arg_object_trie_type:
        if (arg->object_trie != NULL) {
            fputs(head, file);
            json_type_check_error_print_object_trie_val_arg(
                arg->object_trie, file);
        }
        break;

    case json_type_check_error_val_arg_array_trie_type:
        if (arg->array_trie.open != NULL ||
            arg->array_trie.closed != NULL) {
            fputs(head, file);
            (arg->array_trie.n_arg
                ? json_type_check_error_print_array_trie_val_arg_n
                : json_type_check_error_print_array_trie_val_arg)(
                    &arg->array_trie, file);
        }
        break;

    default:
        UNEXPECT_VAR("%d", arg->type);
    }
}

static void json_type_check_error_arg_print_error_desc(
    const struct json_type_check_error_arg_t* arg,
    FILE* file)
{
    switch (arg->type) {
    case json_type_check_error_arg_none_type:
        break;
    case json_type_check_error_arg_key_type:
        json_type_check_error_key_arg_print_error_desc(
            &arg->key, file);
        break;
    case json_type_check_error_arg_val_type:
        json_type_check_error_val_arg_print_error_desc(
            &arg->val, file);
        break;
    default:
        UNEXPECT_VAR("%d", arg->type);
    }
}

static const char* json_type_check_error_type_get_desc(
    enum json_type_check_error_type_t type)
{
    switch (type) {
    case json_type_check_error_none:
        return "-";
    case json_type_check_error_type_mismatch:
        return "type mismatch";
    case json_type_check_error_too_many_args:
        return "too many arguments";
    case json_type_check_error_too_few_args:
        return "too few arguments";
    case json_type_check_error_invalid_arg_name:
        return "invalid argument name";
    default:
        UNEXPECT_VAR("%d", type);
    }
}

static void json_type_check_error_info_print_error_desc(
    const struct json_type_check_error_info_t* info,
    FILE* file)
{
    fputs(json_type_check_error_type_get_desc(
        info->type), file);
    json_type_check_error_arg_print_error_desc(
        &info->arg, file);
}

#ifdef JSON_DEBUG

#undef  CASE
#define CASE(E) \
    case json_type_check_error_ ## E: \
        return #E;

static const char* json_type_check_error_type_get_name(
    enum json_type_check_error_type_t type)
{
    switch (type) {
    CASE(none);
    CASE(type_mismatch);
    CASE(too_many_args);
    CASE(too_few_args);
    CASE(invalid_arg_name);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

#undef  CASE
#define CASE(E) \
    case json_type_check_error_key_arg_ ## E ## _type: \
        return #E;

static const char* json_type_check_error_key_arg_type_get_name(
    enum json_type_check_error_key_arg_type_t type)
{
    switch (type) {
    CASE(name);
    CASE(object_trie);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

static void json_type_check_error_key_arg_print_error_debug(
    const struct json_type_check_error_key_arg_t* arg,
    FILE* file)
{
    fprintf(file, "{.type=%s ",
        json_type_check_error_key_arg_type_get_name(arg->type));

    switch (arg->type) {
    case json_type_check_error_key_arg_name_type:
        fputs(".name=", file);
        json_type_string_print_debug(arg->name, file);
        break;
    case json_type_check_error_key_arg_object_trie_type:
        fputs(".object_trie=", file);
        json_type_object_trie_print_node(arg->object_trie, file);
        break;
    default:
        UNEXPECT_VAR("%d", arg->type);
    }

    fputc('}', file);
}

#undef  CASE
#define CASE(E) \
    case json_type_check_error_val_arg_ ## E ## _type: \
        return #E;

static const char* json_type_check_error_val_arg_type_get_name(
    enum json_type_check_error_val_arg_type_t type)
{
    switch (type) {
    CASE(node);
    CASE(object_trie);
    CASE(array_trie);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

static void json_type_check_error_val_arg_print_error_debug(
    const struct json_type_check_error_val_arg_t* arg,
    FILE* file)
{
    fprintf(file, "{.type=%s ",
        json_type_check_error_val_arg_type_get_name(arg->type));

    switch (arg->type) {
    case json_type_check_error_val_arg_node_type:
        fputs(".node=", file);
        json_type_node_null_print_debug(arg->node, file);
        break;
    case json_type_check_error_val_arg_object_trie_type:
        fputs(".object_trie=", file);
        json_type_object_trie_print_node(arg->object_trie, file);
        break;
    case json_type_check_error_val_arg_array_trie_type:
        fputs(".array_trie={.open=", file);
        json_type_trie_print_node(arg->array_trie.open, file);
        fputs(" .closed=", file);
        json_type_trie_print_node(arg->array_trie.closed, file);
        fprintf(file, " .n_arg=%zu}", arg->array_trie.n_arg);
        break;
    default:
        UNEXPECT_VAR("%d", arg->type);
    }

    fputc('}', file);
}

#undef  CASE
#define CASE(E) \
    case json_type_check_error_arg_ ## E ## _type: \
        return #E;

static const char* json_type_check_error_arg_type_get_name(
    enum json_type_check_error_arg_type_t type)
{
    switch (type) {
    CASE(none);
    CASE(key);
    CASE(val);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

static void json_type_check_error_arg_print_error_debug(
    const struct json_type_check_error_arg_t* arg,
    FILE* file)
{
    fprintf(file, "{.type=%s",
        json_type_check_error_arg_type_get_name(arg->type));

    switch (arg->type) {
    case json_type_check_error_arg_none_type:
        break;
    case json_type_check_error_arg_key_type:
        fputs(" .key=", file);
        json_type_check_error_key_arg_print_error_debug(
            &arg->key, file);
        break;
    case json_type_check_error_arg_val_type:
        fputs(" .val=", file);
        json_type_check_error_val_arg_print_error_debug(
            &arg->val, file);
        break;
    default:
        UNEXPECT_VAR("%d", arg->type);
    }

    fputc('}', file);
}

static void json_type_check_error_info_print_error_debug(
    const struct json_type_check_error_info_t* info,
    FILE* file)
{
    fprintf(file, "# %s {.arg=",
        json_type_check_error_type_get_name(info->type));
    json_type_check_error_arg_print_error_debug(
        &info->arg, file);
    fprintf(file, " .pos={.line=%zu .col=%zu}}",
        info->pos.line, info->pos.col);
}

#endif

enum json_type_obj_type_t
{
    json_type_obj_node_type,
    json_type_obj_args_type,
};

enum json_type_obj_args_type_t
{
    json_type_obj_args_object_type,
    json_type_obj_args_array_type,
    json_type_obj_args_object_trie_type,
    json_type_obj_args_array_trie_type,
};

#define json_type_array_trie_node_t json_type_trie_node_t

struct json_type_obj_args_object_trie_attr_t
{
    struct json_text_pos_t         pos;
    const struct json_type_node_t* base;
};

struct json_type_obj_args_array_trie_attr_t
{
    struct json_text_pos_t              pos;
    const struct json_type_node_t*      base;
    const struct json_type_node_t*      open_node;
    const struct json_type_trie_node_t* open_trie;
    bool                                is_closed;
};

struct json_type_obj_args_t
{
    enum json_type_obj_args_type_t type;
    union {
        const struct json_type_object_node_t*      object;
        const struct json_type_array_node_t*       array;
        const struct json_type_object_trie_node_t* object_trie;
        const struct json_type_array_trie_node_t*  array_trie;
    };
    union {
        // struct json_type_obj_args_object_attr_t   object;
        // struct json_type_obj_args_array_attr_t    array;
        struct json_type_obj_args_object_trie_attr_t object_trie;
        struct json_type_obj_args_array_trie_attr_t  array_trie;
    } attr;
    size_t n_arg;
};

struct json_type_obj_t
{
    enum json_type_obj_type_t type;
    union {
        const struct json_type_node_t* node;
        struct json_type_obj_args_t    args;
    };
    struct json_text_pos_t pos;
};

#undef  STACK_NAME
#define STACK_NAME             json_type
#undef  STACK_ELEM_TYPE
#define STACK_ELEM_TYPE struct json_type_obj_t
#define STACK_ELEM_PRINT_DEBUG json_type_obj_print_debug
#undef  STACK_ELEM_EQUAL
#define STACK_ELEM_EQUAL(x, y) ((x)->type == (y)->type)

#define STACK_NEED_MAX_SIZE
#include "stack-impl.h"
#undef  STACK_NEED_MAX_SIZE

#define JSON_TYPE_STACK_PUSH_NODE(v, p)     \
    do {                                    \
        struct json_type_obj_t __o;         \
        memset(&__o, 0, sizeof(__o));       \
        __o.type = json_type_obj_node_type; \
        __o.node = v;                       \
        __o.pos = p;                        \
        JSON_TYPE_OBJ_PRINT_DEBUG(          \
            "push: ", &__o);                \
        STACK_PUSH(&type->obj_stack, __o);  \
    } while (0)

#define JSON_TYPE_STACK_PUSH_ARGS_(t, v, p, a) \
    ({                                         \
        struct json_type_obj_t* __r;           \
        struct json_type_obj_t __o;            \
        memset(&__o, 0, sizeof(__o));          \
        __o.type = json_type_obj_args_type;    \
        __o.args.type =                        \
            json_type_obj_args_ ## t ## _type; \
        __o.args.t = v;                        \
        a;                                     \
        __o.pos = p;                           \
        JSON_TYPE_OBJ_PRINT_DEBUG(             \
            "push: ", &__o);                   \
        STACK_PUSH(&type->obj_stack, __o);     \
        __r = STACK_TOP_REF(&type->obj_stack); \
        ASSERT(JSON_TYPE_OBJ_IS(__r, args));   \
        &__r->args;                            \
    })
#define JSON_TYPE_STACK_PUSH_ARGS(t, v, p) \
    JSON_TYPE_STACK_PUSH_ARGS_(t, v, p, )
#define JSON_TYPE_STACK_PUSH_TRIE_ARGS(t, b, v, p, a) \
    JSON_TYPE_STACK_PUSH_ARGS_(t, v, p, \
        __o.args.attr.t.pos = p; \
        __o.args.attr.t.base = b; \
        a)
#define JSON_TYPE_STACK_PUSH_OBJECT_TRIE(b, v, p) \
    JSON_TYPE_STACK_PUSH_TRIE_ARGS(object_trie, b, v, p, )
#define JSON_TYPE_STACK_PUSH_ARRAY_TRIE(b, v, c, p) \
    JSON_TYPE_STACK_PUSH_TRIE_ARGS(array_trie, b, v, p, \
        __o.args.attr.array_trie.is_closed = c)

#define JSON_TYPE_STACK_POP_ARGS(t)      \
    do {                                 \
        struct json_type_obj_t __o;      \
        __o = JSON_TYPE_STACK_POP();     \
        JSON_TYPE_OBJ_PRINT_DEBUG(       \
            "pop: ", &__o);              \
        ASSERT(JSON_TYPE_OBJ_IS(         \
            &__o, args));                \
        ASSERT(                          \
            JSON_TYPE_OBJ_ARGS_IS(       \
                &__o.args, t) ||         \
            JSON_TYPE_OBJ_ARGS_IS(       \
                &__o.args, t ## _trie)); \
    } while (0)

#define JSON_TYPE_STACK_SIZE()    STACK_SIZE(&type->obj_stack)
#define JSON_TYPE_STACK_POP()     STACK_POP(&type->obj_stack)
#define JSON_TYPE_STACK_TOP_REF() STACK_TOP_REF(&type->obj_stack)

#define JSON_TYPE_TYPEOF_IS_OBJ_ARGS(p) \
    TYPEOF_IS(p, struct json_type_obj_args_t*) 

#define JSON_TYPE_OBJ_ARGS_IS(p, n)                    \
    (                                                  \
        STATIC(JSON_TYPE_TYPEOF_IS_OBJ_ARGS(p)),       \
        (p)->type == json_type_obj_args_ ## n ## _type \
    )

#define JSON_TYPE_OBJ_ARGS_AS(p, n)          \
    (                                        \
        ASSERT(JSON_TYPE_OBJ_ARGS_IS(p, n)); \
        (p)->n;                              \
    )

#define JSON_TYPE_OBJ_ARGS_AS_IF(p, n) \
    (                                  \
        JSON_TYPE_OBJ_ARGS_IS(p, n)    \
        ? (p)->n : NULL                \
    )

#define JSON_TYPE_TYPEOF_IS_OBJ(p) \
    TYPEOF_IS(p, struct json_type_obj_t*)

#define JSON_TYPE_OBJ_IS(p, n)                    \
    (                                             \
        STATIC(JSON_TYPE_TYPEOF_IS_OBJ(p)),       \
        (p)->type == json_type_obj_ ## n ## _type \
    )

#define JSON_TYPE_OBJ_AS_NODE(p)           \
    ({                                     \
        ASSERT(JSON_TYPE_OBJ_IS(p, node)); \
        (p)->node;                         \
    })

#define JSON_TYPE_OBJ_AS_IF_NODE(p) \
    ({                              \
        JSON_TYPE_OBJ_IS(p, node)   \
        ? p->node : NULL;           \
    })

#define JSON_TYPE_OBJ_AS_IF_ARGS(p) \
    ({                              \
        JSON_TYPE_OBJ_IS(p, args)   \
        ? &(p->args) : NULL;        \
    })

#define JSON_TYPE_STACK_TOP_REF_ENSURE_AS_ARGS(n)          \
    ({                                                     \
        struct json_type_obj_t* __o;                       \
        struct json_type_obj_args_t* __a;                  \
        __o = JSON_TYPE_STACK_TOP_REF();                   \
        ENSURE(JSON_TYPE_OBJ_IS(__o, args),                \
            "invalid type stack: top element not 'args'"); \
        __a = &__o->args;                                  \
        ENSURE(                                            \
            JSON_TYPE_OBJ_ARGS_IS(__a, n) ||               \
            JSON_TYPE_OBJ_ARGS_IS(__a, n ## _trie),        \
            "invalid type stack: top element neither "     \
            "'" #n " args' nor '" #n " trie args'");       \
        __a;                                               \
    })

#undef  PRINT_DEBUG_COND
#define PRINT_DEBUG_COND type->debug

#ifdef JSON_DEBUG
#define JSON_TYPE_OBJ_PRINT_DEBUG(n, o)       \
    do {                                      \
        PRINT_DEBUG_BEGIN("%s", n);           \
        json_type_obj_print_debug(o, stderr); \
        PRINT_DEBUG_END();                    \
    } while (0)
#define JSON_TYPE_STACK_TOP_PRINT_DEBUG() \
    do {                                  \
        struct json_type_obj_t* __o;      \
        __o = JSON_TYPE_STACK_TOP_REF();  \
        JSON_TYPE_OBJ_PRINT_DEBUG(        \
            "top=", __o);                 \
    } while (0)
#else
#define JSON_TYPE_OBJ_PRINT_DEBUG(n, o)
#define JSON_TYPE_STACK_TOP_PRINT_DEBUG()
#endif

struct json_type_t
{
#ifdef JSON_DEBUG
    bits_t                         debug:
                                   debug_bits;
#endif
    const struct json_handler_t*   handler;
    void*                          handler_obj;
    struct json_type_lib_t         type_lib;
    struct json_obj_t*             json_obj;
    struct json_type_stack_t       obj_stack;
    const struct json_type_node_t* root_node;
    enum json_parse_status_t       current_stat;
    struct json_type_error_info_t  error_info;

    bits_t                         canceled: 1;
    bits_t                         done: 1;
};

#define JSON_TYPE_CALL_HANDLER(n, ...) \
    ({                                 \
        bool __r =                     \
          type->handler &&             \
          type->handler->n ## _func    \
        ? type->handler->n ## _func(   \
            type->handler_obj,         \
            ## __VA_ARGS__)            \
        : true;                        \
        if (!__r)                      \
            type->canceled = true;     \
        __r;                           \
    })

#define JSON_TYPE_CHECK_ERROR_(e)        \
    ({                                   \
        type->error_info.type =          \
            json_type_error_type_check;  \
        type->error_info.type_check = e; \
        type->current_stat =             \
            json_parse_status_error;     \
        false;                           \
    })

#define JSON_TYPE_CHECK_ERROR(e, t)              \
    ({                                           \
        STATIC(                                  \
            json_type_check_error_ ## t !=       \
            json_type_check_error_none);         \
        e.type =                                 \
            json_type_check_error_ ## t;         \
        e.arg.type =                             \
            json_type_check_error_arg_none_type; \
        JSON_TYPE_CHECK_ERROR_(e);               \
    })

#define JSON_TYPE_CHECK_ERROR_KEY(e, a, t)       \
    ({                                           \
        STATIC(                                  \
            json_type_check_error_ ## t !=       \
            json_type_check_error_none);         \
        e.type =                                 \
            json_type_check_error_ ## t;         \
        e.arg.type =                             \
            json_type_check_error_arg_key_type;  \
        e.arg.key = a;                           \
        JSON_TYPE_CHECK_ERROR_(e);               \
    })

#define JSON_TYPE_CHECK_ERROR_CODE(e)                 \
    ({                                                \
        ASSERT(e.type != json_type_check_error_none); \
        JSON_TYPE_CHECK_ERROR_(e);                    \
    })

#define JSON_TYPE_CHECK_NODE(n, v, t)              \
    ({                                             \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_CONST(n)); \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_CONST(v)); \
        ASSERT(!JSON_TYPE_NODE_IS_CONST(n, list)); \
        JSON_TYPE_NODE_IS_CONST(n, t) &&           \
        json_type_check_ ## t(&((n)->node.t), v)   \
            ? &((n)->node.t)                       \
            : NULL;                                \
    })

#define JSON_TYPE_CHECK_IS_NODE(p, n) \
    TYPEOF_IS(p, const struct json_type_ ## n ## _node_t*)

#undef  CASE
#define CASE(n) \
    [json_type_any_ ## n ## _type] = \
    &json_type_any_ ## n ## _node

static const struct json_type_node_t* const
    json_type_any_nodes[] = {
        CASE(type),
        CASE(null),
        CASE(boolean),
        CASE(number),
        CASE(string),
        CASE(object),
        CASE(array)
    };

#define JSON_TYPE_TYPEOF_IS_CHECK_ERROR(e) \
    TYPEOF_IS(e, struct json_type_check_error_info_t)

#define JSON_TYPE_CHECK_NODE_(t, e)                       \
    ({                                                    \
        JSON_TYPE_CHECK_ ## t ## _DECL(__v);              \
        STATIC(JSON_TYPE_TYPEOF_IS_CHECK_ERROR(e));       \
        json_type_check_node(type, &__v, &e);             \
        PRINT_DEBUG("type check: t=%s e.type=%s", #t,     \
            json_type_check_error_type_get_name(e.type)); \
        e.type == json_type_check_error_none;             \
    })

#define JSON_TYPE_CHECK_NULL_NODE(e) \
    JSON_TYPE_CHECK_NODE_(NUL, e)
#define JSON_TYPE_CHECK_BOOLEAN_NODE(e) \
    JSON_TYPE_CHECK_NODE_(BOOLEAN, e)
#define JSON_TYPE_CHECK_NUMBER_NODE(e) \
    JSON_TYPE_CHECK_NODE_(NUMBER, e)
#define JSON_TYPE_CHECK_STRING_NODE(e) \
    JSON_TYPE_CHECK_NODE_(STRING, e)
#define JSON_TYPE_CHECK_OBJECT_NODE(e) \
    JSON_TYPE_CHECK_NODE_(OBJECT, e)
#define JSON_TYPE_CHECK_ARRAY_NODE(e) \
    JSON_TYPE_CHECK_NODE_(ARRAY, e)

#define JSON_TYPE_CHECK_LIST_(p, a)                \
    ({                                             \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_CONST(p)); \
        ASSERT(JSON_TYPE_NODE_IS_CONST(p, list));  \
          (p)->attr.list->a != NULL                \
        ? (p)->attr.list->a->root                  \
        : NULL;                                    \
    })

#define JSON_TYPE_CHECK_LIST_OBJECT(p) \
    JSON_TYPE_CHECK_LIST_(p, object)

#define JSON_TYPE_CHECK_LIST_ARRAY(p, c)         \
    ({                                           \
        const struct json_type_trie_node_t* __r; \
        if (!(  c = !!(                          \
              __r = JSON_TYPE_CHECK_LIST_(       \
                        p, closed_array))))      \
              __r = JSON_TYPE_CHECK_LIST_(       \
                        p, open_array);          \
        __r;                                     \
    })

union json_type_obj_pack_t
{
    const struct json_type_node_t* node;
    struct json_type_obj_args_t*   args;
};

union json_type_obj_args_pack_t
{
    const struct json_type_object_node_t*      object;
    const struct json_type_array_node_t*       array;
    const struct json_type_object_trie_node_t* object_trie;
    const struct json_type_trie_node_t*        array_trie;
};

#ifdef JSON_DEBUG

#undef  CASE
#define CASE(E) case json_type_obj_ ## E ## _type: return #E

static const char* json_type_obj_type_get_name(
    enum json_type_obj_type_t type)
{
    switch (type) {
    CASE(node);
    CASE(args);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

static void json_type_obj_print_debug(
    struct json_type_obj_t* obj, FILE* file)
{
    union json_type_obj_pack_t o;

    fprintf(file, "{.type=%s ",
        json_type_obj_type_get_name(obj->type));

    if ((o.node = JSON_TYPE_OBJ_AS_IF_NODE(obj))) {
        fputs("# node ", file);
        json_type_node_print_debug(o.node, file);
    }
    else
    if ((o.args = JSON_TYPE_OBJ_AS_IF_ARGS(obj))) {
        union json_type_obj_args_pack_t a;

        fputs("# args {", file);

        // stev: not using JSON_TYPE_OBJ_ARGS_AS_IF(o.args, ...)
        // because each of 'o.args->object', 'o.args->array',
        // 'o.args->object_trie' or 'o.args->array_trie'
        // can meaningfully be 'NULL'

        if (JSON_TYPE_OBJ_ARGS_IS(o.args, object)) {
            a.object = o.args->object;

            fputs(".object=", file);
            json_type_object_node_null_print_debug(
                a.object, file);
        }
        else
        if (JSON_TYPE_OBJ_ARGS_IS(o.args, array)) {
            a.array = o.args->array;

            fputs(".array=", file);

            if (a.array == NULL ||
                JSON_TYPE_ARRAY_NODE_IS_CONST(a.array, open) ||
                JSON_TYPE_ARRAY_NODE_IS_CONST(a.array, closed))
                json_type_array_node_null_print_debug(a.array, file);
            else
                UNEXPECT_VAR("%d", a.array->type);
        }
        else
        if (JSON_TYPE_OBJ_ARGS_IS(o.args, object_trie)) {
            a.object_trie = o.args->object_trie;

            fputs(".object_trie=", file);
            json_type_object_trie_print_node(a.object_trie, file);
        }
        else
        if (JSON_TYPE_OBJ_ARGS_IS(o.args, array_trie)) {
            a.array_trie = o.args->array_trie;

            fputs(".array_trie=", file);
            json_type_trie_print_node(a.array_trie, file);
        }
        else
            UNEXPECT_VAR("%d", o.args->type);

        if (JSON_TYPE_OBJ_ARGS_IS(o.args, object_trie) ||
            JSON_TYPE_OBJ_ARGS_IS(o.args, array_trie))
            fputs(" .attr={.base=", file);

        if (JSON_TYPE_OBJ_ARGS_IS(o.args, object_trie)) {
            const struct json_type_obj_args_object_trie_attr_t* a =
                &o.args->attr.object_trie;

            ASSERT(a->base != NULL);
            json_type_node_print_debug(a->base, file);
        }
        else
        if (JSON_TYPE_OBJ_ARGS_IS(o.args, array_trie)) {
            const struct json_type_obj_args_array_trie_attr_t* a =
                &o.args->attr.array_trie;

            ASSERT(a->base != NULL);
            json_type_node_print_debug(a->base, file);
            fputs(" .open_node=", file);
            json_type_node_null_print_debug(a->open_node, file);
            fputs(" .open_trie=", file);
            json_type_trie_print_node(a->open_trie, file);
            fprintf(file, " .is_closed=%d", a->is_closed);
        }

        if (JSON_TYPE_OBJ_ARGS_IS(o.args, object_trie) ||
            JSON_TYPE_OBJ_ARGS_IS(o.args, array_trie))
            fputc('}', file);

        fprintf(file, " .n_arg=%zu}", o.args->n_arg);
    }
    else
        UNEXPECT_VAR("%d", obj->type);

    fprintf(file, " .pos={.line=%zu .col=%zu}}",
        obj->pos.line, obj->pos.col);
}

#endif

static const struct json_type_node_t*
    json_type_node_any(
        const struct json_type_node_t* node,
        bool null)
{
    union json_type_node_pack_t p;

    ASSERT(node != NULL);

    if ((p.any = JSON_TYPE_NODE_AS_IF_CONST(node, any))) {
        if (JSON_TYPE_ANY_NODE_IS_CONST(p.any, object) ||
            JSON_TYPE_ANY_NODE_IS_CONST(p.any, array))
            return null
                ? NULL //!!!HACKISH
                : node;

        UNEXPECT_VAR("%d", p.any->type);
    }
    else
    if ((p.plain = JSON_TYPE_NODE_AS_IF_CONST(node, plain))) {
        const struct json_type_node_t* r;

        STATIC(
            json_type_any_null_type ==
            json_type_plain_null_type + 1 &&

            json_type_any_boolean_type ==
            json_type_plain_boolean_type + 1 &&

            json_type_any_number_type ==
            json_type_plain_number_type + 1 &&

            json_type_any_string_type ==
            json_type_plain_string_type + 1);

        r = ARRAY_NULL_ELEM(
            json_type_any_nodes,
            p.plain->type + 1);
        ASSERT(r != NULL);

        return r;
    }
    UNEXPECT_VAR("%d", node->type);
}

static bool json_type_check_any(
    const struct json_type_any_node_t* node,
    const struct json_type_node_t* input)
{
    if (node->type == json_type_any_type_type)
        return true;
    else {
        const struct json_type_node_t* e;
        const struct json_type_any_node_t* a;

        e = json_type_node_any(input, false);
        a = JSON_TYPE_NODE_AS_CONST(e, any);
        return node->type == a->type;
    }
}

static bool json_type_check_plain(
    const struct json_type_plain_node_t* node,
    const struct json_type_node_t* input)
{
    const struct json_type_plain_node_t* p;

    return
        (p = JSON_TYPE_NODE_AS_IF_CONST(input, plain)) &&
        json_type_plain_node_cmp(node, p) ==
        trie_sym_cmp_eq;
}

static const struct json_type_any_node_t*
    json_type_check_list_any(
        const struct json_type_node_t* node,
        const struct json_type_node_t* input)
{
    const struct json_type_any_node_t* r = NULL;

    ASSERT(JSON_TYPE_NODE_IS_CONST(node, list));

    if (node->attr.list->any != NULL) {
        const struct json_type_trie_node_t* t = NULL;

        if (json_type_trie_lookup_sym(
                node->attr.list->any,
                json_type_node_any(input, false),
                &t) ||
            json_type_trie_lookup_sym(
                node->attr.list->any,
                &json_type_any_type_node,
                &t)) {
            const struct json_type_node_t* n;

            ASSERT(t != NULL);
            n = JSON_TYPE_TRIE_NODE_AS_VAL(t);
            ASSERT(n != NULL);
            r = JSON_TYPE_NODE_AS_CONST(n, any);
            ASSERT(r != NULL);
        }
        else
            ASSERT(t == NULL);
    }

    return r;
}

static const struct json_type_plain_node_t*
    json_type_check_list_plain(
        const struct json_type_node_t* node,
        const struct json_type_node_t* input)
{
    const struct json_type_plain_node_t* r = NULL;

    ASSERT(JSON_TYPE_NODE_IS_CONST(node, list));
    ASSERT(JSON_TYPE_NODE_IS_CONST(input, plain));

    if (node->attr.list->plain != NULL) {
        const struct json_type_trie_node_t* t = NULL;

        if (json_type_trie_lookup_sym(
                node->attr.list->plain,
                input, &t)) {
            const struct json_type_node_t* n;

            ASSERT(t != NULL);
            n = JSON_TYPE_TRIE_NODE_AS_VAL(t);
            ASSERT(n != NULL);
            r = JSON_TYPE_NODE_AS_CONST(n, plain);
            ASSERT(r != NULL);
        }
        else
            ASSERT(t == NULL);
    }

    return r;
}

#ifdef JSON_DEBUG
#define JSON_TYPE_CHECK_DEBUG_NODE__(v)             \
    do {                                            \
        json_type_node_null_print_debug(v, stderr); \
    } while (0)
#define JSON_TYPE_CHECK_DEBUG_NODE_(n, v) \
    do {                                  \
        PRINT_DEBUG_BEGIN(#n "=");        \
        JSON_TYPE_CHECK_DEBUG_NODE__(v);  \
        PRINT_DEBUG_END();                \
    } while (0)
#else
#define JSON_TYPE_CHECK_DEBUG_NODE__(n, v) \
    do {} while (0)
#define JSON_TYPE_CHECK_DEBUG_NODE_(n, v) \
    do {} while (0)
#endif

#define JSON_TYPE_CHECK_DEBUG_NODE(n) \
    JSON_TYPE_CHECK_DEBUG_NODE_(n, n)

static enum json_type_check_error_type_t
    json_type_check_base(
        struct json_type_t* type,
        const struct json_type_node_t* input,
        const struct json_type_node_t* node,
        const struct json_text_pos_t* pos)
{
    const struct json_type_any_node_t* a;

    ASSERT(node != NULL);
    ASSERT(input != NULL);

    JSON_TYPE_CHECK_DEBUG_NODE(node);

    if (JSON_TYPE_NODE_IS_CONST(input, plain)) {
        if (JSON_TYPE_NODE_IS_CONST(node, list)) {
            if (!json_type_check_list_any(node, input) &&
                !json_type_check_list_plain(node, input))
                return json_type_check_error_type_mismatch;
        }
        else {
            if (!JSON_TYPE_CHECK_NODE(node, input, any) &&
                !JSON_TYPE_CHECK_NODE(node, input, plain))
                return json_type_check_error_type_mismatch;
        }
    }
    else
    if ((a = JSON_TYPE_NODE_AS_IF_CONST(input, any))) {
        if (JSON_TYPE_ANY_NODE_IS_CONST(a, object)) {
            if (JSON_TYPE_NODE_IS_CONST(node, list)) {
                const struct json_type_object_trie_node_t* n = NULL;
                struct json_type_obj_args_t* o;

                if (!json_type_check_list_any(node, input) &&
                    !(n = JSON_TYPE_CHECK_LIST_OBJECT(node)))
                    return json_type_check_error_type_mismatch;

                o = JSON_TYPE_STACK_PUSH_OBJECT_TRIE(node, n, *pos);
                ASSERT(o->n_arg == 0);
            }
            else {
                const struct json_type_object_node_t* n = NULL;
                struct json_type_obj_args_t* o;

                if (!JSON_TYPE_CHECK_NODE(node, input, any) &&
                    !(n = JSON_TYPE_NODE_AS_IF_CONST(node, object)))
                    return json_type_check_error_type_mismatch;

                o = JSON_TYPE_STACK_PUSH_ARGS(object, n, *pos);
                ASSERT(o->n_arg == 0);
            }
        }
        else
        if (JSON_TYPE_ANY_NODE_IS_CONST(a, array)) {
            if (JSON_TYPE_NODE_IS_CONST(node, list)) {
                const struct json_type_array_trie_node_t* n = NULL;
                struct json_type_obj_args_t* o;
                bool c = false;

                if (!json_type_check_list_any(node, input) &&
                    !(n = JSON_TYPE_CHECK_LIST_ARRAY(node, c)))
                    return json_type_check_error_type_mismatch;

                o = JSON_TYPE_STACK_PUSH_ARRAY_TRIE(node, n, c, *pos);
                ASSERT(o->n_arg == 0);
            }
            else {
                const struct json_type_array_node_t* n = NULL;
                union json_type_array_node_pack_t p;
                struct json_type_obj_args_t* o;

                if (!JSON_TYPE_CHECK_NODE(node, input, any) &&
                    !(n = JSON_TYPE_NODE_AS_IF_CONST(node, array)))
                    return json_type_check_error_type_mismatch;

                o = JSON_TYPE_STACK_PUSH_ARGS(array, n, *pos);
                if ((n == NULL) ||
                    (p.open =
                        JSON_TYPE_ARRAY_NODE_AS_IF_CONST(n, open)))
                    ; // stev: nop
                else
                if ((p.closed =
                        JSON_TYPE_ARRAY_NODE_AS_IF_CONST(n, closed))) {
                    if (p.closed->size > 0)
                        ASSERT(o->n_arg == 0);
                }
                else
                    UNEXPECT_VAR("%d", n->type);
            }
        }
        else
            UNEXPECT_VAR("%d", a->type);
    }
    else
        UNEXPECT_VAR("%d", input->type);

    return json_type_check_error_none;
}

#ifdef JSON_DEBUG
#define JSON_TYPE_TRIE_PRINT_NODE(t, n, f)       \
    do {                                         \
        json_type ## t ## trie_print_node(n, f); \
    } while (0)
#else
#define JSON_TYPE_TRIE_PRINT_NODE(t, n, f) \
    do {} while (0)
#endif

#define JSON_TYPE_TRIE_LOOKUP_SYM(t, e, n)               \
    ({                                                   \
        const struct json_type_node_t* __e = (e);        \
        bool __r = json_type_trie_lookup_sym(t, __e, n); \
        PRINT_DEBUG_BEGIN("trie lookup sym: node=");     \
        JSON_TYPE_CHECK_DEBUG_NODE__(__e);               \
        PRINT_DEBUG_FMT(" ret=%d result=", __r);         \
        JSON_TYPE_TRIE_PRINT_NODE(_, *n, stderr);        \
        PRINT_DEBUG_END();                               \
        __r;                                             \
    })

#define JSON_TYPE_TRIE_NODE_HAS_NULL_SIB_(t, n)          \
    ({                                                   \
        bool __r =                                       \
            json_type_trie_node_has ## t ## null_sib(n); \
        PRINT_DEBUG("has" #t "null_sib=%d", __r);        \
        __r;                                             \
    })
#define JSON_TYPE_TRIE_NODE_HAS_NON_NULL_SIB(n) \
    JSON_TYPE_TRIE_NODE_HAS_NULL_SIB_(_non_, n)
#define JSON_TYPE_TRIE_NODE_HAS_NULL_SIB(n) \
    JSON_TYPE_TRIE_NODE_HAS_NULL_SIB_(_, n)

#ifdef JSON_DEBUG
#define JSON_TYPE_CHECK_DEBUG_SYM_NODE(s)             \
    do {                                              \
        json_type_object_sym_print_debug(&s, stderr); \
    } while (0)
#define JSON_TYPE_CHECK_DEBUG_LOOKUP_SYM_NULL(t, v)   \
    do {                                              \
        if (__v == NULL)                              \
            PRINT_DEBUG(                              \
                "%strie lookup sym node: node=(nil)", \
                #t);                                  \
    } while (0)
#else
#define JSON_TYPE_CHECK_DEBUG_SYM_NODE(s) \
    do {} while (0)
#define JSON_TYPE_CHECK_DEBUG_LOOKUP_SYM_NULL(t, v) \
    do {} while (0)
#endif

#define JSON_TYPE_OBJECT_TRIE_LOOKUP_SYM_NODE_(t, n, v, r)         \
    ({                                                             \
        struct json_type_object_sym_t __k = {                      \
            .type = json_type_object_sym_ ## n ## _type,           \
            .val.n = v                                             \
        };                                                         \
        bool __r =                                                 \
            json_type_object_trie_lookup_sym_node(t, __k, r);      \
        PRINT_DEBUG_BEGIN("object_trie lookup sym node: %s=", #n); \
        JSON_TYPE_CHECK_DEBUG_SYM_NODE(__k);                       \
        PRINT_DEBUG_FMT(" ret=%d result=", __r);                   \
        JSON_TYPE_TRIE_PRINT_NODE(_object_, *r, stderr);           \
        PRINT_DEBUG_END();                                         \
        __r;                                                       \
    })
#define JSON_TYPE_OBJECT_TRIE_LOOKUP_SYM_NODE(t, v, r) \
    JSON_TYPE_OBJECT_TRIE_LOOKUP_SYM_NODE_(t, node, v, r)
#define JSON_TYPE_OBJECT_TRIE_LOOKUP_SYM_NAME(t, v, r) \
    JSON_TYPE_OBJECT_TRIE_LOOKUP_SYM_NODE_(t, name, v, r)

#define JSON_TYPE_OBJECT_TRIE_LOOKUP_SYM_NODE_NULL(t, v, r)  \
    ({                                                       \
        const struct json_type_node_t* __v = (v);            \
        JSON_TYPE_CHECK_DEBUG_LOOKUP_SYM_NULL(object_, __v); \
        __v != NULL &&                                       \
        JSON_TYPE_OBJECT_TRIE_LOOKUP_SYM_NODE(t, __v, r);    \
    })

#define JSON_TYPE_TRIE_LOOKUP_SYM_NODE(t, v, r)             \
    ({                                                      \
        bool __r = json_type_trie_lookup_sym_node(t, v, r); \
        PRINT_DEBUG_BEGIN("trie lookup sym node: node=");   \
        JSON_TYPE_CHECK_DEBUG_NODE__(v);                    \
        PRINT_DEBUG_FMT(" ret=%d result=", __r);            \
        JSON_TYPE_TRIE_PRINT_NODE(_, *r, stderr);           \
        PRINT_DEBUG_END();                                  \
        __r;                                                \
    })
#define JSON_TYPE_TRIE_LOOKUP_SYM_NODE_NULL(t, v, r)  \
    ({                                                \
        const struct json_type_node_t* __v = (v);     \
        JSON_TYPE_CHECK_DEBUG_LOOKUP_SYM_NULL(, __v); \
        __v != NULL &&                                \
        JSON_TYPE_TRIE_LOOKUP_SYM_NODE(t, __v, r);    \
    })

#define JSON_TYPE_OBJECT_TRIE_SYM_UNEXPECT_TYPE(s) \
    do {                                           \
        STATIC(JSON_TYPE_TYPEOF_IS_OBJECT_SYM(s)); \
        UNEXPECT_VAR("%d", s.type);                \
    } while (0)
#define JSON_TYPE_TRIE_SYM_UNEXPECT_TYPE(s)        \
    do {                                           \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_CONST(s)); \
        UNEXPECT_VAR("%p", s);                     \
    } while (0)

#define JSON_TYPE_TRIE_IS_SYM_FUNC_(m, n, t)                    \
static bool json_type ## n ## trie_is_ ## t ## _sym(            \
    const void* obj UNUSED,                                     \
    const struct json_type ## n ## trie_node_t* node)           \
{                                                               \
    const struct json_type_node_t* v;                           \
                                                                \
    if (JSON_TYPE ## m ## TRIE_SYM_IS_NULL(node->sym))          \
        return false;                                           \
    if ((v = JSON_TYPE ## m ## TRIE_SYM_AS_IF_NODE(node->sym))) \
        return JSON_TYPE_NODE_IS_CONST(v, t);                   \
    else                                                        \
        JSON_TYPE ## m ## TRIE_SYM_UNEXPECT_TYPE(node->sym);    \
}
#define JSON_TYPE_OBJECT_TRIE_IS_SYM_FUNC(t) \
    JSON_TYPE_TRIE_IS_SYM_FUNC_(_OBJECT_, _object_, t)
#define JSON_TYPE_TRIE_IS_SYM_FUNC(t) \
    JSON_TYPE_TRIE_IS_SYM_FUNC_(_, _, t)

JSON_TYPE_OBJECT_TRIE_IS_SYM_FUNC(object)
JSON_TYPE_OBJECT_TRIE_IS_SYM_FUNC(array)

JSON_TYPE_TRIE_IS_SYM_FUNC(object)
JSON_TYPE_TRIE_IS_SYM_FUNC(array)

#define JSON_TYPE_TRIE_LOOKUP_FIRST_NODE_(n, t, v, r)       \
    ({                                                      \
        bool __r = false;                                   \
        const struct json_type_any_node_t* __n;             \
        ASSERT(v != NULL);                                  \
        if ((__n = JSON_TYPE_NODE_AS_IF_CONST(v, any))) {   \
            json_type ## n ## trie_node_cond_func_t __f;    \
            if (JSON_TYPE_ANY_NODE_IS_CONST(__n, object))   \
                __f = json_type ## n ## trie_is_object_sym; \
            else                                            \
            if (JSON_TYPE_ANY_NODE_IS_CONST(__n, array))    \
                __f = json_type ## n ## trie_is_array_sym;  \
            else                                            \
                UNEXPECT_VAR("%d", v->node.any.type);       \
            __r = json_type ## n ## trie_lookup_first_node( \
                    t, __f, NULL, r);                       \
        }                                                   \
        PRINT_DEBUG_BEGIN(                                  \
            "%strie lookup 1st node: node=", #n + 1);       \
        JSON_TYPE_CHECK_DEBUG_NODE__(v);                    \
        PRINT_DEBUG_FMT(" ret=%d result=", __r);            \
        JSON_TYPE_TRIE_PRINT_NODE(n, *r, stderr);           \
        PRINT_DEBUG_END();                                  \
        __r;                                                \
    })
#define JSON_TYPE_OBJECT_TRIE_LOOKUP_FIRST_NODE(t, v, r) \
    JSON_TYPE_TRIE_LOOKUP_FIRST_NODE_(_object_, t, v, r)
#define JSON_TYPE_TRIE_LOOKUP_FIRST_NODE(t, v, r) \
    JSON_TYPE_TRIE_LOOKUP_FIRST_NODE_(_, t, v, r)

#define JSON_TYPE_CHECK_LOOKUP_TRIE_NODE_(n, t, v, r) \
    (                                                 \
        JSON_TYPE ## n ## TRIE_LOOKUP_SYM_NODE(       \
            t, v, r) ||                               \
        JSON_TYPE ## n ## TRIE_LOOKUP_SYM_NODE_NULL(  \
            t, json_type_node_any(v, true), r) ||     \
        JSON_TYPE ## n ## TRIE_LOOKUP_SYM_NODE(       \
            t, &json_type_any_type_node, r) ||        \
        JSON_TYPE ## n ## TRIE_LOOKUP_FIRST_NODE(     \
            t, v, r)                                  \
    )
#define JSON_TYPE_CHECK_LOOKUP_OBJECT_TRIE_NODE(t, v, r) \
    JSON_TYPE_CHECK_LOOKUP_TRIE_NODE_(_OBJECT_, t, v, r)
#define JSON_TYPE_CHECK_LOOKUP_TRIE_NODE(t, v, r) \
    JSON_TYPE_CHECK_LOOKUP_TRIE_NODE_(_, t, v, r)

#ifdef JSON_DEBUG
#define JSON_TYPE_CHECK_DEBUG_TRIE_(t, n, v)          \
    do {                                              \
        PRINT_DEBUG_BEGIN(#n "=");                    \
        json_type ## t ## trie_print_node(v, stderr); \
        PRINT_DEBUG_END();                            \
    } while (0)
#define JSON_TYPE_CHECK_DEBUG_RESULT(e)              \
    do {                                             \
        PRINT_DEBUG_BEGIN("error=%s",                \
            json_type_check_error_type_get_name(e)); \
        PRINT_DEBUG_STR(" result=");                 \
        json_type_check_error_arg_print_error_debug( \
            &err->arg, stderr);                      \
        PRINT_DEBUG_END();                           \
    } while (0)
#else
#define JSON_TYPE_CHECK_DEBUG_TRIE_(t, n, v) \
    do {} while (0)
#define JSON_TYPE_CHECK_DEBUG_RESULT(e) \
    do {} while (0)
#endif

#define JSON_TYPE_CHECK_DEBUG_TRIE(n) \
    JSON_TYPE_CHECK_DEBUG_TRIE_(_, n, n)
#define JSON_TYPE_CHECK_DEBUG_OBJECT_TRIE(n) \
    JSON_TYPE_CHECK_DEBUG_TRIE_(_object_, n, n)

#define JSON_TYPE_CHECK_NODE_RESULT__(t, v)     \
    do {                                        \
        err->arg.type =                         \
            json_type_check_error_arg_val_type; \
        err->arg.val.type =                     \
            json_type_check_error_val_arg_ ## t \
            ## _type;                           \
        err->arg.val.t = v;                     \
    } while (0)

#define JSON_TYPE_CHECK_NODE_RESULT_(n)          \
    ({                                           \
        err->type = json_type_check_base(        \
            type, input, n, &err->pos);          \
        JSON_TYPE_CHECK_NODE_RESULT__(node, n);  \
        JSON_TYPE_CHECK_DEBUG_RESULT(err->type); \
        err->type == json_type_check_error_none; \
    })
#define JSON_TYPE_CHECK_ERROR_RESULT_(t, v, e)   \
    ({                                           \
        err->type = e;                           \
        JSON_TYPE_CHECK_NODE_RESULT__(t, v);     \
        JSON_TYPE_CHECK_DEBUG_RESULT(err->type); \
        STATIC(e != json_type_check_error_none); \
        false;                                   \
    })
#define JSON_TYPE_CHECK_ERROR_ARG_RESULT(t, v, e) \
    JSON_TYPE_CHECK_ERROR_RESULT_(t, v, json_type_check_error_ ## e)
#define JSON_TYPE_CHECK_ERROR_RESULT(e) \
    JSON_TYPE_CHECK_ERROR_RESULT_(node, NULL, json_type_check_error_ ## e)
#define JSON_TYPE_CHECK_NODE_RESULT(n) \
    JSON_TYPE_CHECK_NODE_RESULT_(n)
#define JSON_TYPE_CHECK_ANY_RESULT() \
    JSON_TYPE_CHECK_NODE_RESULT(&json_type_any_type_node)

static bool json_type_check_node(
    struct json_type_t* type,
    const struct json_type_node_t* input,
    struct json_type_check_error_info_t* err)
{
    struct json_type_obj_t* obj;
    union json_type_obj_pack_t o;

    JSON_TYPE_CHECK_DEBUG_NODE(input);

    obj = JSON_TYPE_STACK_TOP_REF();

    if ((o.node = JSON_TYPE_OBJ_AS_IF_NODE(obj)))
        return JSON_TYPE_CHECK_NODE_RESULT(o.node);

    if ((o.args = JSON_TYPE_OBJ_AS_IF_ARGS(obj))) {
        union json_type_obj_args_pack_t a;

        // stev: not using JSON_TYPE_OBJ_ARGS_AS_IF(o.args, ...)
        // because each of 'o.args->object', 'o.args->array',
        // 'o.args->object_trie' or 'o.args->array_trie'
        // can meaningfully be 'NULL'

        if (JSON_TYPE_OBJ_ARGS_IS(o.args, object)) {
            a.object = o.args->object;

            ENSURE((o.args->n_arg % 2) == 1,
                "invalid type stack: top element not "
                "an 'object val'");

            if (a.object == NULL) {
                o.args->n_arg ++;
                return JSON_TYPE_CHECK_ANY_RESULT();
            }
            else {
                size_t k = o.args->n_arg / 2;
                return
                      a.object->size > k
                    ? o.args->n_arg ++,
                      JSON_TYPE_CHECK_NODE_RESULT(
                        a.object->args[k].type)
                    : JSON_TYPE_CHECK_ERROR_RESULT(
                        too_many_args);
            }
        }
        else
        if (JSON_TYPE_OBJ_ARGS_IS(o.args, array)) {
            union json_type_array_node_pack_t n;

            a.array = o.args->array;

            if (a.array == NULL)
                return JSON_TYPE_CHECK_ANY_RESULT();
            if ((n.open =
                    JSON_TYPE_ARRAY_NODE_AS_IF_CONST(a.array, open)))
                return JSON_TYPE_CHECK_NODE_RESULT(n.open->arg);
            if ((n.closed =
                    JSON_TYPE_ARRAY_NODE_AS_IF_CONST(a.array, closed))) {
                size_t k = o.args->n_arg;
                return
                      n.closed->size > k
                    ? o.args->n_arg ++,
                      JSON_TYPE_CHECK_NODE_RESULT(
                        n.closed->args[k])
                    : JSON_TYPE_CHECK_ERROR_RESULT(
                        too_many_args);
            }
            UNEXPECT_VAR("%d", a.array->type);
        }
        else
        if (JSON_TYPE_OBJ_ARGS_IS(o.args, object_trie)) {
            struct json_type_obj_args_object_trie_attr_t* p;

            a.object_trie = o.args->object_trie;
            p = &o.args->attr.object_trie;

            ENSURE(JSON_TYPE_NODE_IS_CONST(p->base, list),
                "invalid type stack: top element's object trie "
                "base not a 'list'");

            if (a.object_trie == NULL)
                return JSON_TYPE_CHECK_ANY_RESULT();
            else {
                const struct json_type_object_sym_t* s =
                    &a.object_trie->sym;
                const struct json_type_object_trie_node_t* n = NULL;
                const struct json_type_node_t* r;

                ENSURE(
                    s->type == json_type_object_sym_node_type,
                    "invalid type stack: top element not "
                    "an 'object val'");

                if (!JSON_TYPE_CHECK_LOOKUP_OBJECT_TRIE_NODE(
                        a.object_trie, input, &n))
                    return JSON_TYPE_CHECK_ERROR_ARG_RESULT(
                        object_trie, a.object_trie,
                        type_mismatch);

                JSON_TYPE_CHECK_DEBUG_OBJECT_TRIE(n);

                ASSERT(n != NULL);
                r = JSON_TYPE_OBJECT_TRIE_SYM_AS(n->sym, node);
                n = JSON_TYPE_OBJECT_TRIE_NODE_AS_EQ(n);

                ASSERT(n != NULL);
                // stev: n must be a 'null' or a 'name' object sym
                ASSERT(!JSON_TYPE_OBJECT_TRIE_SYM_IS(n->sym, node));

                o.args->object_trie = n;

                return JSON_TYPE_CHECK_NODE_RESULT(r);
            }
        }
        else
        if (JSON_TYPE_OBJ_ARGS_IS(o.args, array_trie)) {
            struct json_type_obj_args_array_trie_attr_t* p;
            const struct json_type_trie_t* q = NULL;

            a.array_trie = o.args->array_trie;
            p = &o.args->attr.array_trie;

            ENSURE(JSON_TYPE_NODE_IS_CONST(p->base, list),
                "invalid type stack: top element's array trie "
                "base not a 'list'");

            o.args->n_arg ++;

            if (p->is_closed && o.args->n_arg == 1) {
                q = p->base->attr.list->open_array;

                ASSERT(p->open_node == NULL);
                ASSERT(p->open_trie == NULL);

                if (q != NULL) {
                    ASSERT(q->root != NULL);

                    if (JSON_TYPE_CHECK_LOOKUP_TRIE_NODE(
                            q->root, input, &p->open_trie)) {
                        const struct json_type_trie_node_t* t;
                        const struct json_type_open_array_node_t* r;
                        const struct json_type_array_node_t* s;
                        const struct json_type_node_t* n;

                        ASSERT(p->open_trie != NULL);
                        t = JSON_TYPE_TRIE_NODE_AS_EQ(p->open_trie);

                        n = JSON_TYPE_TRIE_NODE_AS_VAL(t);
                        ASSERT(n != NULL);
                        s = JSON_TYPE_NODE_AS_CONST(n, array);
                        ASSERT(q != NULL);
                        r = JSON_TYPE_ARRAY_NODE_AS_CONST(s, open);
                        ASSERT(r != NULL);

                        ASSERT(r->arg != NULL);
                        p->open_node = r->arg;
                    }
                }
            }
            else
            if (p->is_closed && p->open_node != NULL) {
                const struct json_type_any_node_t* a;

                // stev: the type library attribute errors of kind
                // 'invalid_list_sort_of_..._of_..._not_supported',
                // where '...' stands for either 'object', 'array'
                // or 'list' -- which are raised by the functions
                // 'json_type_lib_check_..._not_supported', where
                // '...' is either 'arrays', 'object' or 'array'
                // -- ensure that the conditions below are true:
                ASSERT(
                    JSON_TYPE_NODE_IS_CONST(p->open_node, any) ||
                    JSON_TYPE_NODE_IS_CONST(p->open_node, plain));
                if ((a = JSON_TYPE_NODE_AS_IF_CONST(p->open_node, any)))
                    ASSERT(
                        !JSON_TYPE_ANY_NODE_IS_CONST(a, object) &&
                        !JSON_TYPE_ANY_NODE_IS_CONST(a, array));

                if (!JSON_TYPE_CHECK_NODE(p->open_node, input, any) &&
                    !JSON_TYPE_CHECK_NODE(p->open_node, input, plain))
                    p->open_node = NULL;
            }

            JSON_TYPE_OBJ_PRINT_DEBUG("obj=", obj);

            if (a.array_trie == NULL) //!!!ARRAY_TRIE_NULL
                return p->open_node != NULL
                    ? JSON_TYPE_CHECK_NODE_RESULT(p->open_node)
                    : JSON_TYPE_CHECK_ANY_RESULT();
            else {
                const struct json_type_trie_node_t* n = NULL;
                const struct json_type_node_t* r;

                ASSERT(p->is_closed || o.args->n_arg == 1);

                if (a.array_trie->sym == NULL &&
                    !JSON_TYPE_TRIE_NODE_HAS_NON_NULL_SIB(
                        a.array_trie)) {
                    if (p->open_node != NULL) {
                        p->is_closed = false;
                        o.args->array_trie = NULL; //!!!ARRAY_TRIE_NULL
                        return JSON_TYPE_CHECK_NODE_RESULT(
                            p->open_node);
                    }
                    else
                    if (p->open_trie != NULL) {
                        struct json_type_check_error_array_trie_val_arg_t e;

                        e.open = p->open_trie;
                        e.closed = NULL;
                        e.n_arg = SIZE_DEC(o.args->n_arg);

                        return JSON_TYPE_CHECK_ERROR_ARG_RESULT(
                            array_trie, e, type_mismatch);
                    }
                    else
                        return JSON_TYPE_CHECK_ERROR_RESULT(
                            too_many_args);
                }

                if (!JSON_TYPE_CHECK_LOOKUP_TRIE_NODE(
                        a.array_trie, input, &n)) {
                    ASSERT(n == NULL);

                    if (p->open_node == NULL) {
                        struct json_type_check_error_array_trie_val_arg_t e;

                        if (p->is_closed) {
                            e.open = q != NULL ? q->root : p->open_trie;
                            e.closed = a.array_trie;
                        }
                        else {
                            e.open = a.array_trie;
                            e.closed = NULL;
                        }
                        e.n_arg = SIZE_DEC(o.args->n_arg);

                        return JSON_TYPE_CHECK_ERROR_ARG_RESULT(
                            array_trie, e, type_mismatch);
                    }
                    r = p->open_node;
                }
                else {
                    ASSERT(n != NULL);

                    JSON_TYPE_CHECK_DEBUG_TRIE(n);

                    ASSERT(n->sym != NULL);
                    r = n->sym;

                    if (p->is_closed) {
                        n = JSON_TYPE_TRIE_NODE_AS_EQ(n);
                        ASSERT(n != NULL);
                    }
                    else {
                        p->open_node = r;
                        n = NULL;
                    }
                }

                if (n == NULL)
                    p->is_closed = false;
                o.args->array_trie = n; //!!!ARRAY_TRIE_NULL

                return JSON_TYPE_CHECK_NODE_RESULT(r);
            }
        }
        UNEXPECT_VAR("%d", o.args->type);
    }
    UNEXPECT_VAR("%d", obj->type);
}

static bool json_type_object_trie_is_name_sym(
    const void* obj UNUSED,
    const struct json_type_object_trie_node_t* node)
{
    if (JSON_TYPE_OBJECT_TRIE_SYM_IS(node->sym, null))
        return false;
    if (JSON_TYPE_OBJECT_TRIE_SYM_IS(node->sym, name))
        return true;
    else
        UNEXPECT_VAR("%d", node->sym.type);
}

#define JSON_TYPE_GET_CURRENT_TOK_POS(e)  \
    do {                                  \
        e.pos = json_get_current_tok_pos( \
            type->json_obj);              \
    } while (0)

#define JSON_TYPE_CHECK_PLAIN_DECL__(n, t, v)       \
    const struct json_type_node_t n = {             \
        .type = json_type_plain_node_type,          \
        .node.plain = {                             \
            .type = json_type_plain_ ## t ## _type, \
            v                                       \
        },                                          \
        .pos = {0, 0}                               \
    }
#define JSON_TYPE_CHECK_PLAIN_DECL_(n, t, v) \
    JSON_TYPE_CHECK_PLAIN_DECL__(n, t, .val.t = v)

#define JSON_TYPE_CHECK_NUL_DECL(n) \
    JSON_TYPE_CHECK_PLAIN_DECL__(n, null, )

static bool json_type_null(struct json_type_t* type)
{
    struct json_type_check_error_info_t e;

    JSON_TYPE_STACK_TOP_PRINT_DEBUG();

    JSON_TYPE_GET_CURRENT_TOK_POS(e);
    if (!JSON_TYPE_CHECK_NULL_NODE(e))
        return JSON_TYPE_CHECK_ERROR_CODE(e);

    return JSON_TYPE_CALL_HANDLER(null);
}

#define JSON_TYPE_CHECK_BOOLEAN_DECL(n) \
    JSON_TYPE_CHECK_PLAIN_DECL_(n, boolean, val)

static bool json_type_boolean(struct json_type_t* type, bool val)
{
    struct json_type_check_error_info_t e;

    PRINT_DEBUG("val=%d", val);
    JSON_TYPE_STACK_TOP_PRINT_DEBUG();

    JSON_TYPE_GET_CURRENT_TOK_POS(e);
    if (!JSON_TYPE_CHECK_BOOLEAN_NODE(e))
        return JSON_TYPE_CHECK_ERROR_CODE(e);

    return JSON_TYPE_CALL_HANDLER(boolean, val);
}

#define JSON_TYPE_CHECK_NUMBER_DECL(n) \
    JSON_TYPE_CHECK_PLAIN_DECL_(n, number, val)

static bool json_type_number(struct json_type_t* type, 
    const uchar_t* val, size_t len)
{
    struct json_type_check_error_info_t e;

#ifdef JSON_DEBUG
    PRINT_DEBUG_BEGIN("val=");
    json_type_string_len_print_debug(
        val, len, false, stderr);
    PRINT_DEBUG_END();
#endif
    JSON_TYPE_STACK_TOP_PRINT_DEBUG();

    JSON_TYPE_GET_CURRENT_TOK_POS(e);
    if (!JSON_TYPE_CHECK_NUMBER_NODE(e))
        return JSON_TYPE_CHECK_ERROR_CODE(e);

    return JSON_TYPE_CALL_HANDLER(number, val, len);
}

#define JSON_TYPE_CHECK_STRING_DECL(n) \
    JSON_TYPE_CHECK_PLAIN_DECL_(n, string, val)

static bool json_type_string(struct json_type_t* type,
    const uchar_t* val, size_t len)
{
    struct json_type_check_error_info_t e;

#ifdef JSON_DEBUG
    PRINT_DEBUG_BEGIN("val=");
    json_type_string_len_print_debug(
        val, len, true, stderr);
    PRINT_DEBUG_END();
#endif
    JSON_TYPE_STACK_TOP_PRINT_DEBUG();

    JSON_TYPE_GET_CURRENT_TOK_POS(e);
    if (!JSON_TYPE_CHECK_STRING_NODE(e))
        return JSON_TYPE_CHECK_ERROR_CODE(e);

    return JSON_TYPE_CALL_HANDLER(string, val, len);
}

#define JSON_TYPE_CHECK_ANY_DECL_(n, t) \
    const struct json_type_node_t n = \
        json_type_any_ ## t ## _node

#define JSON_TYPE_CHECK_OBJECT_DECL(n) \
    JSON_TYPE_CHECK_ANY_DECL_(n, object)

static bool json_type_object_start(struct json_type_t* type)
{
    struct json_type_check_error_info_t e;

    JSON_TYPE_STACK_TOP_PRINT_DEBUG();

    JSON_TYPE_GET_CURRENT_TOK_POS(e);
    if (!JSON_TYPE_CHECK_OBJECT_NODE(e))
        return JSON_TYPE_CHECK_ERROR_CODE(e);

    return JSON_TYPE_CALL_HANDLER(object_start);
}

static bool json_type_object_key(struct json_type_t* type,
    const uchar_t* key, size_t len)
{
    struct json_type_check_error_info_t e;
    struct json_type_obj_args_t* o;

#ifdef JSON_DEBUG
    PRINT_DEBUG_BEGIN("key=");
    json_type_string_len_print_debug(
        key, len, true, stderr);
    PRINT_DEBUG_END();
#endif
    JSON_TYPE_STACK_TOP_PRINT_DEBUG();

    JSON_TYPE_GET_CURRENT_TOK_POS(e);
    o = JSON_TYPE_STACK_TOP_REF_ENSURE_AS_ARGS(object);

    if (JSON_TYPE_OBJ_ARGS_IS(o, object)) {
        ENSURE((o->n_arg % 2) == 0,
            "invalid type stack: top element not an 'object key'");

        if (o->object != NULL) {
            size_t k = o->n_arg / 2;

            if (k >= o->object->size)
                return JSON_TYPE_CHECK_ERROR(e, too_many_args);

            if (!strlucmp(o->object->args[k].name, key, len)) {
                struct json_type_check_error_key_arg_t a;

                a.type = json_type_check_error_key_arg_name_type;
                a.name = o->object->args[k].name;

                return JSON_TYPE_CHECK_ERROR_KEY(
                    e, a, invalid_arg_name);
            }
        }

        o->n_arg ++;
    }
    else
    if (JSON_TYPE_OBJ_ARGS_IS(o, object_trie)) {
        if (o->object_trie != NULL) {
            const struct json_type_object_sym_t* s =
                &o->object_trie->sym;
            const struct json_type_object_trie_node_t* n = NULL;

            ENSURE(
                s->type == json_type_object_sym_null_type ||
                s->type == json_type_object_sym_name_type,
                "invalid type stack: top element not an 'object key'");

            // stev: note that the parser object
            // makes 'key' to be null-terminated
            if (!JSON_TYPE_OBJECT_TRIE_LOOKUP_SYM_NAME(
                    o->object_trie, key, &n)) {
                if (json_type_object_trie_lookup_first_node(
                        o->object_trie,
                        json_type_object_trie_is_name_sym,
                        NULL, NULL)) {
                    struct json_type_check_error_key_arg_t a;

                    a.type = json_type_check_error_key_arg_object_trie_type;
                    a.object_trie = o->object_trie;

                    return JSON_TYPE_CHECK_ERROR_KEY(e, a, invalid_arg_name);
                }
                else
                    return JSON_TYPE_CHECK_ERROR(e, too_many_args);
            }

#ifdef JSON_DEBUG
            PRINT_DEBUG_BEGIN("n=");
            json_type_object_trie_print_node(n, stderr);
            PRINT_DEBUG_END();
#endif

            ASSERT(n != NULL);
            ASSERT(n->sym.type == json_type_object_sym_name_type);

            n = JSON_TYPE_OBJECT_TRIE_NODE_AS_EQ(n);

            ASSERT(n != NULL);
            ASSERT(n->sym.type == json_type_object_sym_node_type);

            o->object_trie = n;
        }
    }
    else
        UNEXPECT_VAR("%d", o->type);

    return JSON_TYPE_CALL_HANDLER(object_key, key, len);
}

static bool json_type_object_sep(struct json_type_t* type)
{
    struct json_type_check_error_info_t e;
    struct json_type_obj_args_t* o;

    JSON_TYPE_STACK_TOP_PRINT_DEBUG();

    JSON_TYPE_GET_CURRENT_TOK_POS(e);
    o = JSON_TYPE_STACK_TOP_REF_ENSURE_AS_ARGS(object);

    if (JSON_TYPE_OBJ_ARGS_IS(o, object)) {
        ENSURE(o->n_arg > 0 && (o->n_arg % 2) == 0,
            "invalid type stack: top element not an 'object key'");

        if (o->object != NULL) {
            size_t k = o->n_arg / 2;

            if (k >= o->object->size)
                return JSON_TYPE_CHECK_ERROR(
                    e, too_many_args);
        }
    }
    else
    if (JSON_TYPE_OBJ_ARGS_IS(o, object_trie)) {
        if (o->object_trie != NULL) {
            const struct json_type_object_sym_t* s =
                &o->object_trie->sym;

            ENSURE(
                s->type == json_type_object_sym_null_type ||
                s->type == json_type_object_sym_name_type, 
                "invalid type stack: top element not an 'object key'");

            if (!json_type_object_trie_node_has_non_null_sib(
                    o->object_trie))
                return JSON_TYPE_CHECK_ERROR(
                    e, too_many_args);
        }
    }
    else
        UNEXPECT_VAR("%d", o->type);

    return JSON_TYPE_CALL_HANDLER(object_sep);
}

static bool json_type_object_end(struct json_type_t* type)
{
    struct json_type_check_error_info_t e;
    struct json_type_obj_args_t* o;

    JSON_TYPE_STACK_TOP_PRINT_DEBUG();

    JSON_TYPE_GET_CURRENT_TOK_POS(e);
    o = JSON_TYPE_STACK_TOP_REF_ENSURE_AS_ARGS(object);

    if (JSON_TYPE_OBJ_ARGS_IS(o, object)) {
        ENSURE((o->n_arg % 2) == 0,
            "invalid type stack: top element not an 'object key'");

        if (o->object != NULL) {
            size_t k = o->n_arg / 2;

            ASSERT(k <= o->object->size);
            if (k < o->object->size)
                return JSON_TYPE_CHECK_ERROR(
                    e, too_few_args);
        }
    }
    else
    if (JSON_TYPE_OBJ_ARGS_IS(o, object_trie)) {
        if (o->object_trie != NULL) {
            const struct json_type_object_sym_t* s =
                &o->object_trie->sym;

            ENSURE(
                s->type == json_type_object_sym_null_type ||
                s->type == json_type_object_sym_name_type,
                "invalid type stack: top element not an 'object key'");

            if (!json_type_object_trie_node_has_null_sib(
                    o->object_trie))
                return JSON_TYPE_CHECK_ERROR(
                    e, too_few_args);
        }
    }
    else
        UNEXPECT_VAR("%d", o->type);

    JSON_TYPE_STACK_POP_ARGS(object);

    return JSON_TYPE_CALL_HANDLER(object_end);
}

#define JSON_TYPE_CHECK_ARRAY_DECL(n) \
    JSON_TYPE_CHECK_ANY_DECL_(n, array)

static bool json_type_array_start(struct json_type_t* type)
{
    struct json_type_check_error_info_t e;

    JSON_TYPE_STACK_TOP_PRINT_DEBUG();

    JSON_TYPE_GET_CURRENT_TOK_POS(e);
    if (!JSON_TYPE_CHECK_ARRAY_NODE(e))
        return JSON_TYPE_CHECK_ERROR_CODE(e);

    return JSON_TYPE_CALL_HANDLER(array_start);
}

static bool json_type_array_sep(struct json_type_t* type)
{
    struct json_type_check_error_info_t e;
    struct json_type_obj_args_t* o;

    JSON_TYPE_STACK_TOP_PRINT_DEBUG();

    JSON_TYPE_GET_CURRENT_TOK_POS(e);
    o = JSON_TYPE_STACK_TOP_REF_ENSURE_AS_ARGS(array);

    if (JSON_TYPE_OBJ_ARGS_IS(o, array)) {
        union json_type_array_node_pack_t p;

        if ((o->array == NULL) ||
            (p.open =
                JSON_TYPE_ARRAY_NODE_AS_IF_CONST(o->array, open)))
            ; // stev: nop
        else
        if ((p.closed =
                JSON_TYPE_ARRAY_NODE_AS_IF_CONST(o->array, closed))) {
            if (o->n_arg >= p.closed->size)
                return JSON_TYPE_CHECK_ERROR(e, too_many_args);
        }
        else
            UNEXPECT_VAR("%d", o->array->type);
    }
    else
    if (JSON_TYPE_OBJ_ARGS_IS(o, array_trie)) {
        if (o->array_trie != NULL) { //!!!ARRAY_TRIE_NULL
            if (o->array_trie->sym == NULL &&
                o->attr.array_trie.open_node == NULL &&
                o->attr.array_trie.is_closed &&
                !JSON_TYPE_TRIE_NODE_HAS_NON_NULL_SIB(
                    o->array_trie))
                return JSON_TYPE_CHECK_ERROR(e, too_many_args);
        }
    }
    else
        UNEXPECT_VAR("%d", o->type);

    return JSON_TYPE_CALL_HANDLER(array_sep);
}

#define JSON_TYPE_NODE_AS_LIST_HAS_OPEN_ARRAY(p)   \
    ({                                             \
        const struct json_type_trie_t* __t;        \
        STATIC(JSON_TYPE_TYPEOF_IS_NODE_CONST(p)); \
        ASSERT(JSON_TYPE_NODE_IS_CONST(p, list));  \
        __t = (p)->attr.list != NULL               \
            ? (p)->attr.list->open_array           \
            : NULL;                                \
        if (__t != NULL)                           \
            ASSERT(__t->root != NULL);             \
        __t != NULL;                               \
    })

static bool json_type_array_end(struct json_type_t* type)
{
    struct json_type_check_error_info_t e;
    struct json_type_obj_args_t* o;

    JSON_TYPE_STACK_TOP_PRINT_DEBUG();

    JSON_TYPE_GET_CURRENT_TOK_POS(e);
    o = JSON_TYPE_STACK_TOP_REF_ENSURE_AS_ARGS(array);

    if (JSON_TYPE_OBJ_ARGS_IS(o, array)) {
        union json_type_array_node_pack_t p;

        if ((o->array == NULL) ||
            (p.open =
                JSON_TYPE_ARRAY_NODE_AS_IF_CONST(o->array, open)))
            ; // stev: nop
        else
        if ((p.closed =
                JSON_TYPE_ARRAY_NODE_AS_IF_CONST(o->array, closed))) {
            ASSERT(o->n_arg <= p.closed->size);
            if (o->n_arg < p.closed->size)
                return JSON_TYPE_CHECK_ERROR(e, too_few_args);
        }
        else
            UNEXPECT_VAR("%d", o->array->type);
    }
    else
    if (JSON_TYPE_OBJ_ARGS_IS(o, array_trie)) {
        if (o->array_trie != NULL) { //!!!ARRAY_TRIE_NULL
            if (o->array_trie->sym != NULL
                && o->attr.array_trie.open_node == NULL
                && o->attr.array_trie.is_closed
                && !JSON_TYPE_TRIE_NODE_HAS_NULL_SIB(
                        o->array_trie)
                && !(o->n_arg == 0
                    && JSON_TYPE_NODE_AS_LIST_HAS_OPEN_ARRAY(
                            o->attr.array_trie.base)))
                return JSON_TYPE_CHECK_ERROR(e, too_few_args);
        }
    }
    else
        UNEXPECT_VAR("%d", o->type);

    JSON_TYPE_STACK_POP_ARGS(array);

    return JSON_TYPE_CALL_HANDLER(array_end);
}

static bool json_type_value_sep(struct json_type_t* type)
{
    return JSON_TYPE_CALL_HANDLER(value_sep);
}

static struct json_handler_t json_type_handler = {
    .null_func         = (json_null_func_t)         json_type_null,
    .boolean_func      = (json_boolean_func_t)      json_type_boolean,
    .number_func       = (json_number_func_t)       json_type_number,
    .string_func       = (json_string_func_t)       json_type_string,
    .object_start_func = (json_object_start_func_t) json_type_object_start,
    .object_key_func   = (json_object_key_func_t)   json_type_object_key,
    .object_sep_func   = (json_object_sep_func_t)   json_type_object_sep,
    .object_end_func   = (json_object_end_func_t)   json_type_object_end,
    .array_start_func  = (json_array_start_func_t)  json_type_array_start,
    .array_sep_func    = (json_array_sep_func_t)    json_type_array_sep,
    .array_end_func    = (json_array_end_func_t)    json_type_array_end,
    .value_sep_func    = (json_value_sep_func_t)    json_type_value_sep,
};

#undef  JSON_STATUS_IS
#define JSON_STATUS_IS(s)           \
    (                               \
        type->current_stat ==       \
            json_parse_status_ ## s \
    )

#undef  JSON_CURRENT_STATUS
#define JSON_CURRENT_STATUS() (type->current_stat)

#define JSON_TYPE_ERROR(t, e)        \
    do {                             \
        type->error_info.type =      \
            json_type_error_ ## t;   \
        type->error_info.t = e;      \
        type->current_stat =         \
            json_parse_status_error; \
    } while (0)

static struct json_type_t* json_type_create(
    const struct json_type_lib_spec_t* spec,
    const struct json_handler_t* handler, void* handler_obj,
    const struct json_type_sizes_t* sizes)
{
    struct json_type_t* type;

    type = malloc(sizeof(struct json_type_t));
    ENSURE(type != NULL, "malloc failed");

    memset(type, 0, sizeof(struct json_type_t));

#ifdef JSON_DEBUG
    type->debug = SIZE_TRUNC_BITS(
        json_debug_get_level(json_debug_type_class),
        debug_bits);
#endif

    type->handler = handler;
    type->handler_obj = handler_obj;

    type->json_obj = json_obj_create(
        &json_type_handler, type, &sizes->obj);

    json_type_stack_init(
        &type->obj_stack,
        sizes->obj.stack_max,
        sizes->obj.stack_init);

    type->root_node = json_type_lib_init_from_spec(
        &type->type_lib, spec, &sizes->lib);

    if (type->root_node != NULL) {
        struct json_text_pos_t p = {1, 1};
        ASSERT(type->type_lib.error.type == json_type_lib_error_none);
        JSON_TYPE_STACK_PUSH_NODE(type->root_node, p);
    }
    else {
        ASSERT(type->type_lib.error.type != json_type_lib_error_none);
        JSON_TYPE_ERROR(type_lib, type->type_lib.error);
    }

    return type;
}

struct json_type_t* json_type_create_from_def(
    const uchar_t* type_def, const char* type_name,
    const struct json_handler_t* handler, void* handler_obj,
    const struct json_type_sizes_t* sizes)
{
    struct json_type_lib_spec_t s;

    JSON_TYPE_SIZES_VALIDATE(sizes);

    s.type = json_type_lib_text_spec_type;
    s.text.def = type_def;
    s.type_name = type_name;

    return json_type_create(
        &s, handler, handler_obj, sizes);
}

struct json_type_t* json_type_create_from_lib(
    const char* type_lib, const char* type_name,
    const struct json_handler_t* handler, void* handler_obj,
    const struct json_type_sizes_t* sizes)
{
    struct json_type_lib_spec_t s;

    JSON_TYPE_SIZES_VALIDATE(sizes);

    s.type = json_type_lib_file_spec_type;
    s.file.name = type_lib;
    s.type_name = type_name;

    return json_type_create(
        &s, handler, handler_obj, sizes);
}

void json_type_destroy(struct json_type_t* type)
{
    json_type_lib_done(&type->type_lib);
    json_type_stack_done(&type->obj_stack);
    json_obj_destroy(type->json_obj);
    free(type);
}

bool json_type_config_get_param(
    struct json_type_t* type, enum json_config_param_t param)
{
    return json_config_get_param(type->json_obj, param);
}

void json_type_config_set_param(
    struct json_type_t* type, enum json_config_param_t param,
    bool val)
{
    json_config_set_param(type->json_obj, param, val);
}

#define JSON_TYPE_RETURN(e)                    \
    ({                                         \
        enum json_parse_status_t __r = (e);    \
        PRINT_DEBUG("parse=%s canceled=%d",    \
            json_parse_status_get_name(__r),   \
            type->canceled);                   \
        if (__r == json_parse_status_error)    \
            JSON_TYPE_ERROR(parse,             \
                *json_get_error_info(          \
                    type->json_obj));          \
        if (!type->canceled &&                 \
            __r == json_parse_status_canceled) \
            __r =  json_parse_status_error;    \
        type->current_stat = __r;              \
        __r;                                   \
    })

enum json_parse_status_t json_type_parse(
    struct json_type_t* type, const uchar_t* buf, size_t len)
{
    if (JSON_STATUS_IS(ok))
        return JSON_TYPE_RETURN(json_parse(type->json_obj, buf, len));
    else
        return JSON_TYPE_RETURN(JSON_CURRENT_STATUS());
}

enum json_parse_status_t json_type_parse_done(
    struct json_type_t* type)
{
    enum json_parse_status_t r;

    if (!JSON_STATUS_IS(ok) || type->done)
        return JSON_CURRENT_STATUS();

    r = JSON_TYPE_RETURN(json_parse_done(type->json_obj));

    if (r == json_parse_status_ok) {
        const struct json_type_node_t* n;
        struct json_type_obj_t o;
        size_t s;

        s = JSON_TYPE_STACK_SIZE();
        ENSURE(s == 1, "invalid type stack: size is %zu", s);

        o = JSON_TYPE_STACK_POP();
        ENSURE(JSON_TYPE_OBJ_IS(&o, node),
            "invalid type stack: top element is not 'node'");

        n = JSON_TYPE_OBJ_AS_NODE(&o);
        ENSURE(n != NULL && type->root_node == n,
            "invalid type stack: unexpected top element");
    }

    type->done = true;

    return r;
}

static const char* json_type_error_type_get_desc(
    enum json_type_error_type_t type)
{
    switch (type) {
    case json_type_error_none:
        return "-";
    case json_type_error_parse:
        return "parse";
    case json_type_error_type_lib:
        return "type lib";
    case json_type_error_type_check:
        return "type check";
    default:
        UNEXPECT_VAR("%d", type);
    }
}

static struct json_error_pos_t json_type_error_info_get_pos(
    const struct json_type_error_info_t* info)
{
    switch (info->type) {
    case json_type_error_none: {
        struct json_error_pos_t p = {
            .col = 0, .line = 0
        };
        return p;
    }
    case json_type_error_parse:
        return info->parse.pos;
    case json_type_error_type_lib:
        return json_type_lib_error_info_get_pos(
            &info->type_lib);
    case json_type_error_type_check:
        return info->type_check.pos;
    default:
        UNEXPECT_VAR("%d", info->type);
    }
}

static const struct json_file_info_t* json_type_error_info_get_file(
    const struct json_type_error_info_t* info)
{
    switch (info->type) {
    case json_type_error_none:
    case json_type_error_parse:
    case json_type_error_type_check:
        return NULL;
    case json_type_error_type_lib:
        return json_type_lib_error_info_get_file(
            &info->type_lib);
    default:
        UNEXPECT_VAR("%d", info->type);
    }
}

struct json_error_pos_t json_type_get_error_pos(struct json_type_t* type)
{
    return json_type_error_info_get_pos(&type->error_info);
}

const struct json_file_info_t* json_type_get_error_file(
    struct json_type_t* type)
{
    return json_type_error_info_get_file(&type->error_info);
}

#ifdef JSON_DEBUG

#undef  CASE
#define CASE(E) case json_type_error_ ## E: return #E

static const char* json_type_error_type_get_name(
    enum json_type_error_type_t type)
{
    switch (type) {
    CASE(none);
    CASE(parse);
    CASE(type_lib);
    CASE(type_check);
    default:
        UNEXPECT_VAR("%d", type);
    }
}

#endif

static void json_type_error_info_print_error_desc(
    const struct json_type_error_info_t* info, FILE* file)
{
    switch (info->type) {
    case json_type_error_none:
        fputc('-', file);
        break;
    case json_type_error_parse:
        fputs(json_error_info_get_desc(
            &info->parse), file);
        break;
    case json_type_error_type_lib:
        json_type_lib_error_info_print_error_desc(
            &info->type_lib, file);
        break;
    case json_type_error_type_check:
        json_type_check_error_info_print_error_desc(
            &info->type_check, file);
        break;
    default:
        UNEXPECT_VAR("%d", info->type);
    }
}

bool json_type_get_is_error(struct json_type_t* type)
{
    return type->error_info.type != json_type_error_none;
}

void json_type_print_error_desc(struct json_type_t* type, FILE* file)
{
    fprintf(file, "%s error: ",
        json_type_error_type_get_desc(type->error_info.type));
    json_type_error_info_print_error_desc(&type->error_info, file);
}

#ifdef JSON_DEBUG
void json_type_error_info_print_error_debug(
    const struct json_type_error_info_t* info, FILE* file)
{
    struct json_error_pos_t pos =
        json_type_error_info_get_pos(info);

    fprintf(file, "{.type=%s ",
        json_type_error_type_get_name(info->type));

    switch (info->type) {
    case json_type_error_none:
        fputs("{}", file);
        break;
    case json_type_error_parse:
        json_error_info_print_error_debug(
            &info->parse, file);
        break;
    case json_type_error_type_lib:
        json_type_lib_error_info_print_error_debug(
            &info->type_lib, file);
        break;
    case json_type_error_type_check:
        json_type_check_error_info_print_error_debug(
            &info->type_check, file);
        break;
    default:
        UNEXPECT_VAR("%d", info->type);
    }

    fprintf(file, " .pos={.line=%zu .col=%zu}}",
        pos.line, pos.col);
}

void json_type_print_error_debug(struct json_type_t* type, FILE* file)
{
    json_type_error_info_print_error_debug(&type->error_info, file);
}
#endif

#define JSON_FILE_ERROR(e)                       \
    do {                                         \
        error->type = json_file_ ## e ## _error; \
        error->sys = errno;                      \
    } while (0)

bool json_file_is_shared_obj(
    const char* file_name,
    struct json_file_error_t* error)
{
    static const char ELF[] = "\x7f" "ELF";

    struct stat st;
    bool r = false;
    const size_t N = 4;
    uchar_t b[N];
    ssize_t n;
    int d;

    memset(error, 0, sizeof(*error));

    if ((d = open(file_name, O_RDONLY)) < 0) {
        JSON_FILE_ERROR(open);
        return r;
    }
    if (fstat(d, &st)) {
        JSON_FILE_ERROR(stat);
        goto close;
    }
    if (!S_ISREG(st.st_mode))
        goto close;

    n = read(d, b, N);
    if (n < 0 || INT_AS_SIZE(n) != N) {
        JSON_FILE_ERROR(read);
        goto close;
    }
    r = !strnucmp(b, ELF, N);

close:
    if (close(d)) {
        JSON_FILE_ERROR(close);
        r = false;
    }

    return r;
}

const char* json_file_error_type_get_desc(
    enum json_file_error_type_t type)
{
    switch (type) {
    case json_file_none_error:
        return "-";
    case json_file_open_error:
        return "file open error";
    case json_file_stat_error:
        return "file stat error";
    case json_file_read_error:
        return "file read error";
    case json_file_close_error:
        return "file close error";
    default:
        INVALID_ARG("%d", type);
    }
}


