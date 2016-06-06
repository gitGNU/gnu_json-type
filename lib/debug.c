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
#include "common.h"
#include "ptr-traits.h"

#define PRINT_DEBUG_COND
#include "debug.h"

#include "debug-impl.c"

#ifdef JSON_DEBUG

#undef  CASE
#define CASE(n) \
    [json_debug_ ## n ## _class] = 0

static size_t json_debug_levels[] = {
    CASE(obj),
    CASE(ast),
    CASE(type),
    CASE(type_lib),
    CASE(type_ruler),
};

size_t json_debug_get_level(
    enum json_debug_class_t class)
{
    size_t* p;

    if ((p = ARRAY_NULL_ELEM_REF(
            json_debug_levels, class)) == NULL)
        INVALID_ARG("%d", class);

    return *p;
}

void json_debug_set_level(
    enum json_debug_class_t class,
    size_t level)
{
    size_t* p;

    if ((p = ARRAY_NULL_ELEM_REF(
            json_debug_levels, class)) == NULL)
        INVALID_ARG("%d", class);

    *p = level;
}

#endif


