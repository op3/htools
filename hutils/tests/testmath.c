/*
 * Copyright (c) 2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/math.h>
#include <htest/htest.h>

HTEST(Conversion)
{
	HTRY_I(0, ==, double2half(0.0));
	HTRY_I(0x3c00, ==, double2half(1.0));
	HTRY_I(0xc000, ==, double2half(-2.0));
	HTRY_I(0x7bff, ==, double2half(65504.0));

	HTRY_DBL(0.0, ==, half2double(0));
	HTRY_DBL(1.0, ==, half2double(0x3c00));
	HTRY_DBL(-2.0, ==, half2double(0xc000));
	HTRY_DBL(65504.0, ==, half2double(0x7bff));
}

HTEST_SUITE(Math)
{
	HTEST_ADD(Conversion);
}
