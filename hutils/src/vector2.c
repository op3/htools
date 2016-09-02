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

#include <hutils/vector2.h>
#include <math.h>

struct Vector2f *
vector2f_add(struct Vector2f *const a_vo, struct Vector2f const *const a_v1,
    struct Vector2f const *const a_v2)
{
	a_vo->x = a_v1->x + a_v2->x;
	a_vo->y = a_v1->y + a_v2->y;
	return a_vo;
}

float
vector2f_dot(struct Vector2f const *const a_v1, struct Vector2f const *const
    a_v2)
{
	return a_v1->x * a_v2->x + a_v1->y * a_v2->y;
}

float
vector2f_get_magnitude(struct Vector2f const *const a_v)
{
	return sqrt(vector2f_dot(a_v, a_v));
}

struct Vector2f *
vector2f_mad(struct Vector2f *const a_vo, struct Vector2f const *const a_v0,
    struct Vector2f const *const a_v1, struct Vector2f const *const a_v2)
{
	a_vo->x = a_v0->x * a_v1->x + a_v2->x;
	a_vo->y = a_v0->y * a_v1->y + a_v2->y;
	return a_vo;
}

struct Vector2f	*
vector2f_negate(struct Vector2f *const a_vo, struct Vector2f const *const a_v)
{
	a_vo->x = -a_v->x;
	a_vo->y = -a_v->y;
	return a_vo;
}

struct Vector2f *
vector2f_normalize(struct Vector2f *const a_vo, struct Vector2f const *const
    a_v)
{
	return vector2f_scale(a_vo, a_v, 1.0f / vector2f_get_magnitude(a_v));
}

struct Vector2f *
vector2f_scale(struct Vector2f *const a_vo, struct Vector2f const *const a_v,
    float const a_f)
{
	a_vo->x = a_v->x * a_f;
	a_vo->y = a_v->y * a_f;
	return a_vo;
}

struct Vector2f *
vector2f_set(struct Vector2f *a_v, float a_x, float a_y)
{
	a_v->x = a_x;
	a_v->y = a_y;
	return a_v;
}

struct Vector2f *
vector2f_sub(struct Vector2f *const a_vo, struct Vector2f const *const a_v1,
    struct Vector2f const *const a_v2)
{
	a_vo->x = a_v1->x - a_v2->x;
	a_vo->y = a_v1->y - a_v2->y;
	return a_vo;
}
