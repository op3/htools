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

#define N 100

VECTOR_HEAD(IntVector, int);

VECTOR_IMPLEMENT(IntVector, int, 10);

HTEST(CreateAtFree1)
{
	struct IntVector vec;
	int i;

	IntVector_create(&vec, 1);
	HTRY_I(1, ==, vec.size);
	*IntVector_at(&vec, 0) = 3;
	HTRY_SIGNAL(i = *IntVector_at(&vec, -1));
	i = *IntVector_at(&vec, 0);
	HTRY_I(3, ==, i);
	HTRY_SIGNAL(i = *IntVector_at(&vec, 1));
	IntVector_free(&vec);
}

HTEST(PushPopBack100)
{
	struct IntVector vec;
	size_t i;
	int j;

	IntVector_create(&vec, 0);

	for (i = 0; N > i; ++i) {
		*IntVector_push_back(&vec) = 100 + i;
	}
	HTRY_I(N, ==, vec.size);
	for (i = 0; N > i; ++i) {
		j = *IntVector_at(&vec, i);
		HTRY_I(100 + i, ==, j);
	}
	for (i = 0; N > i; ++i) {
		j = *IntVector_back(&vec);
		HTRY_I(99 + N - i, ==, j);
		IntVector_pop_back(&vec);
		HTRY_I(N - i - 1, ==, vec.size);
	}

	IntVector_free(&vec);
}

HTEST(PushPopFront100)
{
	struct IntVector vec;
	size_t i;
	int j;

	IntVector_create(&vec, 0);

	for (i = 0; N > i; ++i) {
		*IntVector_push_front(&vec) = 100 + i;
	}
	HTRY_I(N, ==, vec.size);
	for (i = 0; N > i; ++i) {
		j = *IntVector_at(&vec, i);
		HTRY_I(99 + N - i, ==, j);
	}
	for (i = 0; N > i; ++i) {
		j = *IntVector_front(&vec);
		HTRY_I(99 + N - i, ==, j);
		IntVector_pop_front(&vec);
		HTRY_I(N - i - 1, ==, vec.size);
	}

	IntVector_free(&vec);
}

HTEST_SUITE(Vector)
{
	HTEST_ADD(CreateAtFree1);
	HTEST_ADD(PushPopBack100);
	HTEST_ADD(PushPopFront100);
}
