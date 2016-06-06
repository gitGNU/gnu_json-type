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
#include "json-typelib.h"

static bool json_type_lib_print_check_attr(struct json_type_lib_t* lib)
{
    return json_type_lib_print_attr(lib) && json_type_lib_check_attr(lib);
}

static const obj_json_typelib_func_t type_lib_funcs[] = {
    [options_validate_typelib_func]    = json_type_lib_validate,
    [options_print_typelib_func]       = json_type_lib_print,
    [options_attr_typelib_func]        = json_type_lib_print_attr,
    [options_check_typelib_func]       = json_type_lib_check_attr,
    [options_print_check_typelib_func] = json_type_lib_print_check_attr,
    [options_def_typelib_func]         = json_type_lib_gen_def
};

static void obj_json_typelib_init(
    struct obj_json_typelib_t* this, const struct obj_type_t* type,
    const struct options_t* opts)
{
    obj_init(OBJ_SUPER(this), type);
    OBJ_INIT(this, type);

    this->typelib_func = ARRAY_NULL_ELEM(
        type_lib_funcs, opts->typelib_func);
    if (this->typelib_func == NULL)
        UNEXPECT_ERR(
            "invalid typelib-func: %d",
            opts->typelib_func);

    if (opts->input != NULL) {
        struct json_file_error_t e;

        this->is_shared_obj = json_file_is_shared_obj(
            opts->input, &e);
        if (e.type != json_file_none_error)
            error("%s: %s: %s",
                opts->input,
                json_file_error_type_get_desc(e.type),
                strerror(e.sys));
    }
    else
        this->is_shared_obj = false;

    (this->is_shared_obj
        ? obj_json_base_init_sobj_type_lib
        : obj_json_base_init_text_type_lib)(
        JSON_BASE(this), opts);

    //!!! handle errors of typelib
    //!!! ...
}

static void obj_json_typelib_done(struct obj_json_typelib_t* this)
{
    obj_json_base_done(JSON_BASE(this));

    obj_done(OBJ_SUPER(this));
}

static int obj_json_typelib_run(struct obj_json_typelib_t* this)
{
    if (!this->is_shared_obj &&
        obj_json_base_run(JSON_BASE(this)))
        return 1;

    if (this->typelib_func(
            obj_json_base_get_type_lib(JSON_BASE(this))))
        return 0;

    return obj_json_base_check_error(JSON_BASE(this));
}

OBJ_IMPL(
    obj_json_typelib,
    obj_json_typelib_t,
    obj_json_typelib_init,
    obj_json_typelib_done,
    obj_json_typelib_run
);


