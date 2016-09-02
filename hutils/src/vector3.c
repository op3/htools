/*
 * Copyright (c) 2015-2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
#include <assert.h>
#include <hutils/math.h>

struct Vector3f *
vector3f_add(struct Vector3f *const a_vo, struct Vector3f const *const a_v1,
    struct Vector3f const *const a_v2)
{
	a_vo->x = a_v1->x + a_v2->x;
	a_vo->y = a_v1->y + a_v2->y;
	a_vo->z = a_v1->z + a_v2->z;
	return a_vo;
}

struct Vector3f *
vector3f_cross(struct Vector3f *const a_vo, struct Vector3f const *const a_v1,
    struct Vector3f const *const a_v2)
{
	assert(a_vo != a_v1);
	assert(a_vo != a_v2);
	a_vo->x = a_v1->y * a_v2->z - a_v1->z * a_v2->y;
	a_vo->y = a_v1->z * a_v2->x - a_v1->x * a_v2->z;
	a_vo->z = a_v1->x * a_v2->y - a_v1->y * a_v2->x;
	return a_vo;
}

float
vector3f_dot(struct Vector3f const *const a_v1, struct Vector3f const *const
    a_v2)
{
	return a_v1->x * a_v2->x + a_v1->y * a_v2->y + a_v1->z * a_v2->z;
}

float
vector3f_get_magnitude(struct Vector3f const *const a_v)
{
	return sqrt(vector3f_dot(a_v, a_v));
}

struct Vector3f *
vector3f_negate(struct Vector3f *const a_vo, struct Vector3f const *a_v)
{
	a_vo->x = -a_v->x;
	a_vo->y = -a_v->y;
	a_vo->z = -a_v->z;
	return a_vo;
}

struct Vector3f *
vector3f_normalize(struct Vector3f *const a_vo, struct Vector3f const *a_v)
{
	return vector3f_scale(a_vo, a_v, 1.0f / vector3f_get_magnitude(a_v));
}

struct Vector3f *
vector3f_scale(struct Vector3f *const a_vo, struct Vector3f const *a_v, float
    const a_f)
{
	a_vo->x = a_v->x * a_f;
	a_vo->y = a_v->y * a_f;
	a_vo->z = a_v->z * a_f;
	return a_vo;
}

struct Vector3f *
vector3f_set(struct Vector3f *a_v, float a_x, float a_y, float a_z)
{
	a_v->x = a_x;
	a_v->y = a_y;
	a_v->z = a_z;
	return a_v;
}

struct Vector3f *
vector3f_sub(struct Vector3f *const a_vo, struct Vector3f const *const a_v1,
    struct Vector3f const *const a_v2)
{
	a_vo->x = a_v1->x - a_v2->x;
	a_vo->y = a_v1->y - a_v2->y;
	a_vo->z = a_v1->z - a_v2->z;
	return a_vo;
}
