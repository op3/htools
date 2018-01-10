/*
 * Copyright (c) 2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/time.h>
#include <math.h>
#include <htest/htest.h>

HTEST(GetTime)
{
	double t_0, t_1;

	t_0 = time_getd();
	t_1 = time_getd();
	HTRY_DBL(t_1, >=, t_0);
}

HTEST(Zzzzz)
{
	double t_0, t_1;

	t_0 = time_getd();
	time_sleep(1e-3);
	t_1 = time_getd();
	HTRY_DBL(1e-1, >, fabs(1e-3 - (t_1 - t_0)));

	HTRY_SIGNAL(time_sleep(-1));
	HTRY_VOID(time_sleep(0));
}

HTEST_SUITE(Time)
{
	HTEST_ADD(GetTime);
	HTEST_ADD(Zzzzz);
}
