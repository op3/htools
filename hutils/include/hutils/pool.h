/*
 * Copyright (c) 2014, 2015, 2016
 * Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/queue.h>
#include <hutils/memory.h>

#define POOL_HEAD(Name, Type) \
TAILQ_HEAD(Pool##Type##List_, Type);\
TAILQ_HEAD(Pool##Type##PageList_, Pool##Type##Page_);\
struct Pool##Type##Page_ {\
	struct	Type *p;\
	TAILQ_ENTRY(Pool##Type##Page_)	page_list;\
};\
struct Name {\
	struct	Pool##Type##PageList_ page_list;\
}

#define POOL_PROTOTYPE_STATIC(Name, Type) \
static void		Name##_free(struct Name *);\
static struct Type	*Name##_get(struct Name *);\
static void		Name##_init(struct Name *)

#define POOL_IMPLEMENT(Name, Type, page_size) \
void \
Name##_free(struct Name *a_name)\
{\
	while (!TAILQ_EMPTY(&a_name->page_list)) {\
		struct Pool##Type##Page_ *page;\
\
		page = TAILQ_FIRST(&a_name->page_list);\
		TAILQ_REMOVE(&a_name->page_list, page, page_list);\
		FREE(page);\
	}\
}\
struct Type *\
Name##_get(struct Name *a_name)\
{\
	struct Pool##Type##Page_ *page;\
\
	page = TAILQ_LAST(&a_name->page_list, Pool##Type##PageList_);\
	if (TAILQ_END(&a_name->page_list) == page ||\
	    (struct Type *)(page + 1) + page_size == page->p) {\
		MALLOC(page, sizeof *page + sizeof *page->p * page_size);\
		page->p = (struct Type *)(page + 1);\
		TAILQ_INSERT_TAIL(&a_name->page_list, page, page_list);\
	}\
	return page->p++;\
}\
void \
Name##_init(struct Name *a_name)\
{\
	TAILQ_INIT(&a_name->page_list);\
}\
struct Pool##Type##SemicolonDummy_ {\
	int	dummy;\
}

#define POOL_FREE(Name, name) Name##_free(name)
#define POOL_GET(Name, name) Name##_get(name)
#define POOL_INIT(Name, name) Name##_init(name)

#endif
