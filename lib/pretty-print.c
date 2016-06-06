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

#include <stdio.h>
#include <ctype.h>

#include "common.h"
#include "pretty-print.h"
#include "int-traits.h"

size_t pretty_print_len(char ch, size_t flags)
{
    ASSERT(
        !(flags & pretty_print_char_quotes) ||
        !(flags & pretty_print_string_quotes));

    switch (ch) {
    case '\0':
    case '\a':
    case '\b':
    case '\t':
    case '\n':
    case '\v':
    case '\f':
    case '\r':
    case '\\':
        return 2;
    case '\'':
    case '"':
        return flags
            & (pretty_print_char_quotes|pretty_print_string_quotes)
            ? 2
            : 1;
    default:
        return !isprint(ch)
            ? 4
            : 1;
    }
}

const char* pretty_print_fmt(char ch, size_t flags)
{
    static const char* plain_char = "%c";
    static const char* quoted_char = "\\%c";
    static const char* quoted_hex = "\\x%02x";

    ASSERT(
        !(flags & pretty_print_char_quotes) ||
        !(flags & pretty_print_string_quotes));

    switch (ch) {
    case '\0':
        return "\\0";
    case '\a':
        return "\\a";
    case '\b':
        return "\\b";
    case '\t':
        return "\\t";
    case '\n':
        return "\\n";
    case '\v':
        return "\\v";
    case '\f':
        return "\\f";
    case '\r':
        return "\\r";
    case '\\':
        return quoted_char;
    case '\'':
        return flags & pretty_print_char_quotes
            ? quoted_char
            : plain_char;
    case '"':
        return flags & pretty_print_string_quotes
            ? quoted_char
            : plain_char;
    default:
        return !isprint(ch)
            ? quoted_hex
            : plain_char;
    }
}

size_t pretty_print_char(FILE* file, char ch, size_t flags)
{
    size_t r;
    const bool q = flags & pretty_print_char_quotes;
    int n;

    if (q)
        fputc('\'', file);

    n = fprintf(file, pretty_print_fmt(ch, flags), (uchar_t) ch);
    r = INT_AS_SIZE(n);

    if (q) {
        fputc('\'', file);
        SIZE_ADD_EQ(r, SZ(2));
    }

    return r;
}

size_t pretty_print_string(
    FILE* file, const uchar_t* str, size_t len, size_t flags)
{
    size_t r = 0;
    const bool q = flags & pretty_print_string_quotes;

    if (q)
        fputc('"', file);

    while (len --) {
        uchar_t c = *str ++;
        size_t s;
        int n;

        n = fprintf(file, pretty_print_fmt(c, flags), c);
        s = INT_AS_SIZE(n);

        SIZE_ADD_EQ(r, s);
    }

    if (q) {
        fputc('"', file);
        SIZE_ADD_EQ(r, SZ(2));
    }

    return r;
}

void pretty_print_repr(
    FILE* file, const uchar_t* str, size_t len,
    enum pretty_print_repr_flags_t flags)
{
    if (flags & pretty_print_repr_print_quotes)
        fputc('"', file);

    while (len --) {
        uchar_t c = *str ++;
        size_t n;

        switch (c) {
        case '"':
            fputs("\\\"", file);
            break;
        case '\\':
            fputs("\\\\", file);
            break;
        case '\b':
            fputs("\\b", file);
            break;
        case '\f':
            fputs("\\f", file);
            break;
        case '\n':
            fputs("\\n", file);
            break;
        case '\r':
            fputs("\\r", file);
            break;
        case '\t':
            fputs("\\t", file);
            break;
        default:
            if (c < 0x20u)
                n = 4;
            else
            if (!isascii(c)
                && (flags & pretty_print_repr_quote_non_ascii))
                n = 2;
            else
            if (iscntrl(c)
                && (flags & pretty_print_repr_unicode_quote_cntrl))
                n = 4;
            else
            if (!isprint(c)
                && (flags & pretty_print_repr_quote_non_print))
                n = 2;
            else
                n = 0;

            if (n)
                fprintf(file, "\\%c%0*x", n == 4 ? 'u' : 'x',
                    SIZE_AS_INT(n), c);
            else
                fputc(c, file);
        }
    }

    if (flags & pretty_print_repr_print_quotes)
        fputc('"', file);
}


