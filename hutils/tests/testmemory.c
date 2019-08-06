/*
 * Copyright (c) 2015-2017, 2019
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

#include <hutils/memory.h>
#include <htest/htest.h>

struct SomeStruct {
	int	i;
};

HTEST(Dup)
{
	struct SomeStruct store;
	struct SomeStruct *dst;
	struct SomeStruct *src;

	src = &store;

	src->i = 1;
	DUP(dst, src);
	HTRY_I(1, ==, dst->i);

	src->i = 2;
	HTRY_I(1, ==, dst->i);
	FREE(dst);

	src->i = 3;
	DUP(dst, src);
	HTRY_I(3, ==, dst->i);
	FREE(dst);
}

HTEST(FreeDoesNullify)
{
	char *p;

	CALLOC(p, 1);
	HTRY_PTR(NULL, !=, p);
	FREE(p);
	HTRY_PTR(NULL, ==, p);
}

HTEST_SUITE(Memory)
{
	HTEST_ADD(Dup);
	HTEST_ADD(FreeDoesNullify);
}
