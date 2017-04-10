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

#ifndef __BIT_SET_H
#define __BIT_SET_H

#include <limits.h>

#include "json-defs.h"
#include "int-traits.h"

// stev: the discriminator of the union
// below is 'size' by the conditions:
//   size <= BIT_SET_VAL_BITS => use 'val'
//   size >  BIT_SET_VAL_BITS => use 'ptr'

struct bit_set_t
{
    size_t size;
    union {
        uintptr_t val;
        uchar_t*  ptr;
    };
};

size_t bit_set_get_size(
    const struct bit_set_t* set);

void bit_set_print(
    const struct bit_set_t* set, FILE* file);

#ifdef JSON_DEBUG
void bit_set_print_debug(
    const struct bit_set_t* set, FILE* file);
#endif

#define TYPEOF_IS_BIT_SET_(q, x)            \
    (                                       \
        TYPEOF_IS(x, q struct bit_set_t*)   \
    )
#define TYPEOF_IS_BIT_SET_CONST(x)          \
    (                                       \
        TYPEOF_IS_BIT_SET_(const, x) ||     \
        TYPEOF_IS_BIT_SET_(, x)             \
    )
#define TYPEOF_IS_BIT_SET(x)                \
    TYPEOF_IS_BIT_SET_(, x)

#define BIT_SET_VAL_BITS TYPE_WIDTH(uintptr_t)

#define BIT_SET_IS_VAL_(b)                  \
    (                                       \
        (b)->size <= BIT_SET_VAL_BITS       \
    )

#define BIT_SET_INIT(b, a, s)               \
    do {                                    \
        STATIC(TYPEOF_IS_SIZET(s));         \
        STATIC(TYPEOF_IS_BIT_SET(b));       \
        if ((s) <= BIT_SET_VAL_BITS)        \
            (b)->val = SZ(0);               \
        else {                              \
            size_t __s = BIT_SET_WIDTH_(s); \
            (b)->ptr = a(__s);              \
            memset((b)->ptr, 0, __s);       \
        }                                   \
        (b)->size = s;                      \
    } while (0)

#define BIT_SET_DONE(b, d)                  \
    do {                                    \
        STATIC(TYPEOF_IS_BIT_SET(b));       \
        if (!BIT_SET_IS_VAL_(b))            \
            d((b)->ptr);                    \
    } while (0)

// stev: BIT_SET_NTH_(n) is well-defined <=> n < SIZE_BIT,
// for n of type size_t

#define BIT_SET_NTH_(n)                     \
    (                                       \
        SZ(1) << (n)                        \
    )

#define BIT_SET_POS_(i)                     \
    (                                       \
        (i) / CHAR_BIT                      \
    )

// stev: BIT_SET_VAL_BITS <= SIZE_BIT <=>
//       TYPE_WIDTH(uintptr_t) <= TYPE_WIDTH(size_t) <=>
//       sizeof(uintptr_t) <= sizeof(size_t)

// stev: UINTPTR_MAX <= SIZE_MAX <=>
//       TYPE_WIDTH(uintptr_t) <= TYPE_WIDTH(size_t),
// since both UINTPTR_MAX and SIZE_MAX are powers of 2

// stev: CHAR_BIT <= SIZE_BIT =>
//       BIT_SET_NTH_(i mod CHAR_BIT) is well-defined:
// indeed: (i mod CHAR_BIT) < CHAR_BIT <= SIZE_BIT

#define BIT_SET_BIT_(i)                     \
    (                                       \
        STATIC(SIZE_BIT >= CHAR_BIT),       \
        STATIC(SIZE_MAX >= UINTPTR_MAX),    \
        STATIC(SIZE_IS_POW2((size_t)        \
            CHAR_BIT)),                     \
        BIT_SET_NTH_((i) & (CHAR_BIT - 1))  \
    )

#define BIT_SET_WIDTH_(n)                   \
    ({                                      \
        size_t __r = BIT_SET_POS_(n);       \
        BIT_SET_BIT_(n)                     \
        ? __r + 1                           \
        : __r;                              \
    })

#define BIT_SET_POS_END_(b)                 \
    (                                       \
        BIT_SET_WIDTH_((b)->size)           \
    )

#define BIT_SET_ASSERT_POS_(b, i)           \
    ASSERT((i) < (b)->size)

// stev: BIT_SET_ASSERT_POS_(b, i) && BIT_SET_IS_VAL_(b) =>
//       BIT_SET_NTH_(i) is well-defined:
// indeed, from hypothesis
// => i < b->size <= BIT_SET_VAL_BITS <= SIZE_BIT
// => i < SIZE_BIT => BIT_SET_NTH_(i) is well-defined

// stev: BIT_SET_ASSERT_POS_(b, i) =>
//       b->ptr[BIT_SET_POS_(i)] is well-defined:
// indeed, BIT_SET_ASSERT_POS_(b, i) <=> i < b->size
// => BIT_SET_POS_(i) < BIT_SET_POS_END_(b)
// => BIT_SET_POS_(i) is in range of b->ptr

#define BIT_SET_TEST(b, i)                  \
    ({                                      \
        STATIC(TYPEOF_IS_BIT_SET_CONST(b)); \
        STATIC(TYPEOF_IS_SIZET(i));         \
        BIT_SET_ASSERT_POS_(b, i);          \
        BIT_SET_IS_VAL_(b)                  \
        ? (b)->val & BIT_SET_NTH_(i)        \
        : (b)->ptr[BIT_SET_POS_(i)] &       \
                   BIT_SET_BIT_(i);         \
    })

#define BIT_SET_SET(b, i)                   \
    ({                                      \
        STATIC(TYPEOF_IS_BIT_SET(b));       \
        STATIC(TYPEOF_IS_SIZET(i));         \
        BIT_SET_ASSERT_POS_(b, i);          \
        if (BIT_SET_IS_VAL_(b))             \
            (b)->val |= BIT_SET_NTH_(i);    \
        else                                \
            (b)->ptr[BIT_SET_POS_(i)] |=    \
                     BIT_SET_BIT_(i);       \
    })

#define BIT_SET_RESET(b, i)                 \
    ({                                      \
        STATIC(TYPEOF_IS_BIT_SET(b));       \
        STATIC(TYPEOF_IS_SIZET(i));         \
        BIT_SET_ASSERT_POS_(b, i);          \
        if (BIT_SET_IS_VAL_(b))             \
            (b)->val &= BIT_SET_NTH_(i);    \
        else                                \
            (b)->ptr[BIT_SET_POS_(i)] &=    \
                    ~BIT_SET_BIT_(i);       \
    })

#endif /* __BIT_SET_H */


