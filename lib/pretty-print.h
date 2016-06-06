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

#ifndef __PRETTY_PRINT_H
#define __PRETTY_PRINT_H

#include <stdio.h>

#include "json-defs.h"

enum {
    pretty_print_char_quotes   = 1U << 0,
    pretty_print_string_quotes = 1U << 1,
};

JSON_API size_t pretty_print_len(char ch, size_t flags);
JSON_API const char* pretty_print_fmt(char ch, size_t flags);

JSON_API size_t pretty_print_char(
    FILE* file, char ch, size_t flags);
JSON_API size_t pretty_print_string(
    FILE* file, const uchar_t* str, size_t len, size_t flags);

enum pretty_print_repr_flags_t {
    pretty_print_repr_quote_non_print     = 1U << 0,
    pretty_print_repr_quote_non_ascii     = 1U << 1,
    pretty_print_repr_unicode_quote_cntrl = 1U << 2,
    pretty_print_repr_print_quotes        = 1U << 3,
};

JSON_API void pretty_print_repr(
    FILE* file, const uchar_t* str, size_t len,
    enum pretty_print_repr_flags_t flags);

#endif/*__PRETTY_PRINT_H*/


