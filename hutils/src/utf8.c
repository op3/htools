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

#include <hutils/utf8.h>
#include <stdio.h>
#include <hutils/fmtmod.h>
#include <hutils/memory.h>

static int	get_code(uint8_t const *, size_t, size_t, uint32_t *, size_t
    *) FUNC_RETURNS;

int
get_code(uint8_t const *a_str, size_t a_bytes, size_t a_i, uint32_t *a_code,
    size_t *a_code_bytes)
{
	uint8_t const *p;
	size_t avail;

	if (a_bytes <= a_i) {
		fprintf(stderr, __FILE__":%d: UTF8 index %"PRIz" out of "
		    "bounds %"PRIz".\n", __LINE__, a_i, a_bytes);
		exit(EXIT_FAILURE);
	}
	p = &a_str[a_i];
	avail = a_bytes - a_i;
	if (0x80 > p[0]) {
		*a_code = p[0];
		*a_code_bytes = 1;
		return 1;
	} else if (0xc0 == (0xe0 & p[0])) {
		if (2 > avail || /* Code length. */
		    0x80 != (0xc0 & p[1]) || /* Continuation bits. */
		    0 == (0x1e & p[0])) { /* Overlong. */
			return 0;
		}
		*a_code = (0x1f & p[0]) << 6 | (0x3f & p[1]);
		*a_code_bytes = 2;
		return 1;
	} else if (0xe0 == (0xf0 & p[0])) {
		if (3 > avail || /* Code length. */
		    0x80 != (0xc0 & p[1]) || /* Continuation bits. */
		    0x80 != (0xc0 & p[2]) || /* Continuation bits. */
		    0 == ((0x0f & p[0]) | (0x20 & p[1]))) { /* Overlong. */
			return 0;
		}
		*a_code = (0x0f & p[0]) << 12 | (0x3f & p[1]) << 6 |
		    (0x3f & p[2]);
		if (0xd800 <= *a_code && 0xdfff >= *a_code) {
			/* UTF-16 surrogates. */
			return 0;
		}
		*a_code_bytes = 3;
		return 1;
	} else if (0xf0 == (0xf8 & p[0])) {
		if (4 > avail || /* Code length. */
		    0x80 != (0xc0 & p[1]) || /* Continuation bits. */
		    0x80 != (0xc0 & p[2]) || /* Continuation bits. */
		    0x80 != (0xc0 & p[3]) || /* Continuation bits. */
		    0 == ((0x07 & p[0]) | (0x30 & p[1]))) { /* Overlong. */
			return 0;
		}
		*a_code = (0x07 & p[0]) << 18 | (0x3f & p[1]) << 12 |
		    (0x3f & p[2]) << 6 | (0x3f & p[3]);
		*a_code_bytes = 4;
		return 1;
	}
	return 0;
}

struct UTF8 *
utf8_create(uint8_t const *a_str, size_t a_bytes)
{
	struct UTF8 *utf8;
	uint8_t *dst;
	size_t i, utf8_len, utf8_bytes;

	/* Calculate size with replacement for invalid sequences. */
	utf8_len = 0;
	utf8_bytes = 0;
	for (i = 0; a_bytes > i; ++utf8_len) {
		size_t len;
		uint32_t code;

		if (get_code(a_str, a_bytes, i, &code, &len)) {
			i += len;
			utf8_bytes += len;
		} else {
			++i;
			utf8_bytes += 3;
		}
	}
	/* Allocate and create string. */
	CALLOC(utf8, 1);
	utf8->length = utf8_len;
	utf8->bytes = utf8_bytes;
	utf8->data = malloc(utf8_bytes + 1);
	dst = utf8->data;
	for (i = 0; a_bytes > i;) {
		size_t len;
		uint32_t code;

		if (get_code(a_str, a_bytes, i, &code, &len)) {
			memmove(dst, a_str + i, len);
			i += len;
			dst += len;
		} else {
			++utf8->replacement_num;
			++i;
			*dst++ = 0xef;
			*dst++ = 0xbf;
			*dst++ = 0xbd;
		}
	}
	*dst++ = '\0';
	return utf8;
}

void
utf8_free(struct UTF8 **a_utf8)
{
	struct UTF8 *utf8;

	utf8 = *a_utf8;
	if (NULL == utf8) {
		return;
	}
	FREE(utf8->data);
	FREE(*a_utf8);
}

void
utf8_get(struct UTF8 const *a_utf8, size_t a_i, uint32_t *a_code, size_t
    *a_code_bytes)
{
	int	ret;

	(void)ret;
	ret = get_code(a_utf8->data, a_utf8->bytes, a_i, a_code,
	    a_code_bytes);
}
