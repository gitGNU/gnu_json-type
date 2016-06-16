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

#include "common.h"
#include "json-intf.h"

enum json_intf_type_t
{
    json_obj_intf_type,
    json_ast_intf_type,
    json_type_intf_type,
    json_type_lib_intf_type
};

struct json_intf_t
{
    enum json_intf_type_t type;

    union {
        struct json_obj_t*      obj;
        struct json_ast_t*      ast;
        struct json_type_t*     type;
        struct json_type_lib_t* type_lib;
    } ptr;

    void (*config_set_param)(struct json_intf_t*,
        enum json_config_param_t, bool);
    enum json_parse_status_t (*parse)(struct json_intf_t*,
        const uchar_t*, size_t);
    enum json_parse_status_t (*parse_done)(struct json_intf_t*);
    bool (*get_is_error)(struct json_intf_t*);
    struct json_error_pos_t (*get_error_pos)(struct json_intf_t*);
    const struct json_file_info_t* (*get_error_file)(
        struct json_intf_t*);
    void (*print_error_desc)(struct json_intf_t*, FILE*);
#ifdef JSON_DEBUG
    void (*print_error_debug)(struct json_intf_t*, FILE*);
#endif
    void (*destroy)(struct json_intf_t*);
};

#define JSON_INTF_OBJ_IS(n)          \
    (                                \
        intf->type ==                \
            json_ ## n ## _intf_type \
    )

#define JSON_INTF_OBJ(n)             \
    ({                               \
        ASSERT(JSON_INTF_OBJ_IS(n)); \
        intf->ptr.n;                 \
    })

static void json_intf_config_set_param_obj(
    struct json_intf_t* intf, enum json_config_param_t param,
    bool val)
{
    json_config_set_param(JSON_INTF_OBJ(obj), param, val);
}

static void json_intf_config_set_param_ast(
    struct json_intf_t* intf, enum json_config_param_t param,
    bool val)
{
    json_ast_config_set_param(JSON_INTF_OBJ(ast), param, val);
}

static void json_intf_config_set_param_type(
    struct json_intf_t* intf, enum json_config_param_t param,
    bool val)
{
    json_type_config_set_param(JSON_INTF_OBJ(type), param, val);
}

static void json_intf_config_set_param_type_lib(
    struct json_intf_t* intf UNUSED, enum json_config_param_t param UNUSED,
    bool val UNUSED)
{
    // stev: nop!
}

static enum json_parse_status_t json_intf_parse_obj(
    struct json_intf_t* intf, const uchar_t* buf, size_t len)
{
    return json_parse(JSON_INTF_OBJ(obj), buf, len);
}

static enum json_parse_status_t json_intf_parse_ast(
    struct json_intf_t* intf, const uchar_t* buf, size_t len)
{
    return json_ast_parse(JSON_INTF_OBJ(ast), buf, len);
}

static enum json_parse_status_t json_intf_parse_type(
    struct json_intf_t* intf, const uchar_t* buf, size_t len)
{
    return json_type_parse(JSON_INTF_OBJ(type), buf, len);
}

static enum json_parse_status_t json_intf_parse_type_lib(
    struct json_intf_t* intf, const uchar_t* buf, size_t len)
{
    return json_type_lib_parse(JSON_INTF_OBJ(type_lib), buf, len);
}

static enum json_parse_status_t json_intf_parse_done_obj(
    struct json_intf_t* intf)
{
    return json_parse_done(JSON_INTF_OBJ(obj));
}

static enum json_parse_status_t json_intf_parse_done_ast(
    struct json_intf_t* intf)
{
    return json_ast_parse_done(JSON_INTF_OBJ(ast));
}

static enum json_parse_status_t json_intf_parse_done_type(
    struct json_intf_t* intf)
{
    return json_type_parse_done(JSON_INTF_OBJ(type));
}

static enum json_parse_status_t json_intf_parse_done_type_lib(
    struct json_intf_t* intf)
{
    return json_type_lib_parse_done(JSON_INTF_OBJ(type_lib));
}

static bool json_intf_get_is_error_obj(
    struct json_intf_t* intf)
{
    return json_get_is_error(JSON_INTF_OBJ(obj));
}

static bool json_intf_get_is_error_ast(
    struct json_intf_t* intf)
{
    return json_ast_get_is_error(JSON_INTF_OBJ(ast));
}

static bool json_intf_get_is_error_type(
    struct json_intf_t* intf)
{
    return json_type_get_is_error(JSON_INTF_OBJ(type));
}

static bool json_intf_get_is_error_type_lib(
    struct json_intf_t* intf)
{
    return json_type_lib_get_is_error(JSON_INTF_OBJ(type_lib));
}

static struct json_error_pos_t json_intf_get_error_pos_obj(
    struct json_intf_t* intf)
{
    return json_get_error_pos(JSON_INTF_OBJ(obj));
}

static struct json_error_pos_t json_intf_get_error_pos_ast(
    struct json_intf_t* intf)
{
    return json_ast_get_error_pos(JSON_INTF_OBJ(ast));
}

static struct json_error_pos_t json_intf_get_error_pos_type(
    struct json_intf_t* intf)
{
    return json_type_get_error_pos(JSON_INTF_OBJ(type));
}

static struct json_error_pos_t json_intf_get_error_pos_type_lib(
    struct json_intf_t* intf)
{
    return json_type_lib_get_error_pos(JSON_INTF_OBJ(type_lib));
}

static const struct json_file_info_t* json_intf_get_error_file_obj(
    struct json_intf_t* intf)
{
    return json_get_error_file(JSON_INTF_OBJ(obj));
}

static const struct json_file_info_t* json_intf_get_error_file_ast(
    struct json_intf_t* intf)
{
    return json_ast_get_error_file(JSON_INTF_OBJ(ast));
}

static const struct json_file_info_t* json_intf_get_error_file_type(
    struct json_intf_t* intf)
{
    return json_type_get_error_file(JSON_INTF_OBJ(type));
}

static const struct json_file_info_t* json_intf_get_error_file_type_lib(
    struct json_intf_t* intf)
{
    return json_type_lib_get_error_file(JSON_INTF_OBJ(type_lib));
}

static void json_intf_print_error_desc_obj(
    struct json_intf_t* intf, FILE* file)
{
    json_print_error_desc(JSON_INTF_OBJ(obj), file);
}

static void json_intf_print_error_desc_ast(
    struct json_intf_t* intf, FILE* file)
{
    json_ast_print_error_desc(JSON_INTF_OBJ(ast), file);
}

static void json_intf_print_error_desc_type(
    struct json_intf_t* intf, FILE* file)
{
    json_type_print_error_desc(JSON_INTF_OBJ(type), file);
}

static void json_intf_print_error_desc_type_lib(
    struct json_intf_t* intf, FILE* file)
{
    json_type_lib_print_error_desc(JSON_INTF_OBJ(type_lib), file);
}

#ifdef JSON_DEBUG

static void json_intf_print_error_debug_obj(
    struct json_intf_t* intf, FILE* file)
{
    json_print_error_debug(JSON_INTF_OBJ(obj), file);
}

static void json_intf_print_error_debug_ast(
    struct json_intf_t* intf, FILE* file)
{
    json_ast_print_error_debug(JSON_INTF_OBJ(ast), file);
}

static void json_intf_print_error_debug_type(
    struct json_intf_t* intf, FILE* file)
{
    json_type_print_error_debug(JSON_INTF_OBJ(type), file);
}

static void json_intf_print_error_debug_type_lib(
    struct json_intf_t* intf, FILE* file)
{
    json_type_lib_print_error_debug(JSON_INTF_OBJ(type_lib), file);
}

#endif

static void json_intf_destroy_obj(struct json_intf_t* intf)
{
    json_obj_destroy(JSON_INTF_OBJ(obj));
}

static void json_intf_destroy_ast(struct json_intf_t* intf)
{
    json_ast_destroy(JSON_INTF_OBJ(ast));
}

static void json_intf_destroy_type(struct json_intf_t* intf)
{
    json_type_destroy(JSON_INTF_OBJ(type));
}

static void json_intf_destroy_type_lib(struct json_intf_t* intf)
{
    json_type_lib_destroy(JSON_INTF_OBJ(type_lib));
}

#ifdef JSON_DEBUG
#define JSON_INTF_INIT_OBJ_DEBUG(n) \
    intf->print_error_debug = json_intf_print_error_debug_ ## n;
#else
#define JSON_INTF_INIT_OBJ_DEBUG(n)
#endif

#define JSON_INTF_INIT_OBJ_(n, c, ...)                             \
    do {                                                           \
        intf->type = json_ ## n ## _intf_type;                     \
        intf->ptr.n = json_ ## n ## _create ## c(__VA_ARGS__);     \
        intf->config_set_param = json_intf_config_set_param_ ## n; \
        intf->parse = json_intf_parse_ ## n;                       \
        intf->parse_done = json_intf_parse_done_ ## n;             \
        intf->get_is_error = json_intf_get_is_error_ ## n;         \
        intf->get_error_pos = json_intf_get_error_pos_ ## n;       \
        intf->get_error_file = json_intf_get_error_file_ ## n;     \
        intf->print_error_desc = json_intf_print_error_desc_ ## n; \
        intf->destroy = json_intf_destroy_ ## n;                   \
        JSON_INTF_INIT_OBJ_DEBUG(n)                                \
    } while (0)

#define JSON_INTF_INIT_OBJ(n, ...) \
    JSON_INTF_INIT_OBJ_(n, , ## __VA_ARGS__)
#define JSON_INTF_INIT_OBJ2(n, c, ...) \
    JSON_INTF_INIT_OBJ_(n, _ ## c, ## __VA_ARGS__)

static struct json_intf_t* json_intf_create()
{
    struct json_intf_t* intf;

    intf = malloc(sizeof(struct json_intf_t));
    if (intf == NULL)
        OOM_ERROR("malloc failed creating json_intf_t");

    memset(intf, 0, sizeof(struct json_intf_t));

    return intf;
}

struct json_intf_t* json_intf_create_obj(
    const struct json_handler_t* handler, void* obj,
    const struct json_obj_sizes_t* sizes)
{
    struct json_intf_t* intf;

    intf = json_intf_create();
    JSON_INTF_INIT_OBJ(obj, handler, obj, sizes);

    return intf;
}

struct json_intf_t* json_intf_create_ast(
    const struct json_ast_sizes_t* sizes)
{
    struct json_intf_t* intf;

    intf = json_intf_create();
    JSON_INTF_INIT_OBJ(ast, sizes);

    return intf;
}

struct json_intf_t* json_intf_create_type_from_def(
    const uchar_t* type_def, const char* type_name,
    const struct json_handler_t* handler, void* obj,
    const struct json_type_sizes_t* sizes)
{
    struct json_intf_t* intf;

    intf = json_intf_create();
    JSON_INTF_INIT_OBJ2(
        type, from_def, type_def, type_name,
        handler, obj, sizes);

    return intf;
}

struct json_intf_t* json_intf_create_type_from_lib(
    const char* type_lib, const char* type_name,
    const struct json_handler_t* handler, void* obj,
    const struct json_type_sizes_t* sizes)
{
    struct json_intf_t* intf;

    intf = json_intf_create();
    JSON_INTF_INIT_OBJ2(
        type, from_lib, type_lib, type_name,
        handler, obj, sizes);

    return intf;
}

struct json_intf_t* json_intf_create_text_type_lib(
    const struct json_type_lib_sizes_t* sizes)
{
    struct json_intf_t* intf;

    intf = json_intf_create();
    JSON_INTF_INIT_OBJ2(type_lib, from_source_text, sizes);

    return intf;
}

struct json_intf_t* json_intf_create_sobj_type_lib(
    const char* lib_name, const struct json_type_lib_sizes_t* sizes)
{
    struct json_intf_t* intf;

    intf = json_intf_create();
    JSON_INTF_INIT_OBJ2(type_lib, from_shared_obj, lib_name, sizes);

    return intf;
}

void json_intf_destroy(struct json_intf_t* intf)
{
    intf->destroy(intf);
    free(intf);
}

void json_intf_config_set_param(struct json_intf_t* intf,
    enum json_config_param_t param, bool val)
{ intf->config_set_param(intf, param, val); }

enum json_parse_status_t json_intf_parse(struct json_intf_t* intf,
    const uchar_t* buf, size_t len)
{ return intf->parse(intf, buf, len); }

enum json_parse_status_t json_intf_parse_done(struct json_intf_t* intf)
{ return intf->parse_done(intf); }

bool json_intf_get_is_error(struct json_intf_t* intf)
{ return intf->get_is_error(intf); }

struct json_error_pos_t json_intf_get_error_pos(struct json_intf_t* intf)
{ return intf->get_error_pos(intf); }

const struct json_file_info_t* json_intf_get_error_file(
    struct json_intf_t* intf)
{ return intf->get_error_file(intf); }

void json_intf_print_error_desc(struct json_intf_t* intf, FILE* file)
{ intf->print_error_desc(intf, file); }

#ifdef JSON_DEBUG
void json_intf_print_error_debug(struct json_intf_t* intf, FILE* file)
{ intf->print_error_debug(intf, file); }
#endif

#define JSON_INTF_OBJ_AS(n)                                           \
    ({                                                                \
        if (!JSON_INTF_OBJ_IS(n))                                     \
            UNEXPECT_ERR("object interface is not of type '%s'", #n); \
        intf->ptr.n;                                                  \
    })

struct json_obj_t* json_intf_get_as_obj(struct json_intf_t* intf)
{
    return JSON_INTF_OBJ_AS(obj);
}

struct json_ast_t* json_intf_get_as_ast(struct json_intf_t* intf)
{
    return JSON_INTF_OBJ_AS(ast);
}

struct json_type_t* json_intf_get_as_type(struct json_intf_t* intf)
{
    return JSON_INTF_OBJ_AS(type);
}

struct json_type_lib_t* json_intf_get_as_type_lib(struct json_intf_t* intf)
{
    return JSON_INTF_OBJ_AS(type_lib);
}

