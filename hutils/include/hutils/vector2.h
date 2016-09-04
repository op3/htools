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

#ifndef HUTILS_VECTOR2_H
#define HUTILS_VECTOR2_H

#include <hutils/cdecls.h>
#include <hutils/funcattr.h>

CDECLS_BEGIN

struct Vector2f {
	float	x;
	float	y;
};

struct Vector2f	*vector2f_add(struct Vector2f *, struct Vector2f const *,
    struct Vector2f const *);
float		vector2f_dot(struct Vector2f const *, struct Vector2f const
    *) FUNC_PURE FUNC_RETURNS;
float		vector2f_get_magnitude(struct Vector2f const *) FUNC_PURE
FUNC_RETURNS;
struct Vector2f	*vector2f_mad(struct Vector2f *, struct Vector2f const *,
    struct Vector2f const *, struct Vector2f const *);
struct Vector2f	*vector2f_negate(struct Vector2f *, struct Vector2f const *);
struct Vector2f	*vector2f_normalize(struct Vector2f *, struct Vector2f const
    *);
struct Vector2f	*vector2f_scale(struct Vector2f *, struct Vector2f const *,
    float);
struct Vector2f	*vector2f_set(struct Vector2f *, float, float);
struct Vector2f	*vector2f_sub(struct Vector2f *, struct Vector2f const *,
    struct Vector2f const *);

CDECLS_END

#endif
