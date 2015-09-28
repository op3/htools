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

#include <hutils/bitmask.h>
#include <stdint.h>
#include <hutils/memory.h>

struct Bitmask {
	int	size;
	uint32_t	*data;
};

void
bitmask_clear(struct Bitmask *const a_bm, int const a_index)
{
	if (0 > a_index || a_bm->size <= a_index) {
		abort();
	}
	a_bm->data[a_index / 32] &= ~(1 << (31 & a_index));
}

struct Bitmask *
bitmask_create(int const a_size)
{
	struct Bitmask *bm;

	if (0 >= a_size) {
		abort();
	}
	CALLOC(bm, 1);
	bm->size = a_size;
	CALLOC(bm->data, (31 + a_size) / 32);
	return bm;
}

void
bitmask_free(struct Bitmask **const a_bm)
{
	struct Bitmask *bm;

	bm = *a_bm;
	if (NULL == bm) {
		return;
	}
	FREE(bm->data);
	FREE(*a_bm);
}

int
bitmask_get(struct Bitmask *const a_bm, int const a_index)
{
	if (0 > a_index || a_bm->size <= a_index) {
		abort();
	}
	return (1 << (31 & a_index)) & a_bm->data[a_index / 32];
}

void
bitmask_one(struct Bitmask *const a_bm)
{
	memset(a_bm->data, 0xff, (a_bm->size + 7) / 8);
}

void
bitmask_set(struct Bitmask *const a_bm, int const a_index)
{
	if (0 > a_index || a_bm->size <= a_index) {
		abort();
	}
	a_bm->data[a_index / 32] |= (1 << (31 & a_index));
}

void
bitmask_zero(struct Bitmask *const a_bm)
{
	memset(a_bm->data, 0, (a_bm->size + 7) / 8);
}
