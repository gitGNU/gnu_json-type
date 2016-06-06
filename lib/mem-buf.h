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

#ifndef __MEM_BUF_H
#define __MEM_BUF_H

#include <stdint.h>

#include "json-defs.h"

struct mem_buf_t
{
    size_t max_size;
    size_t init_size;
    uchar_t* ptr;
    size_t size;
    size_t len;
};

JSON_API struct mem_buf_t* mem_buf_create(
    size_t max_size, size_t init_size);
JSON_API void mem_buf_destroy(struct mem_buf_t* buf);

JSON_API void mem_buf_init(struct mem_buf_t* buf,
    size_t max_size, size_t init_size);
JSON_API void mem_buf_done(struct mem_buf_t* buf);

JSON_API void mem_buf_enlarge(struct mem_buf_t* buf, size_t len);

enum mem_buf_op_t {
    mem_buf_append = 1,
    mem_buf_shiftin,
    mem_buf_reset,
};

JSON_API void mem_buf_update(
    struct mem_buf_t* buf, const uchar_t* ptr, size_t len,
    enum mem_buf_op_t op);

JSON_API void mem_buf_shift_fit(
    struct mem_buf_t* buf, size_t len);

#define MEM_BUF_TYPEOF_IS_BUF(x) \
    TYPEOF_IS(x, struct mem_buf_t*)

#define MEM_BUF_APPEND(buf, ch)             \
    do {                                    \
        STATIC(MEM_BUF_TYPEOF_IS_BUF(buf)); \
        STATIC(TYPEOF_IS_CHAR_UCHAR(ch));   \
        ASSERT(buf->len <= buf->size);      \
        if (buf->len == buf->size) {        \
            mem_buf_enlarge(buf, 1);        \
            ASSERT(buf->len < buf->size);   \
        }                                   \
        *(buf->ptr + buf->len ++) = ch;     \
    } while (0)

#endif/*__MEM_BUF_H*/

