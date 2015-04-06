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

#include <hutils/macros.h>
#include <htest/htest.h>

HTEST(IsPow2)
{
	HTRY_I(1, ==, IS_POW2(1));
	HTRY_I(1, ==, IS_POW2(2));
	HTRY_I(0, ==, IS_POW2(3));
	HTRY_I(1, ==, IS_POW2(4));
	HTRY_I(0, ==, IS_POW2(5));
	HTRY_I(0, ==, IS_POW2(6));
	HTRY_I(0, ==, IS_POW2(7));
	HTRY_I(1, ==, IS_POW2(8));
	HTRY_I(0, ==, IS_POW2(9));
}

HTEST(Length)
{
	int8_t p8[2];
	int32_t p32[3];

	HTRY_I(2, ==, LENGTH(p8));
	HTRY_I(3, ==, LENGTH(p32));
}

HTEST(MaxMin)
{
	HTRY_I(1, ==, MAX(0, 1));
	HTRY_I(2, ==, MAX(-1, 2));
	HTRY_I(0, ==, MIN(0, 1));
	HTRY_I(-1, ==, MIN(-1, 2));
}

HTEST(Trunc)
{
	HTRY_I(-1, ==, TRUNC(-2, -1, 1));
	HTRY_I(-1, ==, TRUNC(-1, -1, 1));
	HTRY_I(0, ==, TRUNC(0, -1, 1));
	HTRY_I(1, ==, TRUNC(1, -1, 1));
	HTRY_I(1, ==, TRUNC(2, -1, 1));
}

HTEST_SUITE(Macros)
{
	HTEST_ADD(IsPow2);
	HTEST_ADD(Length);
	HTEST_ADD(MaxMin);
	HTEST_ADD(Trunc);
}
