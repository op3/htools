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

#ifndef HUTILS_BITPACK_H
#define HUTILS_BITPACK_H

#include <stdlib.h>
#include <hutils/funcattr.h>
#include <hutils/stdint.h>

struct BitPacker {
	size_t	size;
	uint8_t	*buf;
	size_t	bit_i;
};

void	bitpack_init(struct BitPacker *, uint8_t *, size_t) FUNC_NONNULL(());
int	bitpack_pack_float(struct BitPacker *, float, unsigned, unsigned)
	FUNC_NONNULL(()) FUNC_RETURNS;
int	bitpack_pack_string(struct BitPacker *, char const *) FUNC_NONNULL(())
	FUNC_RETURNS;
int	bitpack_pack_uint32(struct BitPacker *, uint32_t, unsigned)
	FUNC_NONNULL(()) FUNC_RETURNS;
int	bitpack_unpack_float(struct BitPacker *, float *, unsigned, unsigned)
	FUNC_NONNULL(()) FUNC_RETURNS;
int	bitpack_unpack_string(struct BitPacker *, char **) FUNC_NONNULL(())
	FUNC_RETURNS;
int	bitpack_unpack_uint32(struct BitPacker *, uint32_t *, unsigned)
	FUNC_NONNULL(()) FUNC_RETURNS;

#endif
