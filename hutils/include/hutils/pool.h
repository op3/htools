/*
 * Copyright (c) 2015-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * Dynamic pool allocator.
 * Constant allocation will walk through objects and pages linearly, freed
 * objects are put in a free list. Freed objects in the last page will be put
 * at the tail of the free list, careful design of the using code can then
 * allow for dynamic freeing of pages.
 */

#ifndef HUTILS_POOL_H
#define HUTILS_POOL_H

#include <hutils/assert.h>
#include <hutils/memory.h>
#include <hutils/queue.h>

#define POOL_HEAD(PoolType, Type, num) \
struct PoolType##Node_ {\
	union {\
		struct	Type type;\
		TAILQ_ENTRY(PoolType##Node_)	freed;\
	} u;\
	struct	PoolType##Page_ *page;\
};\
TAILQ_HEAD(PoolType##List_, PoolType##Node_);\
struct PoolType##Page_ {\
	struct	PoolType##Node_ node_array[num];\
	size_t	ofs;\
	size_t	used_num;\
	TAILQ_ENTRY(PoolType##Page_)	page_list;\
};\
TAILQ_HEAD(PoolType##PageList_, PoolType##Page_);\
struct PoolType {\
	struct	PoolType##PageList_ page_list;\
	struct	PoolType##List_ freed_list;\
}

#define POOL_PROTOTYPE(PoolType, Type) \
static struct Type	*PoolType##_alloc(struct PoolType *) FUNC_RETURNS;\
static void		PoolType##_free(struct PoolType *, struct Type **);\
static void		PoolType##_init(struct PoolType *);\
static void		PoolType##_shutdown(struct PoolType *)

#define POOL_IMPLEMENT(PoolType, Type) \
static struct Type *\
PoolType##_alloc(struct PoolType *a_pool)\
{\
	struct PoolType##Node_ *node;\
	struct PoolType##Page_ *page;\
	if (!TAILQ_EMPTY(&a_pool->freed_list)) {\
		node = TAILQ_FIRST(&a_pool->freed_list);\
		TAILQ_REMOVE(&a_pool->freed_list, node, u.freed);\
		page = node->page;\
	} else {\
		page = TAILQ_LAST(&a_pool->page_list, PoolType##PageList_);\
		if (TAILQ_END(&a_pool->page_list) == page ||\
		    LENGTH(page->node_array) == page->ofs) {\
			page = malloc(sizeof *page);\
			page->ofs = 0;\
			page->used_num = 0;\
			TAILQ_INSERT_TAIL(&a_pool->page_list, page,\
			    page_list);\
		}\
		node = &page->node_array[page->ofs];\
		++page->ofs;\
		node->page = page;\
	}\
	++page->used_num;\
	ASSERT(void *, "p", &node->u.type, ==, node);\
	return &node->u.type;\
}\
FUNC_UNUSED static void \
PoolType##_free(struct PoolType *a_pool, struct Type **a_type)\
{\
	struct PoolType##Node_ *node;\
	struct PoolType##Page_ *page;\
	node = (void *)*a_type;\
	page = node->page;\
	ASSERT(void *, "p", (void *)&page->node_array, <=, (void *)node);\
	ASSERT(void *, "p", (void *)node, <=, (void *)\
	    &page->node_array[LENGTH(page->node_array) - 1]);\
	if (TAILQ_END(&a_pool->page_list) == TAILQ_NEXT(page, page_list)) {\
		TAILQ_INSERT_TAIL(&a_pool->freed_list, node, u.freed);\
	} else {\
		TAILQ_INSERT_HEAD(&a_pool->freed_list, node, u.freed);\
	}\
	--page->used_num;\
	if (0 == page->used_num) {\
		size_t i;\
		for (i = 0; page->ofs > i; ++i) {\
			TAILQ_REMOVE(&a_pool->freed_list,\
			    &page->node_array[i], u.freed);\
		}\
		TAILQ_REMOVE(&a_pool->page_list, page, page_list);\
		FREE(page);\
	}\
	*a_type = NULL;\
}\
static void \
PoolType##_init(struct PoolType *a_pool)\
{\
	TAILQ_INIT(&a_pool->page_list);\
	TAILQ_INIT(&a_pool->freed_list);\
}\
static void \
PoolType##_shutdown(struct PoolType *a_pool)\
{\
	while (!TAILQ_EMPTY(&a_pool->page_list)) {\
		struct PoolType##Page_ *page;\
		page = TAILQ_FIRST(&a_pool->page_list);\
		TAILQ_REMOVE(&a_pool->page_list, page, page_list);\
		FREE(page);\
	}\
}\
struct PoolType

#endif
