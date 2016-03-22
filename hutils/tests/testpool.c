/*
 * Copyright (c) 2015-2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
#include <math.h>
#include <htest/htest.h>

struct Int {
	int	i;
};

POOL_HEAD(PoolInt, Int);

POOL_PROTOTYPE_STATIC(PoolInt, Int);
POOL_IMPLEMENT(PoolInt, Int, 10);

HTEST(AllocAndFree100)
{
	struct PoolInt pool_int;
	struct Int *p[100];
	size_t i;

	POOL_INIT(PoolInt, &pool_int);
	for (i = 0; 100 > i; ++i) {
		p[i] = POOL_GET(PoolInt, &pool_int);
		p[i]->i = 100 - i;
	}
	for (i = 0; 100 > i; ++i) {
		HTRY_I(p[i]->i, ==, 100 - i);
	}
	POOL_FREE(PoolInt, &pool_int);
}

HTEST_SUITE(Pool)
{
	HTEST_ADD(AllocAndFree100);
}
