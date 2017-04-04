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

#ifndef HUTILS_UTF8_H
#define HUTILS_UTF8_H

#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hutils/stdint.h>

CDECLS_BEGIN

struct UTF8 {
	/* Number of replacements performed on source string. */
	int	replacement_num;
	size_t	length;
	size_t	bytes;
	uint8_t	*data;
};

struct UTF8	*utf8_create(uint8_t const *, size_t) FUNC_RETURNS;
void		utf8_free(struct UTF8 **);
void		utf8_get(struct UTF8 const *, size_t, uint32_t *, size_t *);

CDECLS_END

#endif
