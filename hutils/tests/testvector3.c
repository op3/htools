/*
 * Copyright (c) 2014 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/vector3.h>
#include <math.h>
#include <htest/htest.h>

HTEST(CrossAxes)
{
	struct Vector3f const c_x = {1.0f, 0.0f, 0.0f};
	struct Vector3f const c_y = {0.0f, 1.0f, 0.0f};
	struct Vector3f const c_z = {0.0f, 0.0f, 1.0f};
	struct Vector3f v;

	HTRY_PTR(&v, ==, vector3f_cross(&v, &c_x, &c_y));
	HTRY_FLT(c_z.x, ==, v.x);
	HTRY_FLT(c_z.y, ==, v.y);
	HTRY_FLT(c_z.z, ==, v.z);

	HTRY_PTR(&v, ==, vector3f_cross(&v, &c_x, &c_z));
	HTRY_FLT(-c_y.x, ==, v.x);
	HTRY_FLT(-c_y.y, ==, v.y);
	HTRY_FLT(-c_y.z, ==, v.z);

	HTRY_PTR(&v, ==, vector3f_cross(&v, &c_y, &c_z));
	HTRY_FLT(c_x.x, ==, v.x);
	HTRY_FLT(c_x.y, ==, v.y);
	HTRY_FLT(c_x.z, ==, v.z);
}

HTEST(CrossQuadArea)
{
	struct Vector3f const c_v1 = {1.0f, 0.0f, 1.0f};
	struct Vector3f const c_v2 = {2.0f, 2.0f, 0.0f};
	struct Vector3f v;

	vector3f_cross(&v, &c_v1, &c_v2);
	HTRY_FLT(1e-3f, >, abs(sqrt(12) - vector3f_get_magnitude(&v)));
}

HTEST(CrossPointers)
{
	struct Vector3f u, v, w;

	HTRY_SIGNAL(vector3f_cross(&u, &u, &v));
	HTRY_SIGNAL(vector3f_cross(&u, &v, &u));
	HTRY_VOID(vector3f_cross(&u, &v, &w));
}

HTEST(DotAxes)
{
	struct Vector3f const c_x = {1.0f, 0.0f, 0.0f};
	struct Vector3f const c_y = {0.0f, 1.0f, 0.0f};
	struct Vector3f const c_z = {0.0f, 0.0f, 1.0f};

	HTRY_FLT(1.0f, ==, vector3f_dot(&c_x, &c_x));
	HTRY_FLT(0.0f, ==, vector3f_dot(&c_x, &c_y));
	HTRY_FLT(0.0f, ==, vector3f_dot(&c_x, &c_z));
	HTRY_FLT(1.0f, ==, vector3f_dot(&c_y, &c_y));
	HTRY_FLT(0.0f, ==, vector3f_dot(&c_y, &c_z));
	HTRY_FLT(1.0f, ==, vector3f_dot(&c_z, &c_z));
}

HTEST(Magnitude)
{
	struct Vector3f const c_v = {1.0f, 4.0f, 8.0f};

	HTRY_FLT(9.0f, ==, vector3f_get_magnitude(&c_v));
}

HTEST(Negate)
{
	struct Vector3f const c_v = {1.0f, 2.0f, 3.0f};
	struct Vector3f v;

	HTRY_PTR(&v, ==, vector3f_negate(&v, &c_v));
	HTRY_FLT(-1.0f, ==, v.x);
	HTRY_FLT(-2.0f, ==, v.y);
	HTRY_FLT(-3.0f, ==, v.z);
}

HTEST(Scale)
{
	struct Vector3f const c_v = {1.0f, 2.0f, 3.0f};
	struct Vector3f v;

	HTRY_PTR(&v, ==, vector3f_scale(&v, &c_v, 2.0f));
	HTRY_FLT(2.0f, ==, v.x);
	HTRY_FLT(4.0f, ==, v.y);
	HTRY_FLT(6.0f, ==, v.z);
}

HTEST(Sub)
{
	struct Vector3f const c_v1 = {5.0f, 9.0f, 11.0f};
	struct Vector3f const c_v2 = {1.0f, 2.0f, 3.0f};
	struct Vector3f v;

	HTRY_PTR(&v, ==, vector3f_sub(&v, &c_v1, &c_v2));
	HTRY_FLT(4.0f, ==, v.x);
	HTRY_FLT(7.0f, ==, v.y);
	HTRY_FLT(8.0f, ==, v.z);
}

HTEST_SUITE(Vector3f)
{
	HTEST_ADD(CrossAxes);
	HTEST_ADD(CrossQuadArea);
	HTEST_ADD(CrossPointers);
	HTEST_ADD(DotAxes);
	HTEST_ADD(Magnitude);
	HTEST_ADD(Negate);
	HTEST_ADD(Scale);
	HTEST_ADD(Sub);
}
