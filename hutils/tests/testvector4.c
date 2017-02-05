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

#include <hutils/vector4.h>
#include <math.h>
#include <htest/htest.h>

HTEST(Add)
{
	struct Vector4f const c_v1 = {5.0f, 9.0f, 11.0f, 13.0f};
	struct Vector4f const c_v2 = {1.0f, 2.0f, 3.0f, 4.0f};
	struct Vector4f v;

	HTRY_PTR(&v, ==, vector4f_add(&v, &c_v1, &c_v2));
	HTRY_FLT(6.0f, ==, v.x);
	HTRY_FLT(11.0f, ==, v.y);
	HTRY_FLT(14.0f, ==, v.z);
	HTRY_FLT(17.0f, ==, v.w);
}

HTEST(DotAxes)
{
	struct Vector4f const c_x = {1.0f, 0.0f, 0.0f, 0.0f};
	struct Vector4f const c_y = {0.0f, 1.0f, 0.0f, 0.0f};
	struct Vector4f const c_z = {0.0f, 0.0f, 1.0f, 0.0f};
	struct Vector4f const c_w = {0.0f, 0.0f, 0.0f, 1.0f};

	HTRY_FLT(1.0f, ==, vector4f_dot(&c_x, &c_x));
	HTRY_FLT(0.0f, ==, vector4f_dot(&c_x, &c_y));
	HTRY_FLT(0.0f, ==, vector4f_dot(&c_x, &c_z));
	HTRY_FLT(0.0f, ==, vector4f_dot(&c_x, &c_w));
	HTRY_FLT(1.0f, ==, vector4f_dot(&c_y, &c_y));
	HTRY_FLT(0.0f, ==, vector4f_dot(&c_y, &c_z));
	HTRY_FLT(0.0f, ==, vector4f_dot(&c_y, &c_w));
	HTRY_FLT(1.0f, ==, vector4f_dot(&c_z, &c_z));
	HTRY_FLT(0.0f, ==, vector4f_dot(&c_z, &c_w));
	HTRY_FLT(1.0f, ==, vector4f_dot(&c_w, &c_w));
}

HTEST(Magnitude)
{
	struct Vector4f const c_v = {2.0f, 4.0f, 5.0f, 6.0f};

	HTRY_FLT(9.0f, ==, vector4f_get_magnitude(&c_v));
}

HTEST(Scale)
{
	struct Vector4f const c_v = {1.0f, 2.0f, 3.0f, 4.0f};
	struct Vector4f v;

	HTRY_PTR(&v, ==, vector4f_scale(&v, &c_v, 2.0f));
	HTRY_FLT(2.0f, ==, v.x);
	HTRY_FLT(4.0f, ==, v.y);
	HTRY_FLT(6.0f, ==, v.z);
	HTRY_FLT(8.0f, ==, v.w);
}

HTEST(Set)
{
	struct Vector4f v;

	vector4f_set(&v, 1.0f, 2.0f, 3.0f, 4.0f);
	HTRY_FLT(1.0f, ==, v.x);
	HTRY_FLT(2.0f, ==, v.y);
	HTRY_FLT(3.0f, ==, v.z);
	HTRY_FLT(4.0f, ==, v.w);
}

HTEST(Sub)
{
	struct Vector4f const c_v1 = {5.0f, 9.0f, 11.0f, 13.0f};
	struct Vector4f const c_v2 = {1.0f, 2.0f, 3.0f, 4.0f};
	struct Vector4f v;

	HTRY_PTR(&v, ==, vector4f_sub(&v, &c_v1, &c_v2));
	HTRY_FLT(4.0f, ==, v.x);
	HTRY_FLT(7.0f, ==, v.y);
	HTRY_FLT(8.0f, ==, v.z);
	HTRY_FLT(9.0f, ==, v.w);
}

HTEST_SUITE(Vector4)
{
	HTEST_ADD(Add);
	HTEST_ADD(DotAxes);
	HTEST_ADD(Magnitude);
	HTEST_ADD(Scale);
	HTEST_ADD(Set);
	HTEST_ADD(Sub);
}
