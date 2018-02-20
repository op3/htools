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

#include <hutils/pool.h>
#include <htest/htest.h>

struct Int {
	int	i;
};
POOL_HEAD(PoolInt, Int, 10);
POOL_HEAD(PoolIntBig, Int, 10000);

POOL_PROTOTYPE(PoolInt, Int);
POOL_IMPLEMENT(PoolInt, Int);
POOL_PROTOTYPE(PoolIntBig, Int);
POOL_IMPLEMENT(PoolIntBig, Int);

HTEST(AllocAndFree100)
{
	struct PoolInt pool_int;
	struct Int *p[100];
	size_t i;

	PoolInt_init(&pool_int);
	HTRY_BOOL(TAILQ_EMPTY(&pool_int.freed_list));
	HTRY_BOOL(TAILQ_EMPTY(&pool_int.page_list));
	for (i = 0; LENGTH(p) - 1 > i; ++i) {
		p[i] = PoolInt_alloc(&pool_int);
		p[i]->i = i;
	}
	HTRY_BOOL(TAILQ_EMPTY(&pool_int.freed_list));
	HTRY_BOOL(!TAILQ_EMPTY(&pool_int.page_list));
	PoolInt_free(&pool_int, &p[0]);
	HTRY_BOOL(!TAILQ_EMPTY(&pool_int.freed_list));
	HTRY_BOOL(!TAILQ_EMPTY(&pool_int.page_list));
	for (i = 1; LENGTH(p) - 1 > i; ++i) {
		HTRY_I(p[i]->i, ==, i);
		PoolInt_free(&pool_int, &p[i]);
	}
	HTRY_BOOL(TAILQ_EMPTY(&pool_int.freed_list));
	HTRY_BOOL(TAILQ_EMPTY(&pool_int.page_list));
	PoolInt_shutdown(&pool_int);
}

HTEST(AllocAndFreeMulti)
{
	struct PoolIntBig pool_int;
	unsigned j;

	HTRY_VOID(j = 0);
	for (j = 0; j < 10; ++j) {
		size_t i;

		PoolIntBig_init(&pool_int);
		for (i = 0; 100000 > i; ++i) {
			PoolIntBig_alloc(&pool_int)->i = i;
		}
		PoolIntBig_shutdown(&pool_int);
	}
}

HTEST_SUITE(Pool)
{
	HTEST_ADD(AllocAndFree100);
	HTEST_ADD(AllocAndFreeMulti);
}
