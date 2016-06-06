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

#ifndef __JSON_PRINTER_H
#define __JSON_PRINTER_H

#include "obj.h"
#include "json-base.h"
#include "common.h"

#include "int-traits.h"

struct obj_json_printer_t
{
    OBJ_BASE(obj_t);

    struct obj_json_base_t json_base;
    const struct json_ast_visitor_t* visitor;

    bits_t first_nl: 1;
    bits_t printed_out: 1;
    bits_t skip_indent: 1;
    bits_t quote_cntrl: 1;
    bits_t print_dots: 1;
    bits_t newline_sep: 1;
#ifdef DEBUG
    bits_t debug: debug_bits;
#endif

    size_t indent_level;
};

#endif/*__JSON_PRINTER_H*/

