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

#ifndef HUTILS_POOL_H
#define HUTILS_POOL_H

#include <hutils/assert.h>
#include <hutils/memory.h>
#include <hutils/queue.h>

#define POOL_ENTRY(PoolType, Type) struct {\
	struct	Pool##PoolType##Page_ *page;\
	TAILQ_ENTRY(Type)	next;\
}
#define POOL_HEAD(PoolType, Type, num) \
TAILQ_HEAD(Pool##PoolType##List_, Type);\
struct Pool##PoolType##Page_ {\
	struct	Type node_array[num];\
	struct	Pool##PoolType##List_	node_list;\
	size_t	used_num;\
	TAILQ_ENTRY(Pool##PoolType##Page_)	next;\
};\
TAILQ_HEAD(PoolType, Pool##PoolType##Page_)

#define POOL_PROTOTYPE(PoolType, Type) \
static struct Type	*PoolType##_alloc(struct PoolType *);\
static void		PoolType##_free(struct PoolType *, struct Type **);\
static void		PoolType##_init(struct PoolType *);\
static void		PoolType##_shutdown(struct PoolType *)

#define POOL_IMPLEMENT(PoolType, Type, entry) \
static struct Type *\
PoolType##_alloc(struct PoolType *a_pool)\
{\
	struct Pool##PoolType##Page_ *page;\
	struct Type *node;\
\
	TAILQ_FOREACH(page, a_pool, next) {\
		if (!TAILQ_EMPTY(&page->node_list)) {\
			break;\
		}\
	}\
	if (TAILQ_END(a_pool) == page) {\
		size_t i;\
\
		page = malloc(sizeof *page);\
		TAILQ_INIT(&page->node_list);\
		for (i = 0; LENGTH(page->node_array) > i; ++i) {\
			page->node_array[i].entry.page = page;\
			TAILQ_INSERT_TAIL(&page->node_list,\
			    &page->node_array[i], entry.next);\
		}\
		page->used_num = 0;\
		TAILQ_INSERT_TAIL(a_pool, page, next);\
	}\
	node = TAILQ_FIRST(&page->node_list);\
	TAILQ_REMOVE(&page->node_list, node, entry.next);\
	++page->used_num;\
	return node;\
}\
static void \
PoolType##_free(struct PoolType *a_pool, struct Type **a_node)\
{\
	struct Type *node;\
	struct Pool##PoolType##Page_ *page;\
\
	node = *a_node;\
	page = node->entry.page;\
	ASSERT(void *, "p", (void *)&page->node_array, <=, (void *)node);\
	ASSERT(void *, "p", (void *)node, <, (void *)&page->node_list);\
	TAILQ_INSERT_TAIL(&page->node_list, node, entry.next);\
	--page->used_num;\
	if (0 == page->used_num) {\
		TAILQ_REMOVE(a_pool, page, next);\
	}\
	*a_node = NULL;\
}\
static void \
PoolType##_init(struct PoolType *a_pool)\
{\
	TAILQ_INIT(a_pool);\
}\
static void \
PoolType##_shutdown(struct PoolType *a_pool)\
{\
	while (!TAILQ_EMPTY(a_pool)) {\
		struct Pool##PoolType##Page_ *page;\
\
		page = TAILQ_FIRST(a_pool);\
		TAILQ_REMOVE(a_pool, page, next);\
		FREE(page);\
	}\
}\
struct PoolType

#endif
