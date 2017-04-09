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

#include "ptr-traits.h"

#ifndef VSTACK_NAME
#error  VSTACK_NAME is not defined
#endif

#ifndef VSTACK_NODE_NAME
#error  VSTACK_NODE_NAME is not defined
#endif

#ifndef VSTACK_ELEM_TYPE
#define VSTACK_ELEM_TYPE uchar_t
#endif

#define VSTACK_MAKE_NAME_(n, s) n ## _stack_ ## s
#define VSTACK_MAKE_NAME(n, s)  VSTACK_MAKE_NAME_(n, s)

#define VSTACK_TYPE       VSTACK_MAKE_NAME(VSTACK_NAME, t)
#define VSTACK_POOL_TYPE  VSTACK_MAKE_NAME(VSTACK_NAME, pool_t)
#define VSTACK_CREATE     VSTACK_MAKE_NAME(VSTACK_NAME, create)
#define VSTACK_DESTROY    VSTACK_MAKE_NAME(VSTACK_NAME, destroy)
#define VSTACK_INIT       VSTACK_MAKE_NAME(VSTACK_NAME, init)
#define VSTACK_DONE       VSTACK_MAKE_NAME(VSTACK_NAME, done)
#define VSTACK_PUSH       VSTACK_MAKE_NAME(VSTACK_NAME, push)
#define VSTACK_POP        VSTACK_MAKE_NAME(VSTACK_NAME, pop)

#define VSTACK_MAKE_NODE_NAME_(n, s) n ## _ ## s
#define VSTACK_MAKE_NODE_NAME(n, s)  VSTACK_MAKE_NODE_NAME_(n, s)

#define VSTACK_NODE_TYPE  VSTACK_MAKE_NODE_NAME(VSTACK_NODE_NAME, t)

// stev: the stack implementation below
// allows variable-length nodes as its
// elements -- with the added feature
// of keeping valid the pointers to the
// nodes it holds throughout any series
// of push/pop operations called on the
// structure

// stev: for the top of stack, the chain
// <top, top->prev, ...> is comprised of
// each and every node of the stack, in
// the reverse order of which they were
// pushed in the structure

// stev: for any given node, the chain
// <node, node->next, ..., node> is
// circular and comprised of each and
// every node of the stack, in the order
// that they were pushed in; therefore,
// the in-order layout of the stack is:
// <top->next, top->next->next, ..., top>

struct VSTACK_NODE_TYPE
{
    size_t size;
#ifdef VSTACK_DEBUG
    size_t align;
#endif
    struct VSTACK_NODE_TYPE* prev;
#ifdef VSTACK_NEED_CIRCULAR_LINKS
    struct VSTACK_NODE_TYPE* next;
#endif
    VSTACK_ELEM_TYPE ptr[0];
};

struct VSTACK_POOL_TYPE
{
    struct VSTACK_POOL_TYPE* prev;
    struct VSTACK_POOL_TYPE* next;
    size_t size;
    size_t used;
    char   buf[0];
};

// stev: the chain <pool->next, ...>
// is comprised of free pools, while
// the chain <pool, pool->prev, ...>
// is made of in-use stacked pools;
// upon certain patterns of pushes on
// and pops off the stack, any pool in
// use can actually become free; note
// that a free pool is characterized
// by 'used == 0'!

// stev: note that 'max_size' below
// is to be understood as the upper
// bound for the total amount of
// memory allocatable to stack nodes;
// this amount is not including the
// memory consumed by pool structure
// headers (the fixed-size part of
// struct VSTACK_POOL_TYPE)

struct VSTACK_TYPE
{
    struct VSTACK_POOL_TYPE* pool;
    struct VSTACK_NODE_TYPE* top;
    size_t init_size;
    size_t max_size;
    size_t used;
};

#define TYPEOF_IS_VSTACK(x) \
    TYPEOF_IS(x, struct VSTACK_TYPE*)
#define TYPEOF_IS_VSTACK_POOL(x) \
    TYPEOF_IS(x, struct VSTACK_POOL_TYPE*)
#define TYPEOF_IS_VSTACK_NODE(x) \
    TYPEOF_IS(x, struct VSTACK_NODE_TYPE*)

#define VSTACK_ASSERT_NODE_INVARIANTS(q)  \
    do {                                  \
        STATIC(TYPEOF_IS_VSTACK(stack));  \
        STATIC(TYPEOF_IS_VSTACK_POOL(p)); \
        STATIC(TYPEOF_IS_VSTACK_NODE(q)); \
        ASSERT((q)->size > 0);            \
    } while (0)

#define VSTACK_ASSERT_POOL_INVARIANTS(p)    \
    do {                                    \
        STATIC(TYPEOF_IS_VSTACK(stack));    \
        STATIC(TYPEOF_IS_VSTACK_POOL(p));   \
        ASSERT((p)->used <= (p)->size);     \
        ASSERT((stack)->used >= (p)->used); \
    } while (0)

#define VSTACK_ASSERT_INVARIANTS(stack)                  \
    do {                                                 \
        STATIC(TYPEOF_IS_VSTACK(stack));                 \
        /* top != NULL => pool != NULL */                \
        ASSERT(!(stack)->top || (stack)->pool);          \
        ASSERT((stack)->used <= (stack)->max_size);      \
        ASSERT((stack)->init_size <= (stack)->max_size); \
    } while (0)

static void VSTACK_INIT(
    struct VSTACK_TYPE* stack,
    size_t max_size, size_t init_size)
{
    const size_t M = SIZE_MAX / 2;

    if (max_size == 0 ||
        max_size > M)
        max_size = M;

    if (init_size == 0)
        init_size = 1024;

    if (init_size > max_size)
        init_size = max_size;

    memset(stack, 0, sizeof(struct VSTACK_TYPE));

    stack->init_size = init_size;
    stack->max_size = max_size;
}

static void VSTACK_DONE(
    struct VSTACK_TYPE* stack)
{
    struct VSTACK_POOL_TYPE *p, *r;

    VSTACK_ASSERT_INVARIANTS(stack);

    p = stack->pool;
    if (p == NULL)
        return;

    while (p->next) {
        p = p->next;
        // p must be a free pool
        ASSERT(!p->used);
    }

    do {
        VSTACK_ASSERT_POOL_INVARIANTS(p);
        r = p->prev;
        free(p);
    } while ((p = r));
}

#ifdef VSTACK_NEED_CREATE_DESTROY

static struct VSTACK_TYPE* VSTACK_CREATE(
    size_t max_size, size_t init_size)
{
    struct VSTACK_TYPE* stack;

    stack = malloc(sizeof(struct VSTACK_TYPE));
    ENSURE(stack != NULL, "malloc failed");

    VSTACK_INIT(stack, max_size, init_size);

    return stack;
}

static void VSTACK_DESTROY(
    struct VSTACK_TYPE* stack)
{
    VSTACK_DONE(stack);
    free(stack);
}

#endif // VSTACK_NEED_CREATE_DESTROY

static struct VSTACK_NODE_TYPE* VSTACK_PUSH(
    struct VSTACK_TYPE* stack,
    size_t len)
{
    const size_t l =
        sizeof(struct VSTACK_POOL_TYPE);
    const size_t m =
        MEM_ALIGNOF(struct VSTACK_NODE_TYPE);
    const size_t n =
        sizeof(struct VSTACK_NODE_TYPE);

    struct VSTACK_POOL_TYPE *p, *d;
    struct VSTACK_NODE_TYPE* q;
    size_t a, s = len, u, r, t;

    VSTACK_ASSERT_INVARIANTS(stack);

    ASSERT_SIZE_ADD_NO_OVERFLOW(s, n);
    s += n;

    for (p = stack->pool, d = NULL; p; d = p, p = p->next) {
        VSTACK_ASSERT_POOL_INVARIANTS(p);
        // p != stack->pool => p is free
        if (p != stack->pool)
            ASSERT(!p->used);

        a = PTR_ALIGN_SIZE(p->buf + p->used, m);
        ASSERT_SIZE_ADD_NO_OVERFLOW(s, a);
        if (p->size - p->used >= s + a) {
            s += a;
            break;
        }
    }
    // => p == NULL || p->size - p->used >= s

    if (p == NULL) {
        ASSERT_SIZE_ADD_NO_OVERFLOW(s, m);
        s += m;
    }

    u = stack->max_size - stack->used;
    ASSERT_SIZE_ADD_NO_OVERFLOW(s, l);
    ENSURE(s + l <= u, "stack overflow (by %zu bytes)",
        SIZE_SUB(s, u));

    if (p == NULL) {
        size_t t = stack->init_size;

        if (t > u)
            t = u;
        // => t <= u
        if (t < s)
            t = s;
        // => s <= t <= u

        VERIFY_SIZE_ADD_NO_OVERFLOW(l, t);
        p = malloc(l + t);
        ENSURE(p != NULL, "malloc failed");

        memset(p, 0, l);
        // => p->prev == NULL &&
        //    p->next == NULL &&
        //    p->used == 0

        p->size = t;  // => p->size >= s
        if (d != NULL) {
            p->prev = d;
            d->next = p;
        }

        VERIFY_SIZE_MUL_NO_OVERFLOW(t, SZ(2));
        t *= SZ(2);

        if (stack->init_size < t)
            stack->init_size = t;

        if (stack->init_size > stack->max_size)
            stack->init_size = stack->max_size;

        a = PTR_ALIGN_SIZE(p->buf, m);

        // =>  p->size - p->used = p->size >= s
        ASSERT(p->size - p->used >= s);
    }
    // => p != NULL && p->size - p->used >= s

    t = len + n + a;
    // s = len + n + a  = t, or
    // s = len + n + m >= t  [m >= a]
    // => p->size - p->used >= s >= t
    ASSERT(p->size - p->used >= t);

    // stack->used + t < stack->max_size:
    //   t <= s < s + l <= u =>
    //   t <  u == max_size - used <=>
    //   used + t < max_size
    stack->used += t;

    r = p->used + a;
    // p->used + t <= p->size   [by the ASSERT above]
    p->used += t;

    q = (struct VSTACK_NODE_TYPE*) (p->buf + r);

    q->size = t;
#ifdef VSTACK_DEBUG
    q->align = a;
#endif
    q->prev = stack->top;
#ifdef VSTACK_NEED_CIRCULAR_LINKS
    if (q->prev != NULL) {
        q->next = q->prev->next;
        q->prev->next = q;
    }
    else
        q->next = q;
#endif
    stack->pool = p;
    stack->top = q;

    return q;
}

#ifdef VSTACK_DEBUG
#define VSTACK_ASSERT_TOP_POOL_INVARIANTS(stack) \
    do {                                         \
        size_t __u =                             \
            (stack)->pool->used;                 \
        if ((stack)->top != NULL) {              \
            size_t __b =                         \
                PTR_TO_SIZE((stack)->pool->buf); \
            size_t __q =                         \
                PTR_TO_SIZE((stack)->top);       \
            size_t __a =                         \
                (stack)->top->align;             \
            size_t __s =                         \
                (stack)->top->size;              \
            SIZE_ADD_EQ(__b, __u);               \
            SIZE_ADD_EQ(__b, __a);               \
            SIZE_ADD_EQ(__q, __s);               \
            ASSERT(__b == __q);                  \
        }                                        \
        else                                     \
            ASSERT(__u == 0);                    \
    } while (0)
#else // VSTACK_DEBUG
#define VSTACK_ASSERT_TOP_POOL_INVARIANTS(stack) \
    do {} while (0)
#endif // VSTACK_DEBUG

static void VSTACK_POP(
    struct VSTACK_TYPE* stack)
{
    struct VSTACK_POOL_TYPE* p;
    struct VSTACK_NODE_TYPE* q;

    VSTACK_ASSERT_INVARIANTS(stack);

    q = stack->top;
    ENSURE(q != NULL, "stack underflow");
    VSTACK_ASSERT_NODE_INVARIANTS(q);

    p = stack->pool;
    VERIFY(p != NULL);
    VSTACK_ASSERT_POOL_INVARIANTS(p);

    p->used -= q->size;
    stack->used -= q->size;
#ifdef VSTACK_NEED_CIRCULAR_LINKS
    if (q->prev != NULL) {
        ASSERT(q->next != q);
        ASSERT(q->next != NULL);
        q->prev->next = q->next;
    }
#endif
    stack->top = q->prev;

    if (p->used)
        goto assert_top;

    while (p->prev && !p->prev->used) {
        VSTACK_ASSERT_POOL_INVARIANTS(
            p->prev);
        p = p->prev;
    }

    // p->prev != NULL =>
    // p->prev->used != 0
    stack->pool = p->prev
        ? p->prev
        : p;
    // => stack->pool != NULL

assert_top:
    VSTACK_ASSERT_TOP_POOL_INVARIANTS(
        stack);
}


