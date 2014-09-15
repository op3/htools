/*
 * Copyright (c) 2014 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
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

#ifndef VERTEX3_H
#define VERTEX3_H

struct Vertex3 {
	float	x;
	float	y;
	float	z;
};

struct Vertex3 *vertex3_cross(struct Vertex3 *, struct Vertex3 const *, struct
    Vertex3 const *);
float vertex3_get_magnitude(struct Vertex3 const *);
struct Vertex3 *vertex3_scale(struct Vertex3 *, struct Vertex3 const *,
    float);
struct Vertex3 *vertex3_sub(struct Vertex3 *, struct Vertex3 const *, struct
    Vertex3 const *);

#endif
