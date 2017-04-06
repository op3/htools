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

#ifndef HUTILS_VECTOR4_H
#define HUTILS_VECTOR4_H

#include <hutils/cdecls.h>
#include <hutils/funcattr.h>

CDECLS_BEGIN

struct Vector4f {
	float	x;
	float	y;
	float	z;
	float	w;
};

struct Vector4f	*vector4f_add(struct Vector4f *, struct Vector4f const *,
    struct Vector4f const *) FUNC_NONNULL(());
float		vector4f_dot(struct Vector4f const *, struct Vector4f const
    *) FUNC_NONNULL(()) FUNC_PURE FUNC_RETURNS;
float		vector4f_get_magnitude(struct Vector4f const *) FUNC_NONNULL(())
	FUNC_PURE FUNC_RETURNS;
struct Vector4f	*vector4f_scale(struct Vector4f *, struct Vector4f const *,
    float) FUNC_NONNULL(());
struct Vector4f	*vector4f_set(struct Vector4f *, float, float, float, float)
	FUNC_NONNULL(());
struct Vector4f	*vector4f_sub(struct Vector4f *, struct Vector4f const *,
    struct Vector4f const *) FUNC_NONNULL(());

CDECLS_END

#endif
