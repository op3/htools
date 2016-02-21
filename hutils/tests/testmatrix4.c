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

#include <hutils/matrix4.h>
#include <math.h>
#include <htest/htest.h>
#include <hutils/vector3.h>

HTEST(Identity)
{
	struct Matrix4f eye, a, b;
	size_t i;

	matrix4f_set_identity(&eye);
	for (i = 0; 16 > i; ++i) {
		a.m[i] = i;
	}

	matrix4f_mul(&b, &eye, &eye);
	for (i = 0; 16 > i; ++i) {
		HTRY_FLT(1e-9, >, fabs(b.m[i] - eye.m[i]));
	}

	matrix4f_mul(&b, &a, &eye);
	for (i = 0; 16 > i; ++i) {
		HTRY_FLT(1e-9, >, fabs(b.m[i] - a.m[i]));
	}

	matrix4f_mul(&b, &eye, &a);
	for (i = 0; 16 > i; ++i) {
		HTRY_FLT(1e-9, >, fabs(b.m[i] - a.m[i]));
	}
}

HTEST(Ortho)
{
	struct Matrix4f o;
	struct Vector3f u, v;

	matrix4f_set_ortho(&o, -2, 2, -2, 2, -2, 2);

	v.x = -2; v.y = -2; v.z = -2;
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT(-1, ==, u.x);
	HTRY_FLT(-1, ==, u.y);
	HTRY_FLT(+1, ==, u.z);

	v.x = 0; v.y = 0; v.z = 0;
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT(0, ==, u.x);
	HTRY_FLT(0, ==, u.y);
	HTRY_FLT(0, ==, u.z);

	v.x = 2; v.y = 2; v.z = 2;
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT(+1, ==, u.x);
	HTRY_FLT(+1, ==, u.y);
	HTRY_FLT(-1, ==, u.z);

	matrix4f_set_ortho(&o, 2, 0, 2, 0, 0, 2);

	v.x = 0; v.y = 0; v.z = 0;
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT(+1, ==, u.x);
	HTRY_FLT(+1, ==, u.y);
	HTRY_FLT(+1, ==, u.z);

	v.x = 1; v.y = 1; v.z = 1;
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT(0, ==, u.x);
	HTRY_FLT(0, ==, u.y);
	HTRY_FLT(0, ==, u.z);

	v.x = 2; v.y = 2; v.z = 2;
	matrix4f_mul_vector3f(&u, &o, &v);
	HTRY_FLT(-1, ==, u.x);
	HTRY_FLT(-1, ==, u.y);
	HTRY_FLT(-1, ==, u.z);
}

HTEST_SUITE(Matrix4)
{
	HTEST_ADD(Identity);
	HTEST_ADD(Ortho);
}
