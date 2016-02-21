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
#include <hutils/macros.h>
#include <hutils/vector3.h>

struct Matrix4f *
matrix4f_mul(struct Matrix4f *const a_m, struct Matrix4f const *const a_l,
    struct Matrix4f const *const a_r)
{
	int i;

	for (i = 0; 4 > i; ++i) {
		int j;

		for (j = 0; 4 > j; ++j) {
			a_m->m[i + 4 * j] =
			    a_l->m[i + 4 * 0] * a_r->m[0 + 4 * j] +
			    a_l->m[i + 4 * 1] * a_r->m[1 + 4 * j] +
			    a_l->m[i + 4 * 2] * a_r->m[2 + 4 * j] +
			    a_l->m[i + 4 * 3] * a_r->m[3 + 4 * j];
		}
	}
	return a_m;
}

struct Vector3f *
matrix4f_mul_vector3f(struct Vector3f *const a_u, struct Matrix4f *const a_l,
    struct Vector3f const *const a_r)
{
	a_u->x = a_l->m[0] * a_r->x + a_l->m[4] * a_r->y + a_l->m[8] * a_r->z
	    + a_l->m[12];
	a_u->y = a_l->m[1] * a_r->x + a_l->m[5] * a_r->y + a_l->m[9] * a_r->z
	    + a_l->m[13];
	a_u->z = a_l->m[2] * a_r->x + a_l->m[6] * a_r->y + a_l->m[10] * a_r->z
	    + a_l->m[14];
	return a_u;
}

struct Matrix4f *
matrix4f_set_identity(struct Matrix4f *const a_m)
{
	ZERO(a_m->m);
	a_m->m[0] = 1.0f;
	a_m->m[5] = 1.0f;
	a_m->m[10] = 1.0f;
	a_m->m[15] = 1.0f;
	return a_m;
}

struct Matrix4f *
matrix4f_set_ortho(struct Matrix4f *const a_m, float const a_left, float const
    a_right, float const a_bottom, float const a_top, float const a_near,
    float const a_far)
{
	ZERO(a_m->m);
	a_m->m[0] = 2.0f / (a_right - a_left);
	a_m->m[5] = 2.0f / (a_top - a_bottom);
	a_m->m[10] = 2.0f / (a_near - a_far);
	a_m->m[12] = (a_left + a_right) / (a_left - a_right);
	a_m->m[13] = (a_bottom + a_top) / (a_bottom - a_top);
	a_m->m[14] = (a_far + a_near) / (a_far - a_near);
	a_m->m[15] = 1.0f;
	return a_m;
}
