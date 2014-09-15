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

#include <htest/htest.h>
#include <hutils/vertex3.h>

HTEST(Cross)
{
	struct Vertex3 const c_x = {1.0f, 0.0f, 0.0f};
	struct Vertex3 const c_y = {0.0f, 1.0f, 0.0f};
	struct Vertex3 const c_z = {0.0f, 0.0f, 1.0f};
	struct Vertex3 v;

	HTRY_P(&v, ==, vertex3_cross(&v, &c_x, &c_y));
	HTRY_F(v.x, ==, c_z.x);
	HTRY_F(v.y, ==, c_z.y);
	HTRY_F(v.z, ==, c_z.z);

	HTRY_P(&v, ==, vertex3_cross(&v, &c_x, &c_z));
	HTRY_F(v.x, ==, -c_y.x);
	HTRY_F(v.y, ==, -c_y.y);
	HTRY_F(v.z, ==, -c_y.z);

	HTRY_P(&v, ==, vertex3_cross(&v, &c_y, &c_z));
	HTRY_F(v.x, ==, c_x.x);
	HTRY_F(v.y, ==, c_x.y);
	HTRY_F(v.z, ==, c_x.z);

	HTRY_SIGNAL(vertex3_cross(&v, &v, &c_x));
	HTRY_SIGNAL(vertex3_cross(&v, &c_x, &v));
}

HTEST(Magnitude)
{
	struct Vertex3 const c_v = {1.0f, 4.0f, 8.0f};

	HTRY_F(9.0f, ==, vertex3_get_magnitude(&c_v));
}

HTEST(Scale)
{
	struct Vertex3 const c_v = {1.0f, 2.0f, 3.0f};
	struct Vertex3 v;

	HTRY_P(&v, ==, vertex3_scale(&v, &c_v, 2.0f));
	HTRY_F(2.0f, ==, v.x);
	HTRY_F(4.0f, ==, v.y);
	HTRY_F(6.0f, ==, v.z);
}

HTEST(Sub)
{
	struct Vertex3 const c_v1 = {5.0f, 9.0f, 11.0f};
	struct Vertex3 const c_v2 = {1.0f, 2.0f, 3.0f};
	struct Vertex3 v;

	HTRY_P(&v, ==, vertex3_sub(&v, &c_v1, &c_v2));
	HTRY_F(4.0f, ==, v.x);
	HTRY_F(7.0f, ==, v.y);
	HTRY_F(8.0f, ==, v.z);
}

HTEST_SUITE(Vertex3)
{
	HTEST_ADD(Cross);
	HTEST_ADD(Magnitude);
	HTEST_ADD(Scale);
	HTEST_ADD(Sub);
}
