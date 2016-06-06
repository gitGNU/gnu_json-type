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

#ifndef __JSON2_H
#define __JSON2_H

#include "obj.h"
#include "json-base.h"

struct obj_json2_node_t;
struct obj_json2_node_pool_t;
struct obj_json2_string_stack_t;

struct obj_json2_t
{
    OBJ_BASE(obj_t);

    struct obj_json_base_t json_base;

    struct obj_json2_node_pool_t* pool;
    struct obj_json2_string_stack_t* stack;

    struct obj_json2_node_t* path;
    struct obj_json2_node_t* last;
};

#endif/*__JSON2_H */


