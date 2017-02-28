/*
 * Copyright (c) 2015-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/hash.h>
#include <hutils/assert.h>
#include <hutils/fmtmod.h>

#define FNV_OFFSET_BASIS 2166136261U
#define FNV_PRIME 16777619

uint32_t
hutils_fnv1a32(void const *a_data, size_t a_data_bytes)
{
	uint8_t const *p8;
	size_t i;
	uint32_t hash;

	ASSERT(size_t, PRIz, 0, <, a_data_bytes);
	p8 = a_data;
	hash = FNV_OFFSET_BASIS;
	for (i = 0; i < a_data_bytes; ++i) {
		hash = (hash ^ *p8) * FNV_PRIME;
		++p8;
	}
	return hash;
}
