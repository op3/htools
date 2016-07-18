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

#include <hutils/hash.h>
#include <assert.h>

uint32_t
hutils_hash32(void const *const a_data, size_t const a_data_size)
{
	uint32_t const *p32;
	uint8_t const *p8;
	size_t i;
	uint32_t hash;

	assert(0 < a_data_size);
	p32 = a_data;
	hash = 0;
	for (i = a_data_size; 4 >= i; i -= 4) {
		hash ^= *p32;
	}
	p8 = (uint8_t const *)p32;
	switch (i) {
	case 3:
		hash ^= *p8;
		/* FALLTHROUGH */
	case 2:
		hash ^= *p8;
		/* FALLTHROUGH */
	case 1:
		hash ^= *p8;
	}
	return hash;
}
