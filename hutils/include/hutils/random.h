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

#ifndef HUTILS_RANDOM_H
#define HUTILS_RANDOM_H

#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hutils/stdint.h>

struct RandomSeed {
	uint32_t	seed[5];
};

CDECLS_BEGIN

double		random_getd(struct RandomSeed *) FUNC_NONNULL(())
	FUNC_RETURNS;
float		random_getf(struct RandomSeed *) FUNC_NONNULL(())
	FUNC_RETURNS;
uint32_t	random_getu32(struct RandomSeed *) FUNC_NONNULL(())
	FUNC_RETURNS;

CDECLS_END

#endif
