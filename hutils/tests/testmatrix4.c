/*
 * Copyright (c) 2016-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/matrix4.h>
#include <htest/htest.h>
#include <hutils/math.h>
#include <hutils/vector3.h>

HTEST(Identity)
{
	struct Matrix4f eye, a, b;
	size_t i;

	matrix4f_set_identity(&eye);
	for (i = 0; 16 > i; ++i) {
		a.m[i] = (float)i;
	}

	matrix4f_mul(&b, &eye, &eye);
	for (i = 0; 16 > i; ++i) {
		HTRY_DBL(1e-9, >, fabs(b.m[i] - eye.m[i]));
	}

	matrix4f_mul(&b, &a, &eye);
	for (i = 0; 16 > i; ++i) {
		HTRY_DBL(1e-9, >, fabs(b.m[i] - a.m[i]));
	}

	matrix4f_mul(&b, &eye, &a);
	for (i = 0; 16 > i; ++i) {
		HTRY_DBL(1e-9, >, fabs(b.m[i] - a.m[i]));
	}
}

HTEST(Ortho)
{
	struct Matrix4f o;
	struct Vector3f u, v;

	matrix4f_set_ortho(&o, -2.0f, 2.0f, -2.0f, 2.0f, -2.0f, 2.0f);

	vector3f_set(&v, -2.0f, -2.0f, -2.0f);
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT(-1.0f, ==, u.x);
	HTRY_FLT(-1.0f, ==, u.y);
	HTRY_FLT(+1.0f, ==, u.z);

	vector3f_set(&v, 0.0f, 0.0f, 0.0f);
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT( 0.0f, ==, u.x);
	HTRY_FLT( 0.0f, ==, u.y);
	HTRY_FLT( 0.0f, ==, u.z);

	vector3f_set(&v, 2.0f, 2.0f, 2.0f);
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT(+1.0f, ==, u.x);
	HTRY_FLT(+1.0f, ==, u.y);
	HTRY_FLT(-1.0f, ==, u.z);

	matrix4f_set_ortho(&o, 2, 0, 2, 0, 0, 2);

	vector3f_set(&v, 0.0f, 0.0f, 0.0f);
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT(+1.0f, ==, u.x);
	HTRY_FLT(+1.0f, ==, u.y);
	HTRY_FLT(+1.0f, ==, u.z);

	vector3f_set(&v, 1.0f, 1.0f, 1.0f);
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT( 0.0f, ==, u.x);
	HTRY_FLT( 0.0f, ==, u.y);
	HTRY_FLT( 0.0f, ==, u.z);

	vector3f_set(&v, 2.0f, 2.0f, 2.0f);
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT(-1.0f, ==, u.x);
	HTRY_FLT(-1.0f, ==, u.y);
	HTRY_FLT(-1.0f, ==, u.z);
}

HTEST(Perspective)
{
	struct Matrix4f p;
	struct Vector3f u, v;

	matrix4f_set_perspective(&p, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 3.0f);

	vector3f_set(&v, -1.0f, -1.0f, -1.0f);
	matrix4f_mul_vector3f(&u, &p, &v);
	HTRY_FLT(-1.0f, ==, u.x);
	HTRY_FLT(-1.0f, ==, u.y);
	HTRY_FLT(-1.0f, ==, u.z);

	vector3f_set(&v, 0.0f, 0.0f, -1.5f);
	matrix4f_mul_vector3f(&u, &p, &v);
	HTRY_FLT( 0.0f, ==, u.x);
	HTRY_FLT( 0.0f, ==, u.y);
	HTRY_FLT( 0.0f, ==, u.z);
}

HTEST(Rotation)
{
	struct Matrix4f r;
	struct Vector3f u, v;

	vector3f_set(&u, 1.0f, 0.0f, 0.0f);
	matrix4f_set_rotation(&r, &u, (float)(0.5 * M_PI));

	vector3f_set(&v, 1.0f, 1.0f, 1.0f);
	matrix4f_mul_vector3f(&u, &r, &v);
	HTRY_FLT(1e-6f, >, fabsf(u.x -  1.0f));
	HTRY_FLT(1e-6f, >, fabsf(u.y - -1.0f));
	HTRY_FLT(1e-6f, >, fabsf(u.z -  1.0f));

	vector3f_set(&u, 1.0f, 1.0f, 1.0f);
	vector3f_normalize(&u, &u);
	matrix4f_set_rotation(&r, &u, (float)(2 * M_PI / 3));

	vector3f_set(&v, 1.0f, 0.0f, 0.0f);
	matrix4f_mul_vector3f(&u, &r, &v);
	HTRY_FLT(1e-6f, >, fabsf(u.x - 0.0f));
	HTRY_FLT(1e-6f, >, fabsf(u.y - 1.0f));
	HTRY_FLT(1e-6f, >, fabsf(u.z - 0.0f));
}

HTEST_SUITE(Matrix4)
{
	HTEST_ADD(Identity);
	HTEST_ADD(Ortho);
	HTEST_ADD(Perspective);
	HTEST_ADD(Rotation);
}
