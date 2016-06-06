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

#ifndef TREAP_NAME
#error  TREAP_NAME is not defined
#endif

#ifndef TREAP_KEY_TYPE
#error  TREAP_KEY_TYPE is not defined
#endif

#ifndef TREAP_VAL_TYPE
#error  TREAP_VAL_TYPE is not defined
#endif

#ifndef TREAP_KEY_IS_EQ
#error  TREAP_KEY_IS_EQ is not defined
#endif

#ifndef TREAP_KEY_CMP
#error  TREAP_KEY_CMP is not defined
#endif

#ifndef TREAP_KEY_ASSIGN
#error  TREAP_KEY_ASSIGN is not defined
#endif

#ifndef TREAP_ALLOC_OBJ_TYPE
#error  TREAP_ALLOC_OBJ_TYPE is not defined
#endif

#define TREAP_MAKE_NAME_(n, s) n ## _treap_ ## s
#define TREAP_MAKE_NAME(n, s)  TREAP_MAKE_NAME_(n, s)

#define TREAP_TYPE_(n)         TREAP_MAKE_NAME(n, t)
#define TREAP_NODE_TYPE_(n)    TREAP_MAKE_NAME(n, node_t)

#define TREAP_TYPE             TREAP_TYPE_(TREAP_NAME)
#define TREAP_NODE_TYPE        TREAP_NODE_TYPE_(TREAP_NAME)

#define TREAP_ALLOC_FUNC_TYPE  TREAP_MAKE_NAME(TREAP_NAME, alloc_func_t)
#define TREAP_INIT             TREAP_MAKE_NAME(TREAP_NAME, init)

#define TREAP_LOOKUP_KEY       TREAP_MAKE_NAME(TREAP_NAME, lookup_key)
#define TREAP_INSERT_KEY       TREAP_MAKE_NAME(TREAP_NAME, insert_key)

// stev: the treap data structure implemented below is due to the work
// of Knuth:
//
// Donald E. Knuth: MMIXware: A RISC Computer for the Third Millennium,
// Heidelberg: Springer-Verlag, 1999, viii + 550pp.
// ISBN 978-3-540-66938-8
//
// Chapter: MMIX-SIM: Simulated memory, sections 16-22, pp. 347-349

// stev: note that upon Knuth's own account, his treap data structure
// follows from ideas of Vuillemin, Aragon and Seidel [CACM 23 (1980),
// 229-239 and IEEE Symp. on Found. of Comp. Sci. 30 (1989), 540-546].

struct TREAP_NODE_TYPE
{
    size_t stamp;
    TREAP_KEY_TYPE key;
    TREAP_VAL_TYPE val;

    struct TREAP_NODE_TYPE* left;
    struct TREAP_NODE_TYPE* right;
};

#ifndef TREAP_KEY_CMP_TYPE
#define TREAP_KEY_CMP_TYPE
enum treap_key_cmp_t
{
    treap_key_cmp_eq,
    treap_key_cmp_lt,
    treap_key_cmp_gt,
};
#endif

#ifdef TREAP_KEY_CMP_NAME
static enum treap_key_cmp_t TREAP_KEY_CMP_NAME(
    TREAP_KEY_TYPE, TREAP_KEY_TYPE);
#endif

typedef struct TREAP_NODE_TYPE* (*TREAP_ALLOC_FUNC_TYPE)(
    TREAP_ALLOC_OBJ_TYPE*);

struct TREAP_TYPE
{
    TREAP_ALLOC_FUNC_TYPE   alloc_func;
    TREAP_ALLOC_OBJ_TYPE*   alloc_obj;
    struct TREAP_NODE_TYPE* root;
    struct TREAP_NODE_TYPE* last;
    size_t priority;
};

void TREAP_INIT(
    struct TREAP_TYPE* treap,
    TREAP_ALLOC_FUNC_TYPE alloc_func,
    TREAP_ALLOC_OBJ_TYPE* alloc_obj)
{
    memset(treap, 0, sizeof(struct TREAP_TYPE));

    treap->alloc_func = alloc_func;
    treap->alloc_obj = alloc_obj;

    treap->priority = 314159265;
}

static bool TREAP_LOOKUP_KEY(
    struct TREAP_TYPE* treap, TREAP_KEY_TYPE key,
    struct TREAP_NODE_TYPE** result)
{
    struct TREAP_NODE_TYPE *node;
    enum treap_key_cmp_t cmp;

    if (treap->last != NULL &&
        TREAP_KEY_IS_EQ(treap->last->key, key)) {
        *result = treap->last;
        return true;
    }

    for (node = treap->root; node; ) {
        cmp = TREAP_KEY_CMP(key, node->key);
        if (cmp == treap_key_cmp_eq) {
            treap->last = node;
            *result = node;
            return true;
        }
        else
        if (cmp == treap_key_cmp_lt)
            node = node->left;
        else
        if (cmp == treap_key_cmp_gt)
            node = node->right;
        else
            UNEXPECT_VAR("%d", cmp);
    }

    *result = NULL;
    return false;
}

// stev: we presume that 'alloc_func' returns
// a pointer to a zeroed-out 'TREAP_NODE_TYPE'

#define TREAP_NEW_NODE()                         \
    ({                                           \
        struct TREAP_NODE_TYPE* __r =            \
            treap->alloc_func(treap->alloc_obj); \
        __r->stamp = treap->priority;            \
        /* floor((2 ** 32) * (phi - 1)) */       \
        treap->priority += 0x9e3779b9;           \
        __r;                                     \
    })

static bool TREAP_INSERT_KEY(
    struct TREAP_TYPE* treap, TREAP_KEY_TYPE key,
    struct TREAP_NODE_TYPE** result)
{
    struct TREAP_NODE_TYPE *node;
    struct TREAP_NODE_TYPE **left, **right;
    struct TREAP_NODE_TYPE** ptr;
    enum treap_key_cmp_t cmp;

    if (treap->last != NULL &&
        TREAP_KEY_IS_EQ(treap->last->key, key)) {
        *result = treap->last;
        return false;
    }

    for (node = treap->root; node; ) {
        cmp = TREAP_KEY_CMP(key, node->key);
        if (cmp == treap_key_cmp_eq) {
            *result = treap->last = node;
            return false;
        }
        else
        if (cmp == treap_key_cmp_lt)
            node = node->left;
        else
        if (cmp == treap_key_cmp_gt)
            node = node->right;
        else
            UNEXPECT_VAR("%d", cmp);
    }

    for (node = treap->root, ptr = &treap->root;
         node && node->stamp < treap->priority;
         node = *ptr) {
        cmp = TREAP_KEY_CMP(key, node->key);
        ASSERT(cmp != treap_key_cmp_eq);
        if (cmp == treap_key_cmp_lt)
            ptr = &node->left;
        else
        if (cmp == treap_key_cmp_gt)
            ptr = &node->right;
        else
            UNEXPECT_VAR("%d", cmp);
    }
    ASSERT(*ptr == node);

    *ptr = TREAP_NEW_NODE();
    TREAP_KEY_ASSIGN((*ptr)->key, key);

    left = &(*ptr)->left;
    right = &(*ptr)->right;
    while (node) {
        cmp = TREAP_KEY_CMP(key, node->key);
        //!!!CMP ASSERT(cmp != treap_key_cmp_eq);
        if (cmp == treap_key_cmp_lt) {
            *right = node;
            right = &node->left;
            node = *right;
        }
        else
        if (cmp == treap_key_cmp_eq || //!!!CMP
            cmp == treap_key_cmp_gt) {
            *left = node;
            left = &node->right;
            node = *left;
        }
        else
            UNEXPECT_VAR("%d", cmp);
    }
    *left = NULL;
    *right = NULL;

    *result = treap->last = *ptr;
    return true;
}

#ifndef __TREAP_IMPL_H
#define __TREAP_IMPL_H

#define TREAP_TYPEOF_IS_TREAP_(q, t, p) \
    TYPEOF_IS(p, q struct TREAP_TYPE_(t)*)

#define TREAP_IS_EMPTY_(q, t, p)                 \
    (                                            \
        STATIC(TREAP_TYPEOF_IS_TREAP_(q, t, p)), \
        (p)->root == NULL                        \
    )
#define TREAP_IS_EMPTY(t, p) \
    TREAP_IS_EMPTY_(, t, p)
#define TREAP_IS_EMPTY_CONST(t, p) \
    TREAP_IS_EMPTY_(const, t, p)

#define TREAP_TYPEOF_IS_NODE(t, p) \
    TYPEOF_IS(p, struct TREAP_NODE_TYPE_(t)*)

#endif/*__TREAP_IMPL_H*/


