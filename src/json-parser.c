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

#include "obj.h"
#include "common.h"
#include "json-parser.h"

static void obj_json_parser_init(
    struct obj_json_parser_t* this, const struct obj_type_t* type,
    const struct options_t* opts)
{
    obj_init(OBJ_SUPER(this), type);
    OBJ_INIT(this, type);

    (opts->type_check != options_type_check_none_type
        ? obj_json_base_init_type
        : obj_json_base_init_obj)(
            JSON_BASE(this), opts, NULL, NULL);
}

static void obj_json_parser_done(struct obj_json_parser_t* this)
{
    obj_json_base_done(JSON_BASE(this));

    obj_done(OBJ_SUPER(this));
}

static int obj_json_parser_run(struct obj_json_parser_t* this)
{
    return obj_json_base_run(JSON_BASE(this));
}

OBJ_IMPL(
    obj_json_parser,
    obj_json_parser_t,
    obj_json_parser_init,
    obj_json_parser_done,
    obj_json_parser_run
);

