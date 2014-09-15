/*
 * Copyright (c) 2014 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
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

#ifndef HUTILS_TAILQSORT_H
#define HUTILS_TAILQSORT_H

#define TAILQ_SORT_DECLARE(List, Node) \
static void tailq_sort_##List(struct List *, size_t)

#define TAILQ_SORT_DEFINE(List, Node, field, cmp) \
static void \
tailq_sort_##List(struct List *a_list, size_t a_size)\
{\
	struct List left, right;\
	struct Node *cur, *prev;\
	size_t middle, i;\
\
	if (1 >= a_size) {\
		return;\
	}\
\
	TAILQ_INIT(&left);\
	TAILQ_INIT(&right);\
\
	/* Find the middle. */\
	middle = a_size / 2;\
	i = middle;\
	TAILQ_FOREACH(cur, a_list, field) {\
		if (0 == i--) {\
			break;\
		}\
		prev = cur;\
	}\
\
	/* Hack-split lists. */\
	left.tqh_first = a_list->tqh_first;\
	left.tqh_last = &prev->field.tqe_next;\
	a_list->tqh_first->field.tqe_prev = &left.tqh_first;\
	prev->field.tqe_next = NULL;\
\
	right.tqh_first = cur;\
	right.tqh_last = a_list->tqh_last;\
	cur->field.tqe_prev = &right.tqh_first;\
\
	/* Recurse. */\
	tailq_sort_##List(&left, middle);\
	tailq_sort_##List(&right, a_size - middle);\
\
	/* Merge. */\
	TAILQ_INIT(a_list);\
	for (;;) {\
		struct Node *p_l, *p_r;\
\
		p_l = TAILQ_FIRST(&left);\
		p_r = TAILQ_FIRST(&right);\
		if (NULL == p_l || NULL == p_r) {\
			break;\
		}\
		if (cmp((struct Node const *)p_l, (struct Node const *)p_r) >\
		    0) {\
			TAILQ_REMOVE(&right, p_r, field);\
			TAILQ_INSERT_TAIL(a_list, p_r, field);\
		} else {\
			TAILQ_REMOVE(&left, p_l, field);\
			TAILQ_INSERT_TAIL(a_list, p_l, field);\
		}\
	}\
	while (!TAILQ_EMPTY(&left)) {\
		cur = TAILQ_FIRST(&left);\
		TAILQ_REMOVE(&left, cur, field);\
		TAILQ_INSERT_TAIL(a_list, cur, field);\
	}\
	while (!TAILQ_EMPTY(&right)) {\
		cur = TAILQ_FIRST(&right);\
		TAILQ_REMOVE(&right, cur, field);\
		TAILQ_INSERT_TAIL(a_list, cur, field);\
	}\
}\
TAILQ_SORT_DECLARE(List, Node)

#endif
