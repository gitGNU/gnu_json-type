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

extern const struct obj_type_t obj_json_parser;
extern const struct obj_type_t obj_json_printer;
extern const struct obj_type_t obj_json_typelib;
extern const struct obj_type_t obj_json2;
//!!!2JSON extern const struct obj_type_t obj_2json;

const struct obj_type_t* get_obj_type(enum obj_name_t name)
{
    if (name == OBJ_JSON_PARSER)
        return &obj_json_parser;
    if (name == OBJ_JSON_PRINTER)
        return &obj_json_printer;
    if (name == OBJ_JSON_TYPELIB)
        return &obj_json_typelib;
    if (name == OBJ_JSON2)
        return &obj_json2;
//!!!2JSON	if (name == OBJ_2JSON)
//!!!2JSON		return &obj_2json;
    else
        UNEXPECT_ERR(
            "unknown obj name: %d",
            name);
}

void obj_init(struct obj_t* this, const struct obj_type_t* type)
{
    OBJ_INIT(this, type);

    this->output = stdout;
}

void obj_done(struct obj_t* this UNUSED)
{
    // stev: nop
}

int obj_run(struct obj_t* this UNUSED)
{
    return 0;
}


