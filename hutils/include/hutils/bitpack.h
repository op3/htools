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

#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hutils/stdint.h>
#include <hconf/include/hutils/bitpack.h>

#if HCONF_BRANCH(BITPACK, IEEE754)
#	if HCONFING(BITPACK)
#		define HCONF_TEST return hconf_test_()
static int hconf_test_(void)
{
	union {
		float	flt;
		uint32_t	u32;
	} u;
	u.flt = 1.0f;
	if (0x3f800000 != u.u32) return 1;
	u.flt = -2.0f;
	if (0xc0000000 != u.u32) return 1;
	return 0;
}
#	endif
#endif

struct BitPacker {
	size_t	size;
	uint8_t	*buf;
	size_t	bit_i;
};

CDECLS_BEGIN

void	bitpack_init(struct BitPacker *, uint8_t *, size_t) FUNC_NONNULL(());
int	bitpack_float(struct BitPacker *, float, unsigned, unsigned)
	FUNC_NONNULL(()) FUNC_RETURNS;
int	bitpack_string(struct BitPacker *, char const *) FUNC_NONNULL(())
	FUNC_RETURNS;
int	bitpack_uint32(struct BitPacker *, uint32_t, unsigned) FUNC_NONNULL(())
	FUNC_RETURNS;
int	bitunpack_float(struct BitPacker *, float *, unsigned, unsigned)
	FUNC_NONNULL(()) FUNC_RETURNS;
int	bitunpack_string(struct BitPacker *, char **) FUNC_NONNULL(())
	FUNC_RETURNS;
int	bitunpack_uint32(struct BitPacker *, uint32_t *, unsigned)
	FUNC_NONNULL(()) FUNC_RETURNS;

CDECLS_END

#endif
