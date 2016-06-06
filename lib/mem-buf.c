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

#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "mem-buf.h"
#include "int-traits.h"

struct mem_buf_t* mem_buf_create(size_t max_size, size_t init_size)
{
    struct mem_buf_t* buf;

    buf = malloc(sizeof(struct mem_buf_t));
    ENSURE(buf != NULL, "malloc failed");

    mem_buf_init(buf, max_size, init_size);

    return buf;
}

void mem_buf_destroy(struct mem_buf_t* buf)
{
    mem_buf_done(buf);
    free(buf);
}

void mem_buf_init(struct mem_buf_t* buf, size_t max_size,
    size_t init_size)
{
    if (max_size == 0)
        max_size = 1024;

    if (init_size == 0 ||
        init_size > max_size)
        init_size = max_size;

    memset(buf, 0, sizeof(struct mem_buf_t));
    buf->max_size = max_size;
    buf->init_size = init_size;
}

void mem_buf_done(struct mem_buf_t* buf)
{
    free(buf->ptr);
}

void mem_buf_enlarge(struct mem_buf_t* buf, size_t len)
{
    size_t sz;

    // stev: the main invariant of 'mem_buf_t'
    ASSERT(buf->size >= buf->len);

    if (len == 0)
        return;

    ASSERT_SIZE_ADD_NO_OVERFLOW(buf->len, len);
    if (len > buf->max_size ||
        buf->len > buf->max_size - len)
        fatal_error(
            "mem-buf: buffer enlarging "
            "exceeds maximum size");

    sz = !buf->size
        ? buf->init_size
        : buf->size;
    ASSERT(sz > 0);
    while (SIZE_SUB(sz, buf->len) < len)
        SIZE_MUL_EQ(sz, SZ(2));

    if (sz > buf->max_size)
        sz = buf->max_size;

    // stev: here we have that:
    //   len <= sz <= buf->max_size

    ASSERT(sz > 0);
    ASSERT(sz >= buf->size);
    if (buf->size < sz) {
        buf->ptr = realloc(buf->ptr, sz);
        ENSURE(buf->ptr != NULL, "realloc failed");
        buf->size = sz;
    }

    // stev: the main invariant of 'mem_buf_t':
    // this is indeed preserved since len > 0
    ASSERT(SIZE_SUB(buf->size, buf->len) >= len);
}

void mem_buf_update(
    struct mem_buf_t* buf, const uchar_t* ptr, size_t len,
    enum mem_buf_op_t op)
{
    size_t n;

    STATIC(
        mem_buf_append + 1 ==
        mem_buf_shiftin &&

        mem_buf_shiftin + 1 ==
        mem_buf_reset);

    if (op < mem_buf_append ||
        op > mem_buf_reset)
        INVALID_ARG("%d", op);

    // stev: then main invariant of 'mem_buf_t'
    ASSERT(buf->size >= buf->len);

    if (op == mem_buf_reset)
        buf->len = 0;

    if (len == 0)
        return;

    if (op != mem_buf_shiftin)
        mem_buf_enlarge(buf, len);

    // stev: the free space in buffer:
    n = SIZE_SUB(buf->size, buf->len);
    if (op != mem_buf_shiftin)
        ASSERT(len <= n);

    if (op == mem_buf_shiftin && len > buf->size) {
        ptr += len - buf->size;
        len = buf->size;
    }

    if (op == mem_buf_shiftin && len > n) {
        size_t s;

        s = SIZE_SUB(buf->size, len);

        // (0) buf->size >= n:
        //     buf->size >= buf->size - buf->len = n

        // (1) len >= n:
        //     prior the assignment 'len = buf->size' above, len > n;
        //     in case len is assigned, len = buf->size >= n, by (0)

        // (2) len <= buf->size
        // (3) s = buf->size - len >= 0     [by (2)]

        // (4) buf->len >= s:
        //     buf->len >= buf->size - len  [by (1)]
        //              = s                 [by (3)]
        // (5) buf->len - s <= buf->size:
        //     buf->len - s <= buf->len     [by s >= 0]
        //                  <= buf->size,   [by the main invariant]
        // (6) buf->size - s >= len:
        //     buf->size - s = len          [by (3)]

        // stev: (4) and (5) from above show that the expression
        // 'buf->ptr + (buf->len - s)' is valid: no underflow on
        // 'buf->len - s' and, also, the pointer expression is
        // in the valid range [buf->ptr, buf->ptr + buf->size].
        memmove(buf->ptr, buf->ptr + (buf->len - s), s);
        buf->len = s;

        // stev: (6) from above shows that the ASSERT
        // immediately below succeeds with equality
    }

    ASSERT(SIZE_SUB(buf->size, buf->len) >= len);
    memcpy(buf->ptr + buf->len, ptr, len);

    // stev: the main invariant is preserved
    // since: buf->size >= buf->len + len
    SIZE_ADD_EQ(buf->len, len);
}

JSON_API void mem_buf_shift_fit(
    struct mem_buf_t* buf, size_t len)
{
    if (len == 0)
        return;

    if (buf->len < len ||
        buf->len - len > buf->size)
        INVALID_ARG("%zu", len);

    // stev: then main invariant of 'mem_buf_t'
    ASSERT(buf->size >= buf->len);

    memmove(buf->ptr, buf->ptr + (buf->len - len), len);
    buf->len = len; 
}


