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

#include <hutils/vector.h>
#include <htest/htest.h>
#include <hutils/random.h>

#define CHUNK 10
#define N 100

struct Int {
	int	i;
};
VECTOR_HEAD(IntVector, Int);

VECTOR_PROTOTYPE(static, boris, IntVector, Int);
VECTOR_IMPLEMENT(static, boris, IntVector, Int, CHUNK);

HTEST(CreateAtFree1)
{
	struct IntVector vec;
	struct Int *j;
	int i;

	boris_init(&vec, 1);
	HTRY_I(0, ==, vec.ofs);
	HTRY_I(1, ==, vec.size);
	HTRY_I(1, ==, vec.capacity);
	boris_at(&vec, 0)->i = 3;

	HTRY_SIGNAL(i = boris_at(&vec, (unsigned)-1)->i);
	i = boris_at(&vec, 0)->i;
	HTRY_I(3, ==, i);
	HTRY_SIGNAL(i = boris_at(&vec, 1)->i);

	VECTOR_FOREACH(j, &vec) {
		HTRY_I(3, ==, j->i);
	}

	boris_clean(&vec);
}

HTEST(PushPopBackMany)
{
	struct IntVector vec;
	struct Int *j;
	size_t i;

	boris_init(&vec, 0);

	for (i = 0; N > i; ++i) {
		HTRY_I(i, ==, vec.size);
		boris_push_back(&vec)->i = 100 + i;
		HTRY_I(i + 1, ==, vec.size);
	}
	i = 0;
	VECTOR_FOREACH(j, &vec) {
		HTRY_I(100 + i, ==, j->i);
		++i;
	}
	HTRY_I(N, ==, i);
	for (i = 0; N > i; ++i) {
		HTRY_I(99 + N - i, ==, boris_back(&vec)->i);
		boris_pop_back(&vec);
		HTRY_I(N - 1 - i, ==, vec.size);
	}
	HTRY_I(0, ==, vec.ofs);
	HTRY_I(CHUNK, ==, vec.capacity);

	boris_clean(&vec);
}

HTEST(PushPopFrontMany)
{
	struct IntVector vec;
	struct Int *j;
	size_t i;

	boris_init(&vec, 0);

	for (i = 0; N > i; ++i) {
		boris_push_front(&vec)->i = 100 + i;
	}
	HTRY_I(N, ==, vec.size);
	i = 0;
	VECTOR_FOREACH(j, &vec) {
		HTRY_I(99 + N - i, ==, j->i);
		++i;
	}
	HTRY_I(N, ==, i);
	for (i = 0; N > i; ++i) {
		HTRY_I(99 + N - i, ==, boris_front(&vec)->i);
		boris_pop_front(&vec);
		HTRY_I(N - 1 - i, ==, vec.size);
	}
	HTRY_I(0, ==, vec.size);

	boris_clean(&vec);
}

HTEST(Insert)
{
	struct RandomSeed seed = {{1, 2, 3, 4, 5}};
	struct IntVector vec;
	int count[N];
	struct Int *j;
	size_t i;

	boris_init(&vec, 0);

	for (i = 0; N > i; ++i) {
		int idx;

		idx = random_getu32(&seed) % (vec.size + 1);
		boris_insert(&vec, idx)->i = i;
	}
	ZERO(count);
	VECTOR_FOREACH(j, &vec) {
		count[j->i] += j->i;
	}
	for (i = 0; N > i; ++i) {
		HTRY_I(i, ==, count[i]);
	}

	boris_clean(&vec);
}

HTEST(Erase)
{
	struct RandomSeed seed = {{1, 2, 3, 4, 5}};
	struct IntVector vec;
	int count[N];
	size_t i;

	boris_init(&vec, 0);

	for (i = 0; N > i; ++i) {
		boris_push_back(&vec)->i = i;
	}
	for (i = 0; N > i; ++i) {
		boris_push_front(&vec)->i = i;
	}
	ZERO(count);
	for (i = 0; 2 * N > i; ++i) {
		int j, idx;

		idx = random_getu32(&seed) % vec.size;
		j = boris_at(&vec, idx)->i ;
		HTRY_I(0, <=, j);
		HTRY_I(N, >, j);
		++count[j];
		boris_erase(&vec, idx);
	}
	for (i = 0; N > i; ++i) {
		HTRY_I(2, ==, count[i]);
	}

	boris_clean(&vec);
}

HTEST(Resize)
{
	struct IntVector vec;
	size_t i;

	boris_init(&vec, 0);

	for (i = 0; N > i; ++i) {
		boris_push_back(&vec)->i = i;
	}
	for (i = N; 0 < i; i >>= 1) {
		struct Int *j;
		size_t k;

		boris_resize(&vec, i);
		HTRY_I(i, ==, vec.size);
		k = 0;
		VECTOR_FOREACH(j, &vec) {
			HTRY_I(k, ==, j->i);
			++k;
		}
	}

	boris_clean(&vec);
}

HTEST_SUITE(Vector)
{
	HTEST_ADD(CreateAtFree1);
	HTEST_ADD(PushPopBackMany);
	HTEST_ADD(PushPopFrontMany);
	HTEST_ADD(Insert);
	HTEST_ADD(Erase);
	HTEST_ADD(Resize);
}
