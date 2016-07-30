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

#ifndef HUTILS_MATRIX4_H
#define HUTILS_MATRIX4_H

#include <hutils/funcattr.h>

struct Matrix4f {
	float	m[16];
};
struct Vector3f;

struct Matrix4f	*matrix4f_mul(struct Matrix4f *, struct Matrix4f const *,
    struct Matrix4f const *);
struct Vector3f	*matrix4f_mul_vector3f(struct Vector3f *, struct Matrix4f *,
    struct Vector3f const *);
struct Matrix4f	*matrix4f_set_identity(struct Matrix4f *);
struct Matrix4f	*matrix4f_set_ortho(struct Matrix4f *, float, float, float,
    float, float, float);

#endif
