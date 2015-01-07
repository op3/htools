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

#include <hutils/tailqsort.h>
#include <sys/queue.h>
#include <htest/htest.h>

#define N 100

TAILQ_HEAD(IntegerList, Integer);
struct Integer {
	int	i;
	TAILQ_ENTRY(Integer)	next;
};

static int	cmp(struct Integer const *, struct Integer const *);

TAILQ_SORT_DEFINE(IntegerList, sort_ascending, Integer, next, cmp);

int
cmp(struct Integer const *a_l, struct Integer const *a_r)
{
	return a_l->i - a_r->i;
}

HTEST(InOrder)
{
	struct IntegerList list;
	struct Integer i[N];
	struct Integer *p;
	int idx;

	TAILQ_INIT(&list);
	for (idx = 0; N > idx; ++idx) {
		i[idx].i = idx;
		TAILQ_INSERT_TAIL(&list, &i[idx], next);
	}
	sort_ascending(&list, N);

	idx = 0;
	TAILQ_FOREACH(p, &list, next) {
		HTRY_I(p->i, ==, idx);
		++idx;
	}
}

HTEST(Reversed)
{
	struct IntegerList list;
	struct Integer i[N];
	struct Integer *p;
	int idx;

	TAILQ_INIT(&list);
	for (idx = 0; N > idx; ++idx) {
		i[idx].i = N - idx;
		TAILQ_INSERT_TAIL(&list, &i[idx], next);
	}
	sort_ascending(&list, N);

	idx = 1;
	TAILQ_FOREACH(p, &list, next) {
		HTRY_I(p->i, ==, idx);
		++idx;
	}
}

HTEST(PseudoRandom)
{
	struct IntegerList list;
	struct Integer i[N];
	struct Integer *p;
	int idx, prev;

	TAILQ_INIT(&list);
	for (idx = 0; N > idx; ++idx) {
		i[idx].i = 15 & rand();
		TAILQ_INSERT_TAIL(&list, &i[idx], next);
	}
	sort_ascending(&list, N);

	prev = 0;
	TAILQ_FOREACH(p, &list, next) {
		HTRY_I(prev, <=, p->i);
		prev = p->i;
	}
}

HTEST_SUITE(TailqSort)
{
	HTEST_ADD(InOrder);
	HTEST_ADD(Reversed);
	HTEST_ADD(PseudoRandom);
}
