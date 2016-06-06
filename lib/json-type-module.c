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

#include "json.h"
#include "json-type.h"

#define MODULE_MAKE_NAME_(n, s) n ## _ ## s
#define MODULE_MAKE_NAME(n, s)  MODULE_MAKE_NAME_(n, s)

#define MODULE_TYPE_DEF        MODULE_MAKE_NAME(MODULE, type_def)
#define MODULE_GET_TYPE_DEF    MODULE_MAKE_NAME(MODULE, get_type_def)
#define MODULE_GET_LIB_VERSION MODULE_MAKE_NAME(MODULE, get_lib_version)

#include MODULE_DEF

JSON_API size_t MODULE_GET_LIB_VERSION(void)
{
    return JSON_VERSION;
}

JSON_API const struct json_type_def_t* MODULE_GET_TYPE_DEF(void)
{
    return &MODULE_TYPE_DEF;
}


