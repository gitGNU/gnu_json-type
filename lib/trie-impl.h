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

#ifndef TRIE_NAME
#error  TRIE_NAME is not defined
#endif

#ifndef TRIE_SYM_TYPE
#error  TRIE_SYM_TYPE is not defined
#endif

#ifndef TRIE_VAL_TYPE
#error  TRIE_VAL_TYPE is not defined
#endif

#ifndef TRIE_NEED_STRUCT_ONLY

#if defined(TRIE_NEED_LOOKUP_SYM) || \
    defined(TRIE_NEED_INSERT_SYM) || \
    defined(TRIE_NEED_NODE_HAS_NULL_SIB)
#ifndef TRIE_NULL_SYM
#error  TRIE_NULL_SYM is not defined
#endif
#endif

#ifndef TRIE_SYM_IS_NULL
#error  TRIE_SYM_IS_NULL is not defined
#endif

#ifdef  TRIE_NEED_MATCH_SYM_STAR
#ifndef TRIE_SYM_IS_ANY
#error  TRIE_SYM_IS_ANY is not defined
#endif
#endif

#ifndef TRIE_SYM_CMP
#error  TRIE_SYM_CMP is not defined
#endif

#ifdef  TRIE_NEED_PRINT

#ifndef TRIE_SYM_PRINT
#error  TRIE_SYM_PRINT is not defined
#endif

#ifndef TRIE_VAL_PRINT
#error  TRIE_VAL_PRINT is not defined
#endif

#endif // TRIE_NEED_PRINT

#ifndef TRIE_KEY_TYPE
#error  TRIE_KEY_TYPE is not defined
#endif

#ifndef TRIE_KEY_INC
#error  TRIE_KEY_INC is not defined
#endif

#ifndef TRIE_KEY_DEREF
#error  TRIE_KEY_DEREF is not defined
#endif

#ifdef  TRIE_NEED_VALIDATE

#ifndef TRIE_PTR_SPACE_TYPE
#error  TRIE_PTR_SPACE_TYPE is not defined
#endif

#ifndef TRIE_VALIDATE_PTR_SPACE_INSERT
#error  TRIE_VALIDATE_PTR_SPACE_INSERT is not defined
#endif

#ifndef TRIE_SYM_VALIDATE
#error  TRIE_SYM_VALIDATE is not defined
#endif

#ifndef TRIE_VAL_VALIDATE
#error  TRIE_VAL_VALIDATE is not defined
#endif

#endif // TRIE_NEED_VALIDATE

#ifdef  TRIE_NEED_GEN_DEF

#ifndef TRIE_PTR_SPACE_TYPE
#error  TRIE_PTR_SPACE_TYPE is not defined
#endif

#ifndef TRIE_PTR_SPACE_NODE_TYPE
#error  TRIE_PTR_SPACE_NODE_TYPE is not defined
#endif

#ifndef TRIE_GEN_DEF_PTR_SPACE_INSERT
#error  TRIE_GEN_DEF_PTR_SPACE_INSERT is not defined
#endif

#ifndef TRIE_GEN_DEF_PTR_SPACE_LOOKUP
#error  TRIE_GEN_DEF_PTR_SPACE_LOOKUP is not defined
#endif

#ifndef TRIE_SYM_GEN_DEF
#error  TRIE_SYM_GEN_DEF is not defined
#endif

#endif // TRIE_NEED_GEN_DEF

#ifndef TRIE_NEED_IMPL_ONLY 

#ifndef TRIE_ALLOC_OBJ_TYPE
#error  TRIE_ALLOC_OBJ_TYPE is not defined
#endif

#endif // TRIE_NEED_IMPL_ONLY

#endif // TRIE_NEED_STRUCT_ONLY

#define TRIE_MAKE_NAME_(n, s) n ## _trie_ ## s
#define TRIE_MAKE_NAME(n, s)  TRIE_MAKE_NAME_(n, s)

#define TRIE_TYPE_(n)        TRIE_MAKE_NAME(n, t)
#define TRIE_NODE_TYPE_(n)   TRIE_MAKE_NAME(n, node_t)

#define TRIE_TYPE            TRIE_TYPE_(TRIE_NAME)
#define TRIE_NODE_TYPE       TRIE_NODE_TYPE_(TRIE_NAME)

#define TRIE_ALLOC_FUNC_TYPE TRIE_MAKE_NAME(TRIE_NAME, alloc_func_t)
#define TRIE_INIT            TRIE_MAKE_NAME(TRIE_NAME, init)

#define TRIE_LOOKUP_SYM_ANY  TRIE_MAKE_NAME(TRIE_NAME, lookup_sym_any)
#define TRIE_LOOKUP_SYM_ANY_NODE \
                             TRIE_MAKE_NAME(TRIE_NAME, lookup_sym_any_node)
#define TRIE_LOOKUP_SYM_STAR TRIE_MAKE_NAME(TRIE_NAME, lookup_sym_star)
#define TRIE_MATCH_SYM_ANY_NODE \
                             TRIE_MAKE_NAME(TRIE_NAME, match_sym_any_node)
#define TRIE_MATCH_SYM_ANY   TRIE_MAKE_NAME(TRIE_NAME, match_sym_any)
#define TRIE_MATCH_SYM_STAR_NODE \
                             TRIE_MAKE_NAME(TRIE_NAME, match_sym_star_node)
#define TRIE_MATCH_SYM_STAR  TRIE_MAKE_NAME(TRIE_NAME, match_sym_star)
#define TRIE_MATCH_SYM_NODE  TRIE_MAKE_NAME(TRIE_NAME, match_sym_node)
#define TRIE_MATCH_SYM       TRIE_MAKE_NAME(TRIE_NAME, match_sym)

#define TRIE_NODE_COND_FUNC_TYPE \
                             TRIE_MAKE_NAME(TRIE_NAME, node_cond_func_t)

#define TRIE_LOOKUP_FIRST_NODE \
                             TRIE_MAKE_NAME(TRIE_NAME, lookup_first_node)

#define TRIE_LOOKUP_SYM_NODE TRIE_MAKE_NAME(TRIE_NAME, lookup_sym_node)
#define TRIE_LOOKUP_SYM      TRIE_MAKE_NAME(TRIE_NAME, lookup_sym)
#define TRIE_INSERT_SYM      TRIE_MAKE_NAME(TRIE_NAME, insert_sym)
#define TRIE_INSERT_KEY      TRIE_MAKE_NAME(TRIE_NAME, insert_key)

#define TRIE_NODE_GET_SHORTEST_KEY_LEN \
                             TRIE_MAKE_NAME(TRIE_NAME, node_get_shortest_key_len)
#define TRIE_NODE_GET_LONGEST_KEY_LEN \
                             TRIE_MAKE_NAME(TRIE_NAME, node_get_longest_key_len)

#define TRIE_NODE_GET_ITER_MAX_DEPTH \
                             TRIE_MAKE_NAME(TRIE_NAME, node_get_iter_max_depth)
#define TRIE_NODE_GET_SIB_ITER_MAX_DEPTH \
                             TRIE_MAKE_NAME(TRIE_NAME, node_get_sib_iter_max_depth)
#define TRIE_NODE_GET_LVL_ITER_MAX_DEPTH \
                             TRIE_MAKE_NAME(TRIE_NAME, node_get_lvl_iter_max_depth)

#define TRIE_PRINT_NODE      TRIE_MAKE_NAME(TRIE_NAME, print_node)
#define TRIE_PRINT           TRIE_MAKE_NAME(TRIE_NAME, print)
#define TRIE_NODE_GET_LEAF   TRIE_MAKE_NAME(TRIE_NAME, node_get_leaf)
#define TRIE_NODE_GET_NODE_COUNT \
                             TRIE_MAKE_NAME(TRIE_NAME, node_get_node_count)
#define TRIE_GET_NODE_COUNT  TRIE_MAKE_NAME(TRIE_NAME, get_node_count)
#define TRIE_NODE_GET_KEY_COUNT TRIE_MAKE_NAME(TRIE_NAME, node_get_key_count)
#define TRIE_NODE_GET_SIB_LVL_COUNT \
                             TRIE_MAKE_NAME(TRIE_NAME, node_get_sib_lvl_count)
#define TRIE_NODE_GET_SIB_COUNT \
                             TRIE_MAKE_NAME(TRIE_NAME, node_get_sib_count)
#define TRIE_NODE_GET_SIB_COUNT_IF \
                             TRIE_MAKE_NAME(TRIE_NAME, node_get_sib_count_if)
#define TRIE_NODE_HAS_NULL_SIB \
                             TRIE_MAKE_NAME(TRIE_NAME, node_has_null_sib)
#define TRIE_NODE_HAS_NON_NULL_SIB \
                             TRIE_MAKE_NAME(TRIE_NAME, node_has_non_null_sib)

#define TRIE_VALIDATE        TRIE_MAKE_NAME(TRIE_NAME, validate)
#define TRIE_NODE_VALIDATE   TRIE_MAKE_NAME(TRIE_NAME, node_validate)
#define TRIE_VALIDATE        TRIE_MAKE_NAME(TRIE_NAME, validate)
#define TRIE_NODE_GEN_DEF    TRIE_MAKE_NAME(TRIE_NAME, node_gen_def)
#define TRIE_GEN_DEF         TRIE_MAKE_NAME(TRIE_NAME, gen_def)

#define TRIE_SYM_STACK_TYPE  TRIE_MAKE_NAME(TRIE_NAME, sym_stack_t)
#define TRIE_SYM_STACK_INIT  TRIE_MAKE_NAME(TRIE_NAME, sym_stack_init)
#define TRIE_SYM_STACK_DONE  TRIE_MAKE_NAME(TRIE_NAME, sym_stack_done)
#define TRIE_SYM_STACK_PUSH  TRIE_MAKE_NAME(TRIE_NAME, sym_stack_push)
#define TRIE_SYM_STACK_POP   TRIE_MAKE_NAME(TRIE_NAME, sym_stack_pop)
#define TRIE_SYM_STACK_BEG_REF \
                             TRIE_MAKE_NAME(TRIE_NAME, sym_stack_beg_ref)
#define TRIE_SYM_STACK_NAME  TRIE_MAKE_NAME(TRIE_NAME, sym)

#define TRIE_ITER_STACK_TYPE TRIE_MAKE_NAME(TRIE_NAME, iter_stack_t)
#define TRIE_ITER_STACK_ELEM_TYPE \
                             TRIE_MAKE_NAME(TRIE_NAME, iter_stack_elem_t)
#define TRIE_ITER_STACK_INIT TRIE_MAKE_NAME(TRIE_NAME, iter_stack_init)
#define TRIE_ITER_STACK_DONE TRIE_MAKE_NAME(TRIE_NAME, iter_stack_done)
#define TRIE_ITER_STACK_NAME TRIE_MAKE_NAME(TRIE_NAME, iter)

#define TRIE_ITERATOR_TYPE   TRIE_MAKE_NAME(TRIE_NAME, iterator_t)
#define TRIE_ITERATOR_INIT   TRIE_MAKE_NAME(TRIE_NAME, iterator_init)
#define TRIE_ITERATOR_DONE   TRIE_MAKE_NAME(TRIE_NAME, iterator_done)
#define TRIE_ITERATOR_AT_END TRIE_MAKE_NAME(TRIE_NAME, iterator_at_end)
#define TRIE_ITERATOR_DEREF  TRIE_MAKE_NAME(TRIE_NAME, iterator_deref)
#define TRIE_ITERATOR_INC    TRIE_MAKE_NAME(TRIE_NAME, iterator_inc)

#define TRIE_SIB_ITER_STACK_TYPE TRIE_MAKE_NAME(TRIE_NAME, sib_iter_stack_t)
#define TRIE_SIB_ITER_STACK_ELEM_TYPE TRIE_MAKE_NAME(TRIE_NAME, sib_iter_stack_elem_t)
#define TRIE_SIB_ITER_STACK_INIT TRIE_MAKE_NAME(TRIE_NAME, sib_iter_stack_init)
#define TRIE_SIB_ITER_STACK_DONE TRIE_MAKE_NAME(TRIE_NAME, sib_iter_stack_done)
#define TRIE_SIB_ITER_STACK_NAME TRIE_MAKE_NAME(TRIE_NAME, sib_iter)

#define TRIE_SIB_ITERATOR_TYPE   TRIE_MAKE_NAME(TRIE_NAME, sib_iterator_t)
#define TRIE_SIB_ITERATOR_INIT   TRIE_MAKE_NAME(TRIE_NAME, sib_iterator_init)
#define TRIE_SIB_ITERATOR_DONE   TRIE_MAKE_NAME(TRIE_NAME, sib_iterator_done)
#define TRIE_SIB_ITERATOR_AT_END TRIE_MAKE_NAME(TRIE_NAME, sib_iterator_at_end)
#define TRIE_SIB_ITERATOR_DEREF  TRIE_MAKE_NAME(TRIE_NAME, sib_iterator_deref)
#define TRIE_SIB_ITERATOR_INC    TRIE_MAKE_NAME(TRIE_NAME, sib_iterator_inc)

#define TRIE_LVL_NODE_STACK_TYPE  TRIE_MAKE_NAME(TRIE_NAME, lvl_node_stack_t)
#define TRIE_LVL_NODE_STACK_INIT  TRIE_MAKE_NAME(TRIE_NAME, lvl_node_stack_init)
#define TRIE_LVL_NODE_STACK_DONE  TRIE_MAKE_NAME(TRIE_NAME, lvl_node_stack_done)
#define TRIE_LVL_NODE_STACK_PUSH  TRIE_MAKE_NAME(TRIE_NAME, lvl_node_stack_push)
#define TRIE_LVL_NODE_STACK_POP   TRIE_MAKE_NAME(TRIE_NAME, lvl_node_stack_pop)
#define TRIE_LVL_NODE_STACK_SIZE  TRIE_MAKE_NAME(TRIE_NAME, lvl_node_stack_size)
#define TRIE_LVL_NODE_STACK_CLEAR TRIE_MAKE_NAME(TRIE_NAME, lvl_node_stack_clear)
#define TRIE_LVL_NODE_STACK_BEG_REF TRIE_MAKE_NAME(TRIE_NAME, lvl_node_stack_beg_ref)
#define TRIE_LVL_NODE_STACK_NAME  TRIE_MAKE_NAME(TRIE_NAME, lvl_node)

#define TRIE_LVL_ITER_STACK_TYPE TRIE_MAKE_NAME(TRIE_NAME, lvl_iter_stack_t)
#define TRIE_LVL_ITER_STACK_ELEM_TYPE TRIE_MAKE_NAME(TRIE_NAME, lvl_iter_stack_elem_t)
#define TRIE_LVL_ITER_STACK_INIT TRIE_MAKE_NAME(TRIE_NAME, lvl_iter_stack_init)
#define TRIE_LVL_ITER_STACK_DONE TRIE_MAKE_NAME(TRIE_NAME, lvl_iter_stack_done)
#define TRIE_LVL_ITER_STACK_NAME TRIE_MAKE_NAME(TRIE_NAME, lvl_iter)

#define TRIE_LVL_ITERATOR_TYPE   TRIE_MAKE_NAME(TRIE_NAME, lvl_iterator_t)
#define TRIE_LVL_ITERATOR_INIT   TRIE_MAKE_NAME(TRIE_NAME, lvl_iterator_init)
#define TRIE_LVL_ITERATOR_DONE   TRIE_MAKE_NAME(TRIE_NAME, lvl_iterator_done)
#define TRIE_LVL_ITERATOR_AT_END TRIE_MAKE_NAME(TRIE_NAME, lvl_iterator_at_end)
#define TRIE_LVL_ITERATOR_DEREF  TRIE_MAKE_NAME(TRIE_NAME, lvl_iterator_deref)
#define TRIE_LVL_ITERATOR_INC    TRIE_MAKE_NAME(TRIE_NAME, lvl_iterator_inc)

// stev: the trie implementation below is due to the work of
// Bentley and Sedgewick; see the web page and the papers
// listed below:
//
// Ternary Search Trees
// http://www.cs.princeton.edu/~rs/strings/
//
// Bentley and Sedgewick: Ternary Search Trees
// Dr. Dobbs Journal April, 1998
// http://www.drdobbs.com/database/ternary-search-trees/184410528
//
// Bentley and Sedgewick: Fast Algorithms for Sorting
//   and Searching Strings
// SODA '97: Proceedings of the eighth annual ACM-SIAM
//   symposium on Discrete algorithms,
//   pp. 360-369, 1997
// http://www.cs.princeton.edu/~rs/strings/paper.pdf

#ifndef TRIE_NEED_IMPL_ONLY

struct TRIE_NODE_TYPE
{
    TRIE_SYM_TYPE sym;

    const struct TRIE_NODE_TYPE* lo;
    const struct TRIE_NODE_TYPE* hi;

    // stev: note that 'sym' is the discriminator
    // attached to the union below by the conditions:
    //   !TRIE_SYM_IS_NULL(sym) => use 'eq'
    //    TRIE_SYM_IS_NULL(sym) => use 'val'
    union {
        const struct TRIE_NODE_TYPE* eq;
        TRIE_VAL_TYPE                val;
    } cell;
};

typedef struct TRIE_NODE_TYPE* (*TRIE_ALLOC_FUNC_TYPE)(
    TRIE_ALLOC_OBJ_TYPE*);

struct TRIE_TYPE
{
    TRIE_ALLOC_FUNC_TYPE         alloc_func;
    TRIE_ALLOC_OBJ_TYPE*         alloc_obj;
    const struct TRIE_NODE_TYPE* root;
};

#endif // TRIE_NEED_IMPL_ONLY

#ifndef TRIE_NEED_STRUCT_ONLY

#ifndef TRIE_SYM_CMP_TYPE
#define TRIE_SYM_CMP_TYPE
enum trie_sym_cmp_t
{
    trie_sym_cmp_eq,
    trie_sym_cmp_lt,
    trie_sym_cmp_gt,
};
#endif

#ifdef TRIE_SYM_CMP_NAME
static enum trie_sym_cmp_t TRIE_SYM_CMP_NAME(
    TRIE_SYM_TYPE, TRIE_SYM_TYPE);
#endif

void TRIE_INIT(
    struct TRIE_TYPE* trie,
    TRIE_ALLOC_FUNC_TYPE alloc_func,
    TRIE_ALLOC_OBJ_TYPE* alloc_obj)
{
    memset(trie, 0, sizeof(struct TRIE_TYPE));

    trie->alloc_func = alloc_func;
    trie->alloc_obj = alloc_obj;
}

// stev: we presume that 'alloc_func' returns
// a pointer to a zeroed-out 'TRIE_NODE_TYPE'

#define TRIE_NEW_NODE(s)                       \
    ({                                         \
        struct TRIE_NODE_TYPE* __r =           \
            trie->alloc_func(trie->alloc_obj); \
        __r->sym = s;                          \
        __r;                                   \
    })

#ifdef TRIE_NEED_LOOKUP_SYM_ANY

static bool TRIE_LOOKUP_SYM_ANY_NODE(
    struct TRIE_NODE_TYPE* node, size_t count,
    const struct TRIE_NODE_TYPE** result)
{
    bool n;

    n = TRIE_SYM_IS_NULL(node->sym);

    if (n && count == 0) {
        *result = node;
        return true;
    }

    if (node->lo != NULL &&
        TRIE_LOOKUP_SYM_ANY_NODE(
            node->lo, count, result))
        return true;

    if (!n && node->cell.eq != NULL &&
        TRIE_LOOKUP_SYM_ANY_NODE(
            node->cell.eq, count - 1, result))
        return true;

    if (node->hi != NULL &&
        TRIE_LOOKUP_SYM_ANY_NODE(
            node->hi, count, result))
        return true;

    *result = NULL;
    return false;
}

static bool TRIE_LOOKUP_SYM_ANY(
    const struct TRIE_TYPE* trie, size_t count,
    const struct TRIE_NODE_TYPE** result)
{
    if (trie->root == NULL) {
        *result = NULL;
        return false;
    }

    return TRIE_LOOKUP_SYM_ANY_NODE(
        trie->root, count, result);
}

#endif // TRIE_NEED_LOOKUP_SYM_ANY

#ifdef TRIE_NEED_LOOKUP_SYM_STAR

static bool TRIE_LOOKUP_SYM_STAR(
    const struct TRIE_TYPE* trie, TRIE_SYM_TYPE sym,
    const struct TRIE_NODE_TYPE** result)
{
    struct TRIE_NODE_TYPE* node;
    enum trie_sym_cmp_t cmp;

    node = trie->root;
    while (node) {
        if (TRIE_SYM_IS_NULL(node->sym)) {
            *result = node;
            return true;
        }
        cmp = TRIE_SYM_CMP(sym, node->sym); 
        if (cmp == trie_sym_cmp_eq)
            node = node->cell.eq;
        else
        if (cmp == trie_sym_cmp_lt)
            node = node->lo;
        else
        if (cmp == trie_sym_cmp_gt)
            node = node->hi;
        else
            UNEXPECT_VAR("%d", cmp);
    }

    *result = NULL;
    return false;
}

#endif // TRIE_NEED_LOOKUP_SYM_STAR

#ifdef TRIE_NEED_MATCH_SYM_ANY

static bool TRIE_MATCH_SYM_ANY_NODE(
    const struct TRIE_NODE_TYPE* node,
    const struct TRIE_NODE_TYPE** result)
{
    if (TRIE_SYM_IS_NULL(node->sym)) {
        *result = node;
        return true;
    }

    if (node->lo != NULL &&
        TRIE_MATCH_SYM_ANY_NODE(
            node->lo, result))
        return true;

    if (node->cell.eq != NULL &&
        TRIE_MATCH_SYM_ANY_NODE(
            node->cell.eq, result))
        return true;

    if (node->hi != NULL &&
        TRIE_MATCH_SYM_ANY_NODE(
            node->hi, result))
        return true;

    *result = NULL;
    return false;
}

static bool TRIE_MATCH_SYM_ANY(
    const struct TRIE_TYPE* trie,
    const struct TRIE_NODE_TYPE** result)
{
    if (trie->root == NULL) {
        *result = NULL;
        return false;
    }

    return TRIE_MATCH_SYM_ANY_NODE(
        trie->root, result);
}

#endif // TRIE_NEED_MATCH_SYM_ANY

#ifdef TRIE_NEED_MATCH_SYM_STAR

static bool TRIE_MATCH_SYM_STAR_NODE(
    const struct TRIE_NODE_TYPE* node, TRIE_SYM_TYPE sym,
    const struct TRIE_NODE_TYPE** result)
{
    bool a;
    enum trie_sym_cmp_t c;

    if (TRIE_SYM_IS_NULL(node->sym)) {
        *result = node;
        return true;
    }

    a = TRIE_SYM_IS_ANY(node->sym);
    c = TRIE_SYM_CMP(sym, node->sym);

    if (node->lo != NULL &&
        (a || c == trie_sym_cmp_lt) &&
        TRIE_MATCH_SYM_STAR_NODE(
            node->lo, sym, result))
        return true;

    if (node->cell.eq != NULL &&
        (a || c == trie_sym_cmp_eq) &&
        TRIE_MATCH_SYM_STAR_NODE(
            node->cell.eq, sym, result))
        return true;

    if (node->hi != NULL &&
        (a || c == trie_sym_cmp_gt) &&
        TRIE_MATCH_SYM_STAR_NODE(
            node->hi, sym, result))
        return true;

    *result = NULL;
    return false;
}

static bool TRIE_MATCH_SYM_STAR(
    const struct TRIE_TYPE* trie, TRIE_SYM_TYPE sym,
    const struct TRIE_NODE_TYPE** result)
{
    if (trie->root == NULL) {
        *result = NULL;
        return false;
    }

    return TRIE_MATCH_SYM_STAR_NODE(
        trie->root, sym, result);
}

#endif // TRIE_NEED_MATCH_SYM_STAR

#if defined(TRIE_NEED_MATCH_SYM) || \
    defined(TRIE_NEED_LOOKUP_FIRST_NODE) || \
    defined(TRIE_NEED_NODE_GET_SIB_COUNT_IF)

typedef bool (*TRIE_NODE_COND_FUNC_TYPE)(
    const void* obj, const struct TRIE_NODE_TYPE*);

#endif // TRIE_NEED_LOOKUP_FIRST_NODE ||
       // TRIE_NEED_NODE_GET_SIB_COUNT_IF

#ifdef TRIE_NEED_MATCH_SYM

static bool TRIE_MATCH_SYM_NODE(
    const struct TRIE_NODE_TYPE* node,
    TRIE_NODE_COND_FUNC_TYPE cond, const void* obj,
    const struct TRIE_NODE_TYPE** result)
{
    if (TRIE_SYM_IS_NULL(node->sym)) {
        *result = node;
        return true;
    }

    if (!cond(obj, node)) {
        *result = NULL;
        return false;
    }

    if (node->lo != NULL &&
        TRIE_MATCH_SYM_NODE(
            node->lo, cond, obj, result))
        return true;

    if (node->cell.eq != NULL &&
        TRIE_MATCH_SYM_NODE(
            node->cell.eq, cond, obj, result))
        return true;

    if (node->hi != NULL &&
        TRIE_MATCH_SYM_NODE(
            node->hi, cond, obj, result))
        return true;

    *result = NULL;
    return false;
}

static bool TRIE_MATCH_SYM(
    const struct TRIE_TYPE* trie,
    TRIE_NODE_COND_FUNC_TYPE cond, const void* obj,
    const struct TRIE_NODE_TYPE** result)
{
    if (trie->root == NULL) {
        *result = NULL;
        return false;
    }

    return TRIE_MATCH_SYM_NODE(
        trie->root, cond, obj, result);
}

#endif // TRIE_NEED_MATCH_SYM

#ifdef TRIE_NEED_LOOKUP_FIRST_NODE

static bool TRIE_LOOKUP_FIRST_NODE(
    const struct TRIE_NODE_TYPE* node,
    TRIE_NODE_COND_FUNC_TYPE cond, const void* obj,
    const struct TRIE_NODE_TYPE** result)
{
    ASSERT(node != NULL);

    if (cond(obj, node)) {
        if (result != NULL)
            *result = node;
        return true;
    }

    if (node->lo != NULL &&
        TRIE_LOOKUP_FIRST_NODE(
            node->lo, cond, obj, result))
        return true;

    if (node->hi != NULL &&
        TRIE_LOOKUP_FIRST_NODE(
            node->hi, cond, obj, result))
        return true;

    if (result != NULL)
        *result = NULL;
    return false;
}

#endif // TRIE_NEED_LOOKUP_FIRST_NODE

#if defined(TRIE_NEED_LOOKUP_SYM_NODE) || \
    defined(TRIE_NEED_NODE_HAS_NULL_SIB)

static bool TRIE_LOOKUP_SYM_NODE(
    const struct TRIE_NODE_TYPE* node, TRIE_SYM_TYPE sym,
    const struct TRIE_NODE_TYPE** result)
{
    enum trie_sym_cmp_t cmp;

    while (node) {
        cmp = TRIE_SYM_CMP(sym, node->sym); 
        if (cmp == trie_sym_cmp_eq) {
            if (result != NULL)
                *result = node;
            return true;
        }
        else
        if (cmp == trie_sym_cmp_lt)
            node = node->lo;
        else
        if (cmp == trie_sym_cmp_gt)
            node = node->hi;
        else
            UNEXPECT_VAR("%d", cmp);
    }

    if (result != NULL)
        *result = NULL;
    return false;
}

#endif // TRIE_NEED_LOOKUP_SYM_NODE ||
       // TRIE_NEED_NODE_HAS_NULL_SIB

#ifdef TRIE_NEED_LOOKUP_SYM

static bool TRIE_LOOKUP_SYM(
    const struct TRIE_TYPE* trie, TRIE_SYM_TYPE sym,
    const struct TRIE_NODE_TYPE** result)
{
    const struct TRIE_NODE_TYPE* node;
    enum trie_sym_cmp_t cmp;

    node = trie->root;
    while (node) {
        cmp = TRIE_SYM_CMP(sym, node->sym); 
        if (cmp == trie_sym_cmp_eq) {
            if (TRIE_SYM_IS_NULL(node->sym)) {
                *result = node;
                return true;
            }
            sym = TRIE_NULL_SYM();
            node = node->cell.eq;
        }
        else
        if (cmp == trie_sym_cmp_lt)
            node = node->lo;
        else
        if (cmp == trie_sym_cmp_gt)
            node = node->hi;
        else
            UNEXPECT_VAR("%d", cmp);
    }

    *result = NULL;
    return false;
}

#endif // TRIE_NEED_LOOKUP_SYM

#if defined(TRIE_NEED_INSERT_SYM) || \
    defined(TRIE_NEED_INSERT_KEY)
#define TRIE_CONST_CAST(p) \
    CONST_CAST(p, const struct TRIE_NODE_TYPE*)
#endif

#ifdef TRIE_NEED_INSERT_SYM

static bool TRIE_INSERT_SYM(
    struct TRIE_TYPE* trie, TRIE_SYM_TYPE sym,
    const struct TRIE_NODE_TYPE** result)
{
    const struct TRIE_NODE_TYPE* node;
    const struct TRIE_NODE_TYPE* const* ptr;
    enum trie_sym_cmp_t cmp;

    ptr = &trie->root;
    while ((node = *ptr)) {
        cmp = TRIE_SYM_CMP(sym, node->sym); 
        if (cmp == trie_sym_cmp_eq) {
            if (TRIE_SYM_IS_NULL(node->sym)) {
                *result = node;
                return false;
            }
            sym = TRIE_NULL_SYM();
            ptr = &node->cell.eq;
        }
        else
        if (cmp == trie_sym_cmp_lt)
            ptr = &node->lo;
        else
        if (cmp == trie_sym_cmp_gt)
            ptr = &node->hi;
        else
            UNEXPECT_VAR("%d", cmp);
    }

    node = *TRIE_CONST_CAST(ptr) =
        TRIE_NEW_NODE(sym);

    if (TRIE_SYM_IS_NULL(sym))
        goto done;

    ptr = &node->cell.eq;

    node = *TRIE_CONST_CAST(ptr) =
        TRIE_NEW_NODE(TRIE_NULL_SYM());

done:
    *result = node;
    return true;
}

#endif // TRIE_NEED_INSERT_SYM

static bool TRIE_INSERT_KEY(
    struct TRIE_TYPE* trie, TRIE_KEY_TYPE key,
    const struct TRIE_NODE_TYPE** result)
{
    const struct TRIE_NODE_TYPE* node;
    const struct TRIE_NODE_TYPE* const* ptr;
    enum trie_sym_cmp_t cmp;

    ptr = &trie->root;
    while ((node = *ptr)) {
        cmp = TRIE_SYM_CMP(
                TRIE_KEY_DEREF(key),
                node->sym); 
        if (cmp == trie_sym_cmp_eq) {
            if (TRIE_SYM_IS_NULL(node->sym)) {
                *result = node;
                return false;
            }
            TRIE_KEY_INC(key);
            ptr = &node->cell.eq;
        }
        else
        if (cmp == trie_sym_cmp_lt)
            ptr = &node->lo;
        else
        if (cmp == trie_sym_cmp_gt)
            ptr = &node->hi;
        else
            UNEXPECT_VAR("%d", cmp);
    }

    while (true) {
        node = *TRIE_CONST_CAST(ptr) =
            TRIE_NEW_NODE(TRIE_KEY_DEREF(key));
        if (TRIE_SYM_IS_NULL(node->sym))
            break;
        TRIE_KEY_INC(key);
        ptr = &node->cell.eq;
    }

    *result = node;
    return true;
}

#ifdef TRIE_NEED_NODE_GET_SHORTEST_KEY_LEN

static size_t TRIE_NODE_GET_SHORTEST_KEY_LEN(
    const struct TRIE_NODE_TYPE* node)
{
    size_t r = 0, c = 0, l;
    bool n;

    ASSERT(node != NULL);

    n = TRIE_SYM_IS_NULL(node->sym);

    if (node->lo != NULL) {
        l = TRIE_NODE_GET_SHORTEST_KEY_LEN(
                node->lo);
        if (c ++ == 0 ||
            r > l)
            r = l;
    }

    if (!n && node->cell.eq != NULL) {
        l = TRIE_NODE_GET_SHORTEST_KEY_LEN(
                node->cell.eq) + 1;
        if (c ++ == 0 ||
            r > l)
            r = l;
    }

    if (node->hi != NULL) {
        l = TRIE_NODE_GET_SHORTEST_KEY_LEN(
                node->hi);
        if (c ++ == 0 ||
            r > l)
            r = l;
    }

    return r;
}

#endif // TRIE_NEED_NODE_GET_SHORTEST_KEY_LEN

#ifdef TRIE_NEED_NODE_GET_LONGEST_KEY_LEN

static size_t TRIE_NODE_GET_LONGEST_KEY_LEN(
    const struct TRIE_NODE_TYPE* node)
{
    size_t r = 0, l;
    bool n;

    ASSERT(node != NULL);

    n = TRIE_SYM_IS_NULL(node->sym);

    if (node->lo != NULL) {
        l = TRIE_NODE_GET_LONGEST_KEY_LEN(
                node->lo);
        if (r < l)
            r = l;
    }

    if (!n && node->cell.eq != NULL) {
        l = TRIE_NODE_GET_LONGEST_KEY_LEN(
                node->cell.eq) + 1;
        if (r < l)
            r = l;
    }

    if (node->hi != NULL) {
        l = TRIE_NODE_GET_LONGEST_KEY_LEN(
                node->hi);
        if (r < l)
            r = l;
    }

    return r;
}

#endif // TRIE_NEED_NODE_GET_LONGEST_KEY_LEN

#if defined(TRIE_NEED_NODE_GET_SIB_ITER_MAX_DEPTH) || \
    defined(TRIE_NEED_SIB_ITERATOR)

static size_t TRIE_NODE_GET_SIB_ITER_MAX_DEPTH(
    const struct TRIE_NODE_TYPE* node)
{
    size_t r = 0, d;

    ASSERT(node != NULL);

    if (node->lo != NULL) {
        d = TRIE_NODE_GET_SIB_ITER_MAX_DEPTH(
                node->lo);
        if (r < d)
            r = d;
    }

    if (node->hi != NULL) {
        d = TRIE_NODE_GET_SIB_ITER_MAX_DEPTH(
                node->hi);
        if (r < d)
            r = d;
    }

    return r + 1;
}

#endif // TRIE_NEED_NODE_GET_SIB_ITER_MAX_DEPTH ||
       // TRIE_NEED_SIB_ITERATOR

#ifdef TRIE_NEED_NODE_GET_SIB_LVL_COUNT

static size_t TRIE_NODE_GET_SIB_LVL_COUNT(
    const struct TRIE_NODE_TYPE* node)
{
    size_t r = 0;
    bool n;

    ASSERT(node != NULL);

    n = TRIE_SYM_IS_NULL(node->sym);

    if (node->lo != NULL)
        r += TRIE_NODE_GET_SIB_LVL_COUNT(
                node->lo);

    if (!n && node->cell.eq != NULL)
        r ++;

    if (node->hi != NULL)
        r += TRIE_NODE_GET_SIB_LVL_COUNT(
                node->hi);

    return r;
}

#endif // TRIE_NEED_GET_SIB_LVL_COUNT

#if defined(TRIE_NEED_NODE_GET_KEY_COUNT) || \
    defined(TRIE_NEED_NODE_GET_LVL_ITER_MAX_DEPTH) || \
    defined(TRIE_NEED_LVL_ITERATOR)

static size_t TRIE_NODE_GET_KEY_COUNT(
    const struct TRIE_NODE_TYPE* node)
{
    size_t r = 0;
    bool n;

    ASSERT(node != NULL);

    if ((n = TRIE_SYM_IS_NULL(node->sym)))
        r ++;

    if (node->lo != NULL)
        r += TRIE_NODE_GET_KEY_COUNT(
                node->lo);

    if (!n && node->cell.eq != NULL)
        r += TRIE_NODE_GET_KEY_COUNT(
                node->cell.eq);

    if (node->hi != NULL)
        r += TRIE_NODE_GET_KEY_COUNT(
                node->hi);

    return r;
}

#endif // TRIE_NEED_NODE_GET_KEY_COUNT ||
       // TRIE_NEED_NODE_GET_LVL_ITER_MAX_DEPTH ||
       // TRIE_NEED_LVL_ITERATOR

#if defined(TRIE_NEED_NODE_GET_LVL_ITER_MAX_DEPTH) || \
    defined(TRIE_NEED_LVL_ITERATOR)

static size_t TRIE_NODE_GET_LVL_ITER_MAX_DEPTH(
    const struct TRIE_NODE_TYPE* node)
{
    return TRIE_NODE_GET_KEY_COUNT(node);
}

#endif // TRIE_NEED_NODE_GET_LVL_ITER_MAX_DEPTH ||
       // TRIE_NEED_LVL_ITERATOR

#if defined(TRIE_NEED_NODE_GET_ITER_MAX_DEPTH) || \
    defined(TRIE_NEED_ITERATOR)

static size_t TRIE_NODE_GET_ITER_MAX_DEPTH(
    const struct TRIE_NODE_TYPE* node)
{
    size_t r = 1, d;
    bool n;

    ASSERT(node != NULL);

    n = TRIE_SYM_IS_NULL(node->sym);

    if (node->lo != NULL) {
        d = TRIE_NODE_GET_ITER_MAX_DEPTH(
                node->lo) + 1;
        if (r < d)
            r = d;
    }

    if (!n && node->cell.eq != NULL) {
        d = TRIE_NODE_GET_ITER_MAX_DEPTH(
                node->cell.eq) + 1;
        if (r < d)
            r = d;
    }

    if (node->hi != NULL) {
        d = TRIE_NODE_GET_ITER_MAX_DEPTH(
                node->hi) + 1;
        if (r < d)
            r = d;
    }

    return r;
}

#endif // TRIE_NEED_NODE_GET_ITER_MAX_DEPTH ||
       // TRIE_NEED_ITERATOR

#ifdef TRIE_NEED_PRINT

static void TRIE_PRINT_NODE(
    const struct TRIE_NODE_TYPE* node, FILE* file)
{
    bool n;

    if (node == NULL) {
        fputs("null", file);
        return;
    }

    n = TRIE_SYM_IS_NULL(node->sym);

    fputc('{', file);

    if (n) {
        fputs("\"val\":", file);
        TRIE_VAL_PRINT(node->cell.val, file);
    }
    else {
        fputs("\"sym\":", file);
        TRIE_SYM_PRINT(node->sym, file);
    }

    fputs(",\"lo\":", file);
    TRIE_PRINT_NODE(node->lo, file);

    if (!n) {
        fputs(",\"eq\":", file);
        TRIE_PRINT_NODE(node->cell.eq, file);
    }

    fputs(",\"hi\":", file);
    TRIE_PRINT_NODE(node->hi, file);

    fputc('}', file);
}

static void TRIE_PRINT(
    const struct TRIE_TYPE* trie, FILE* file)
{
    TRIE_PRINT_NODE(trie ? trie->root : NULL, file);
}

#endif // TRIE_NEED_PRINT

#ifdef TRIE_NEED_NODE_GET_LEAF

static const struct TRIE_NODE_TYPE*
    TRIE_NODE_GET_LEAF(
        const struct TRIE_NODE_TYPE* node)
{
    ASSERT(node != NULL);

    do {
        if (TRIE_SYM_IS_NULL(node->sym))
            return node;
    } while ((node = node->cell.eq));

    return NULL;
}

#endif // TRIE_NEED_NODE_GET_LEAF

#ifdef TRIE_NEED_GET_NODE_COUNT

static size_t TRIE_NODE_GET_NODE_COUNT(
    const struct TRIE_NODE_TYPE* node)
{
    size_t r = 1;
    bool n;

    ASSERT(node != NULL);

    n = TRIE_SYM_IS_NULL(node->sym);

    if (node->lo != NULL)
        r += TRIE_NODE_GET_NODE_COUNT(node->lo);

    if (!n && node->cell.eq != NULL)
        r += TRIE_NODE_GET_NODE_COUNT(node->cell.eq);

    if (node->hi != NULL)
        r += TRIE_NODE_GET_NODE_COUNT(node->hi);

    return r;
}

static size_t TRIE_GET_NODE_COUNT(
    const struct TRIE_TYPE* trie)
{
    return trie->root != NULL
        ? TRIE_NODE_GET_NODE_COUNT(trie->root)
        : 0;
}

#endif // TRIE_NEED_GET_NODE_COUNT

#ifdef TRIE_NEED_NODE_GET_SIB_COUNT

static size_t TRIE_NODE_GET_SIB_COUNT(
    const struct TRIE_NODE_TYPE* node)
{
    size_t r = 1;

    ASSERT(node != NULL);

    if (node->lo != NULL)
        r += TRIE_NODE_GET_SIB_COUNT(node->lo);

    if (node->hi != NULL)
        r += TRIE_NODE_GET_SIB_COUNT(node->hi);

    return r;
}

#endif // TRIE_NEED_NODE_GET_SIB_COUNT

#ifdef TRIE_NEED_NODE_GET_SIB_COUNT_IF

static size_t TRIE_NODE_GET_SIB_COUNT_IF(
    const struct TRIE_NODE_TYPE* node,
    TRIE_NODE_COND_FUNC_TYPE cond,
    const void* obj)
{
    size_t r;

    ASSERT(node != NULL);

    r = cond(obj, node);

    if (node->lo != NULL)
        r += TRIE_NODE_GET_SIB_COUNT_IF(
            node->lo, cond, obj);

    if (node->hi != NULL)
        r += TRIE_NODE_GET_SIB_COUNT_IF(
            node->hi, cond, obj);

    return r;
}

#endif // TRIE_NEED_NODE_GET_SIB_COUNT_IF

#ifdef TRIE_NEED_NODE_HAS_NULL_SIB

static bool TRIE_NODE_HAS_NULL_SIB(
    const struct TRIE_NODE_TYPE* node)
{
    return TRIE_LOOKUP_SYM_NODE(
        node, TRIE_NULL_SYM(), NULL);
}

#endif // TRIE_NEED_HAS_NULL_SIB

#ifdef TRIE_NEED_NODE_HAS_NON_NULL_SIB

static bool TRIE_NODE_HAS_NON_NULL_SIB(
    const struct TRIE_NODE_TYPE* node)
{
    ASSERT(node != NULL);

    if (!TRIE_SYM_IS_NULL(node->sym))
        return true;

    if (node->lo != NULL &&
        TRIE_NODE_HAS_NON_NULL_SIB(node->lo))
        return true;

    if (node->hi != NULL &&
        TRIE_NODE_HAS_NON_NULL_SIB(node->hi))
        return true;

    return false;
}

#endif // TRIE_NEED_HAS_NON_NULL_SIB

#ifdef TRIE_NEED_VALIDATE

static bool TRIE_NODE_VALIDATE(
    const struct TRIE_NODE_TYPE* node,
    TRIE_PTR_SPACE_TYPE* space,
    const void** result)
{
    bool n;

    ASSERT(node != NULL);

    TRIE_VALIDATE_PTR_SPACE_INSERT(node);

    n = TRIE_SYM_IS_NULL(node->sym);

    if (!TRIE_SYM_VALIDATE(node->sym))
        return false;

    if (node->lo != NULL &&
        !TRIE_NODE_VALIDATE(node->lo, space, result))
        return false;

    if (n &&
        !TRIE_VAL_VALIDATE(node->cell.val))
        return false;

    if (!n && node->cell.eq != NULL &&
        !TRIE_NODE_VALIDATE(node->cell.eq, space, result))
        return false;

    if (node->hi != NULL &&
        !TRIE_NODE_VALIDATE(node->hi, space, result))
        return false;

    *result = NULL;
    return true;
}

static bool TRIE_VALIDATE(
    const struct TRIE_TYPE* trie,
    TRIE_PTR_SPACE_TYPE* space,
    const void** result)
{
    TRIE_VALIDATE_PTR_SPACE_INSERT(trie);

    if (trie->root != NULL &&
        !TRIE_NODE_VALIDATE(trie->root, space, result))
        return false;

    *result = NULL;
    return true;
}

#endif // TRIE_NEED_VALIDATE

#ifdef TRIE_NEED_GEN_DEF

static void TRIE_NODE_GEN_DEF(
    const struct TRIE_NODE_TYPE* node,
    TRIE_PTR_SPACE_TYPE* space,
    FILE* file)
{
    TRIE_PTR_SPACE_NODE_TYPE* t;
    bool n;

    ASSERT(node != NULL);

    n = TRIE_SYM_IS_NULL(node->sym);

    if (node->lo != NULL)
        TRIE_NODE_GEN_DEF(node->lo, space, file);

    if (!n && node->cell.eq != NULL)
        TRIE_NODE_GEN_DEF(node->cell.eq, space, file);

    if (node->hi != NULL)
        TRIE_NODE_GEN_DEF(node->hi, space, file);

    t = TRIE_GEN_DEF_PTR_SPACE_INSERT(node);

    fprintf(file,
        "static const struct " STRINGIFY(TRIE_NODE_TYPE)
            " __%zu = {\n"
        "    .sym = ",
        t->val);

    TRIE_SYM_GEN_DEF(node->sym);

    if (node->lo != NULL) {
        t = TRIE_GEN_DEF_PTR_SPACE_LOOKUP(node->lo);
        fprintf(file,
            ",\n"
            "    .lo = &__%zu",
            t->val);
    }

    if (n) {
        t = TRIE_GEN_DEF_PTR_SPACE_LOOKUP(node->cell.val);
        fprintf(file,
            ",\n"
            "    .VAL = &__%zu",
            t->val);
    }
    if (!n && node->cell.eq != NULL) {
        t = TRIE_GEN_DEF_PTR_SPACE_LOOKUP(node->cell.eq);
        fprintf(file,
            ",\n"
            "    .EQ = &__%zu",
            t->val);
    }

    if (node->hi != NULL) {
        t = TRIE_GEN_DEF_PTR_SPACE_LOOKUP(node->hi);
        fprintf(file,
            ",\n"
            "    .hi = &__%zu",
            t->val);
    }

    fputs(
        "\n"
        "};\n\n",
        file);
}

static void TRIE_GEN_DEF(
    const struct TRIE_TYPE* trie,
    TRIE_PTR_SPACE_TYPE* space,
    FILE* file)
{
    TRIE_PTR_SPACE_NODE_TYPE* t;

    if (trie->root != NULL)
        TRIE_NODE_GEN_DEF(trie->root, space, file);

    t = TRIE_GEN_DEF_PTR_SPACE_INSERT(trie);

    fprintf(file,
        "static const struct " STRINGIFY(TRIE_TYPE)
            " __%zu = {",
        t->val);

    if (trie->root != NULL) {
        t = TRIE_GEN_DEF_PTR_SPACE_LOOKUP(trie->root);
        fprintf(file,
            "\n"
            "    .root = &__%zu\n",
            t->val);
    }

    fputs(
        "};\n\n",
        file);
}

#endif // TRIE_NEED_GEN_DEF

#ifdef TRIE_NEED_ITERATOR

#ifndef TRIE_ITER_STATE
#define TRIE_ITER_STATE
enum trie_iter_state_t
{
    trie_iter_lo_state,
    trie_iter_val_state,
    trie_iter_eq_state,
    trie_iter_hi_state,
    trie_iter_pop_state
};
#endif

struct TRIE_ITER_STACK_ELEM_TYPE
{
    enum trie_iter_state_t       state;
    const struct TRIE_NODE_TYPE* node;
};

#undef  STACK_NAME
#define STACK_NAME TRIE_SYM_STACK_NAME

#undef  STACK_ELEM_TYPE
#define STACK_ELEM_TYPE TRIE_SYM_TYPE

#include "stack-impl.h"

// stev: the implementation in stack-impl.h
// does not allow the use of STACK_* macros
// on two different stack instances at the
// same time; therefore we need to define
// the TRIE_SYM_STACK_* functions below:

static inline void TRIE_SYM_STACK_PUSH(
    struct TRIE_SYM_STACK_TYPE* key,
    TRIE_SYM_TYPE sym)
{
    STACK_PUSH(key, sym);
}

static inline void TRIE_SYM_STACK_POP(
    struct TRIE_SYM_STACK_TYPE* key)
{
    STACK_POP(key);
}

static inline const TRIE_SYM_TYPE*
    TRIE_SYM_STACK_BEG_REF(
        struct TRIE_SYM_STACK_TYPE* key)
{
    return STACK_BEG_REF(key);
}

#undef  STACK_NAME
#define STACK_NAME TRIE_ITER_STACK_NAME

#undef  STACK_ELEM_TYPE
#define STACK_ELEM_TYPE struct TRIE_ITER_STACK_ELEM_TYPE

#define STACK_NEED_FIXED_SIZE //!!! STACK_ENSURE_INPLACE_ENLARGE
#include "stack-impl.h"
#undef  STACK_NEED_FIXED_SIZE //!!! STACK_ENSURE_INPLACE_ENLARGE

struct TRIE_ITERATOR_TYPE
{
    struct TRIE_SYM_STACK_TYPE  key;
    struct TRIE_ITER_STACK_TYPE nodes;
    bool done;
};

#define TRIE_ITER_STACK_PUSH(n)                  \
    do {                                         \
        struct TRIE_ITER_STACK_ELEM_TYPE __e = { \
            .state = trie_iter_lo_state,         \
            .node = n                            \
        };                                       \
        STACK_PUSH(&iter->nodes, __e);           \
    } while (0)

#define TRIE_ITER_STACK_POP()      STACK_POP(&iter->nodes)
#define TRIE_ITER_STACK_IS_EMPTY() STACK_IS_EMPTY(&iter->nodes)
#define TRIE_ITER_STACK_TOP_REF()  STACK_TOP_REF(&iter->nodes)

static bool TRIE_ITERATOR_AT_END(
    struct TRIE_ITERATOR_TYPE* iter)
{
    return iter->done;
}

static const TRIE_SYM_TYPE* TRIE_ITERATOR_DEREF(
    struct TRIE_ITERATOR_TYPE* iter)
{
    return TRIE_SYM_STACK_BEG_REF(&iter->key);
}

static void TRIE_ITERATOR_INC(
    struct TRIE_ITERATOR_TYPE* iter)
{
    struct TRIE_ITER_STACK_ELEM_TYPE* e;
    bool n;

    STATIC(
        trie_iter_lo_state + 1 ==
        trie_iter_val_state &&

        trie_iter_val_state + 1 ==
        trie_iter_eq_state &&

        trie_iter_eq_state + 1 ==
        trie_iter_hi_state &&

        trie_iter_hi_state + 1 ==
        trie_iter_pop_state);

test_done:
    if (TRIE_ITER_STACK_IS_EMPTY()) {
        iter->done = true;
        return;
    }

next_node:
    e = TRIE_ITER_STACK_TOP_REF();
    n = TRIE_SYM_IS_NULL(e->node->sym);

    switch (e->state) {

    case trie_iter_lo_state:
        e->state ++;
        if (e->node->lo != NULL) {
            TRIE_ITER_STACK_PUSH(
                e->node->lo);
            goto next_node;
        }

    case trie_iter_val_state:
        e->state ++;
        if (n || e->node->cell.eq != NULL) {
            TRIE_SYM_STACK_PUSH(
                &iter->key,
                e->node->sym);
        }
        if (n) break;

    case trie_iter_eq_state:
        e->state ++;
        if (!n && e->node->cell.eq != NULL) {
            TRIE_ITER_STACK_PUSH(
                e->node->cell.eq);
            goto next_node;
        }

    case trie_iter_hi_state:
        e->state ++;
        if (n || e->node->cell.eq != NULL)
            TRIE_SYM_STACK_POP(
                &iter->key);
        if (e->node->hi != NULL) {
            TRIE_ITER_STACK_PUSH(
                e->node->hi);
            goto next_node;
        }

    case trie_iter_pop_state:
        TRIE_ITER_STACK_POP();
        goto test_done;

    default:
        UNEXPECT_VAR("%d", e->state);
    }
}

static void TRIE_ITERATOR_INIT(
    struct TRIE_ITERATOR_TYPE* iter,
    const struct TRIE_TYPE* trie,
    size_t depth)
{
    memset(iter, 0, sizeof(
        struct TRIE_ITERATOR_TYPE));

    if (depth == 0)
        depth = trie->root
            ? TRIE_NODE_GET_ITER_MAX_DEPTH(
                trie->root)
            : 1;

    TRIE_SYM_STACK_INIT(&iter->key, 16);
    TRIE_ITER_STACK_INIT(&iter->nodes, depth);

    if (trie->root)
        TRIE_ITER_STACK_PUSH(trie->root);

    TRIE_ITERATOR_INC(iter);
}

static void TRIE_ITERATOR_DONE(
    struct TRIE_ITERATOR_TYPE* iter)
{
    TRIE_ITER_STACK_DONE(&iter->nodes);
    TRIE_SYM_STACK_DONE(&iter->key);
}

#endif // TRIE_NEED_ITERATOR

#ifdef TRIE_NEED_SIB_ITERATOR

#ifndef TRIE_SIB_ITER_STATE
#define TRIE_SIB_ITER_STATE
enum trie_sib_iter_state_t
{
    trie_sib_iter_lo_state,
    trie_sib_iter_sym_state,
    trie_sib_iter_hi_state,
    trie_sib_iter_pop_state
};
#endif

struct TRIE_SIB_ITER_STACK_ELEM_TYPE
{
    enum trie_sib_iter_state_t   state;
    const struct TRIE_NODE_TYPE* node;
};

#undef  STACK_NAME
#define STACK_NAME TRIE_SIB_ITER_STACK_NAME

#undef  STACK_ELEM_TYPE
#define STACK_ELEM_TYPE struct TRIE_SIB_ITER_STACK_ELEM_TYPE

#define STACK_NEED_FIXED_SIZE //!!! STACK_ENSURE_INPLACE_ENLARGE
#include "stack-impl.h"
#undef  STACK_NEED_FIXED_SIZE //!!! STACK_ENSURE_INPLACE_ENLARGE

struct TRIE_SIB_ITERATOR_TYPE
{
    struct TRIE_SIB_ITER_STACK_TYPE nodes;
    bool done;
};

#define TRIE_SIB_ITER_STACK_PUSH(n)                  \
    do {                                             \
        struct TRIE_SIB_ITER_STACK_ELEM_TYPE __e = { \
            .state = trie_sib_iter_lo_state,         \
            .node = n                                \
        };                                           \
        STACK_PUSH(&iter->nodes, __e);               \
    } while (0)

#define TRIE_SIB_ITER_STACK_POP()      STACK_POP(&iter->nodes)
#define TRIE_SIB_ITER_STACK_IS_EMPTY() STACK_IS_EMPTY(&iter->nodes)
#define TRIE_SIB_ITER_STACK_TOP_REF()  STACK_TOP_REF(&iter->nodes)

static bool TRIE_SIB_ITERATOR_AT_END(
    struct TRIE_SIB_ITERATOR_TYPE* iter)
{
    return iter->done;
}

static const struct TRIE_NODE_TYPE*
    TRIE_SIB_ITERATOR_DEREF(
        struct TRIE_SIB_ITERATOR_TYPE* iter)
{
    struct TRIE_SIB_ITER_STACK_ELEM_TYPE* e;

    e = TRIE_SIB_ITER_STACK_TOP_REF();

    return e->node;
}

static void TRIE_SIB_ITERATOR_INC(
    struct TRIE_SIB_ITERATOR_TYPE* iter)
{
    struct TRIE_SIB_ITER_STACK_ELEM_TYPE* e;

    STATIC(
        trie_sib_iter_lo_state + 1 ==
        trie_sib_iter_sym_state &&

        trie_sib_iter_sym_state + 1 ==
        trie_sib_iter_hi_state &&

        trie_sib_iter_hi_state + 1 ==
        trie_sib_iter_pop_state);

test_done:
    if (TRIE_SIB_ITER_STACK_IS_EMPTY()) {
        iter->done = true;
        return;
    }

next_node:
    e = TRIE_SIB_ITER_STACK_TOP_REF();

    switch (e->state) {

    case trie_sib_iter_lo_state:
        e->state ++;
        if (e->node->lo != NULL) {
            TRIE_SIB_ITER_STACK_PUSH(
                e->node->lo);
            goto next_node;
        }

    case trie_sib_iter_sym_state:
        e->state ++;
        break;

    case trie_sib_iter_hi_state:
        e->state ++;
        if (e->node->hi != NULL) {
            TRIE_SIB_ITER_STACK_PUSH(
                e->node->hi);
            goto next_node;
        }

    case trie_sib_iter_pop_state:
        TRIE_SIB_ITER_STACK_POP();
        goto test_done;

    default:
        UNEXPECT_VAR("%d", e->state);
    }
}

static void TRIE_SIB_ITERATOR_INIT(
    struct TRIE_SIB_ITERATOR_TYPE* iter,
    const struct TRIE_NODE_TYPE* node,
    size_t depth)
{
    memset(iter, 0, sizeof(
        struct TRIE_SIB_ITERATOR_TYPE));

    if (depth == 0)
        depth = TRIE_NODE_GET_SIB_ITER_MAX_DEPTH(node);

    TRIE_SIB_ITER_STACK_INIT(&iter->nodes, depth);

    if (node)
        TRIE_SIB_ITER_STACK_PUSH(node);

    TRIE_SIB_ITERATOR_INC(iter);
}

static void TRIE_SIB_ITERATOR_DONE(
    struct TRIE_SIB_ITERATOR_TYPE* iter)
{
    TRIE_SIB_ITER_STACK_DONE(&iter->nodes);
}

#endif // TRIE_NEED_SIB_ITERATOR

#ifdef TRIE_NEED_LVL_ITERATOR

#ifndef TRIE_LVL_ITER_STATE
#define TRIE_LVL_ITER_STATE
enum trie_lvl_iter_state_t
{
    trie_lvl_iter_lvl_state,
    trie_lvl_iter_lo_state,
    trie_lvl_iter_eq_state,
    trie_lvl_iter_hi_state,
    trie_lvl_iter_pop_state
};
#endif

struct TRIE_LVL_ITER_STACK_ELEM_TYPE
{
    enum trie_lvl_iter_state_t   state;
    const struct TRIE_NODE_TYPE* node;
};

#undef  STACK_NAME
#define STACK_NAME TRIE_LVL_NODE_STACK_NAME

#undef  STACK_ELEM_TYPE
#define STACK_ELEM_TYPE const struct TRIE_NODE_TYPE*

#include "stack-impl.h"

// stev: the implementation in stack-impl.h
// does not allow the use of STACK_* macros
// on two different stack instances at the
// same time; therefore we need to define
// the TRIE_LVL_NODE_STACK_* functions below:

static inline void TRIE_LVL_NODE_STACK_PUSH(
    struct TRIE_LVL_NODE_STACK_TYPE* equals,
    const struct TRIE_NODE_TYPE* node)
{
    STACK_PUSH(equals, node);
}

static inline const struct TRIE_NODE_TYPE*
    TRIE_LVL_NODE_STACK_POP(
        struct TRIE_LVL_NODE_STACK_TYPE* equals)
{
    return STACK_POP(equals);
}

static inline size_t TRIE_LVL_NODE_STACK_SIZE(
    struct TRIE_LVL_NODE_STACK_TYPE* equals)
{
    return STACK_SIZE(equals);
}

static inline void TRIE_LVL_NODE_STACK_CLEAR(
    struct TRIE_LVL_NODE_STACK_TYPE* equals)
{
    STACK_CLEAR(equals);
}

static inline struct TRIE_NODE_TYPE const* const*
    TRIE_LVL_NODE_STACK_BEG_REF(
        struct TRIE_LVL_NODE_STACK_TYPE* equals)
{
    return STACK_BEG_REF(equals);
}

#undef  STACK_NAME
#define STACK_NAME TRIE_LVL_ITER_STACK_NAME

#undef  STACK_ELEM_TYPE
#define STACK_ELEM_TYPE struct TRIE_LVL_ITER_STACK_ELEM_TYPE

#define STACK_NEED_FIXED_SIZE //!!! STACK_ENSURE_INPLACE_ENLARGE
#include "stack-impl.h"
#undef  STACK_NEED_FIXED_SIZE //!!! STACK_ENSURE_INPLACE_ENLARGE

struct TRIE_LVL_ITERATOR_TYPE
{
    struct TRIE_LVL_ITER_STACK_TYPE nodes;
    struct TRIE_LVL_NODE_STACK_TYPE equals;
    bool done;
};

#define TRIE_LVL_ITER_STACK_PUSH(s, n)               \
    do {                                             \
        struct TRIE_LVL_ITER_STACK_ELEM_TYPE __e = { \
            .state = trie_lvl_iter_ ## s ## _state,  \
            .node = n                                \
        };                                           \
        STACK_PUSH(&iter->nodes, __e);               \
    } while (0)

#define TRIE_LVL_ITER_STACK_POP()      STACK_POP(&iter->nodes)
#define TRIE_LVL_ITER_STACK_IS_EMPTY() STACK_IS_EMPTY(&iter->nodes)
#define TRIE_LVL_ITER_STACK_TOP_REF()  STACK_TOP_REF(&iter->nodes)

static bool TRIE_LVL_ITERATOR_AT_END(
    struct TRIE_LVL_ITERATOR_TYPE* iter)
{
    return iter->done;
}

static const struct TRIE_NODE_TYPE*
    TRIE_LVL_ITERATOR_DEREF(
        struct TRIE_LVL_ITERATOR_TYPE* iter)
{
    struct TRIE_LVL_ITER_STACK_ELEM_TYPE* e;

    e = TRIE_LVL_ITER_STACK_TOP_REF();

    return e->node;
}

static void TRIE_LVL_ITERATOR_INC(
    struct TRIE_LVL_ITERATOR_TYPE* iter)
{
    struct TRIE_NODE_TYPE const* const* p;
    struct TRIE_NODE_TYPE const* const* q;
    struct TRIE_LVL_ITER_STACK_ELEM_TYPE* e;
    size_t s;
    bool n;

    STATIC(
        trie_lvl_iter_lvl_state + 1 ==
        trie_lvl_iter_lo_state &&

        trie_lvl_iter_lo_state + 1 ==
        trie_lvl_iter_eq_state &&

        trie_lvl_iter_eq_state + 1 ==
        trie_lvl_iter_hi_state &&

        trie_lvl_iter_hi_state + 1 ==
        trie_lvl_iter_pop_state);

test_done:
    if (!TRIE_LVL_ITER_STACK_IS_EMPTY())
        goto next_node;

    if (!(s = TRIE_LVL_NODE_STACK_SIZE(&iter->equals))) {
        iter->done = true;
        return;
    }

    for (q = TRIE_LVL_NODE_STACK_BEG_REF(&iter->equals),
         p = q + s;
         p > q; )
        TRIE_LVL_ITER_STACK_PUSH(lvl, *-- p);

    TRIE_LVL_NODE_STACK_CLEAR(&iter->equals);

next_node:
    e = TRIE_LVL_ITER_STACK_TOP_REF();
    n = TRIE_SYM_IS_NULL(e->node->sym);

    switch (e->state) {

    case trie_lvl_iter_lvl_state:
        e->state ++;
        break;

    case trie_lvl_iter_lo_state:
        e->state ++;
        if (e->node->lo != NULL) {
            TRIE_LVL_ITER_STACK_PUSH(
                lo, e->node->lo);
            goto next_node;
        }

    case trie_lvl_iter_eq_state:
        e->state ++;
        if (!n && e->node->cell.eq != NULL) {
            TRIE_LVL_NODE_STACK_PUSH(
                &iter->equals,
                e->node->cell.eq);
        }

    case trie_lvl_iter_hi_state:
        e->state ++;
        if (e->node->hi != NULL) {
            TRIE_LVL_ITER_STACK_PUSH(
                lo, e->node->hi);
            goto next_node;
        }

    case trie_lvl_iter_pop_state:
        TRIE_LVL_ITER_STACK_POP();
        goto test_done;

    default:
        UNEXPECT_VAR("%d", e->state);
    }
}

static void TRIE_LVL_ITERATOR_INIT(
    struct TRIE_LVL_ITERATOR_TYPE* iter,
    const struct TRIE_TYPE* trie,
    size_t depth)
{
    memset(iter, 0, sizeof(
        struct TRIE_LVL_ITERATOR_TYPE));

    if (depth == 0)
        depth = trie->root
            ? TRIE_NODE_GET_LVL_ITER_MAX_DEPTH(
                trie->root)
            : 1;

    TRIE_LVL_ITER_STACK_INIT(&iter->nodes, depth);
    TRIE_LVL_NODE_STACK_INIT(&iter->equals, 32);

    if (trie->root)
        TRIE_LVL_ITER_STACK_PUSH(lvl, trie->root);

    TRIE_LVL_ITERATOR_INC(iter);
}

static void TRIE_LVL_ITERATOR_DONE(
    struct TRIE_LVL_ITERATOR_TYPE* iter)
{
    TRIE_LVL_NODE_STACK_DONE(&iter->equals);
    TRIE_LVL_ITER_STACK_DONE(&iter->nodes);
}

#endif // TRIE_NEED_LVL_ITERATOR

#endif // TRIE_NEED_STRUCT_ONLY

#ifndef __TRIE_IMPL_H
#define __TRIE_IMPL_H

#define TRIE_TYPEOF_IS_TRIE_(q, t, p) \
    TYPEOF_IS(p, q struct TRIE_TYPE_(t)*)

#define TRIE_IS_EMPTY_(q, t, p)                \
    (                                          \
        STATIC(TRIE_TYPEOF_IS_TRIE_(q, t, p)), \
        (p)->root == NULL                      \
    )
#define TRIE_IS_EMPTY(t, p) \
    TRIE_IS_EMPTY_(, t, p)
#define TRIE_IS_EMPTY_CONST(t, p) \
    TRIE_IS_EMPTY_(const, t, p)

#define TRIE_TYPEOF_IS_NODE(t, p) \
    TYPEOF_IS(p, const struct TRIE_NODE_TYPE_(t)*)

#define TRIE_NODE_IS_EQ(t, p, n)           \
    (                                      \
        STATIC(TRIE_TYPEOF_IS_NODE(t, p)), \
        !n((p)->sym)                       \
    )
#define TRIE_NODE_IS_VAL(t, p, n)          \
    (                                      \
        STATIC(TRIE_TYPEOF_IS_NODE(t, p)), \
        n((p)->sym)                        \
    )

#define TRIE_NODE_AS_EQ(t, p, n)          \
    ({                                    \
        ASSERT(TRIE_NODE_IS_EQ(t, p, n)); \
        (p)->cell.eq;                     \
    })
#define TRIE_NODE_AS_VAL(t, p, n)          \
    ({                                     \
        ASSERT(TRIE_NODE_IS_VAL(t, p, n)); \
        (p)->cell.val;                     \
    })
#define TRIE_NODE_AS_VAL_REF(t, p, n)      \
    ({                                     \
        ASSERT(TRIE_NODE_IS_VAL(t, p, n)); \
        &(p)->cell.val;                    \
    })

#endif/*__TRIE_IMPL_H*/


