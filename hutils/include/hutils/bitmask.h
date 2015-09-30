/*
 * Copyright (c) 2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#ifndef HUTILS_BITMASK_H
#define HUTILS_BITMASK_H

#include <hutils/macros.h>

struct Bitmask;

void		bitmask_clear(struct Bitmask *, int);
struct Bitmask	*bitmask_copy(struct Bitmask const *) FUNC_RETURNS;
struct Bitmask	*bitmask_create(int) FUNC_RETURNS;
void		bitmask_free(struct Bitmask **);
int		bitmask_get(struct Bitmask *, int) FUNC_RETURNS;
void		bitmask_one(struct Bitmask *);
void		bitmask_set(struct Bitmask *, int);
void		bitmask_zero(struct Bitmask *);

#endif
