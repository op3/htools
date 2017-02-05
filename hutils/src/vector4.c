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
#include <hutils/math.h>

struct Vector4f *
vector4f_add(struct Vector4f *a_vo, struct Vector4f const *a_v1, struct
    Vector4f const *a_v2)
{
#define VEC4_ADD(c) a_vo->c = a_v1->c + a_v2->c
	VEC4_ADD(x);
	VEC4_ADD(y);
	VEC4_ADD(z);
	VEC4_ADD(w);
	return a_vo;
}

float
vector4f_dot(struct Vector4f const *a_v1, struct Vector4f const *a_v2)
{
#define VEC4_DOT(c) a_v1->c * a_v2->c
	return
	    VEC4_DOT(x) +
	    VEC4_DOT(y) +
	    VEC4_DOT(z) +
	    VEC4_DOT(w);
}

float
vector4f_get_magnitude(struct Vector4f const *a_v)
{
	return (float)sqrt(vector4f_dot(a_v, a_v));
}

struct Vector4f *
vector4f_scale(struct Vector4f *a_vo, struct Vector4f const *a_v, float a_f)
{
#define VEC4_SCALE(c) a_vo->c = a_v->c * a_f
	VEC4_SCALE(x);
	VEC4_SCALE(y);
	VEC4_SCALE(z);
	VEC4_SCALE(w);
	return a_vo;
}

struct Vector4f *
vector4f_set(struct Vector4f *a_v, float a_x, float a_y, float a_z, float a_w)
{
#define VEC4_SET(c) a_v->c = a_##c
	VEC4_SET(x);
	VEC4_SET(y);
	VEC4_SET(z);
	VEC4_SET(w);
	return a_v;
}

struct Vector4f *
vector4f_sub(struct Vector4f *a_vo, struct Vector4f const *a_v1, struct
    Vector4f const *a_v2)
{
#define VEC4_SUB(c) a_vo->c = a_v1->c - a_v2->c
	VEC4_SUB(x);
	VEC4_SUB(y);
	VEC4_SUB(z);
	VEC4_SUB(w);
	return a_vo;
}
