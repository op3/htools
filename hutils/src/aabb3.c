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

#include <hutils/aabb3.h>
#include <math.h>
#include <hutils/macros.h>

float
aabb3f_get_distance(struct AABB3f const *a_aabb, struct Vector3f const *a_v)
{
	return (float)sqrt(aabb3f_get_distance_sqr(a_aabb, a_v));
}

float
aabb3f_get_distance_sqr(struct AABB3f const *a_aabb, struct Vector3f const
    *a_v)
{
	struct	Vector3f d, p;

	p.x = CLAMP(a_v->x, a_aabb->min.x, a_aabb->max.x);
	p.y = CLAMP(a_v->y, a_aabb->min.y, a_aabb->max.y);
	p.z = CLAMP(a_v->z, a_aabb->min.z, a_aabb->max.z);
	vector3f_sub(&d, &p, a_v);
	return vector3f_dot(&d, &d);
}
