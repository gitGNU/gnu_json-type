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

#ifndef __JSON_TYPELIB_H
#define __JSON_TYPELIB_H

#include "obj.h"
#include "json-base.h"

typedef bool (*obj_json_typelib_func_t)(struct json_type_lib_t*);

struct obj_json_typelib_t
{
    OBJ_BASE(obj_t);

    struct obj_json_base_t json_base;

    obj_json_typelib_func_t typelib_func;

    bits_t is_shared_obj: 1;
};

#endif/*__JSON_TYPELIB_H */


