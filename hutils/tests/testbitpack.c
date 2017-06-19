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

#include <hutils/bitpack.h>
#include <htest/htest.h>
#include <hutils/math.h>
#include <hutils/memory.h>

HTEST(Packers)
{
	uint8_t buf[100];
	struct BitPacker packer;
	char *string;
	float flt;
	uint32_t u32;

	bitpack_init(&packer, buf, sizeof buf);
	HTRY_U(sizeof buf, ==, packer.size);
	HTRY_PTR(buf, ==, packer.buf);
	HTRY_U(0, ==, packer.bit_i);

	/* 1 bit + 9 bits -> resolution < 0.1%. */
	HTRY_BOOL(bitpack_pack_float(&packer, -M_PI, 3, 9));
	HTRY_I(13, ==, packer.bit_i);
	HTRY_BOOL(bitpack_pack_string(&packer, "yo"));
	HTRY_I(13 + 3 * 8, ==, packer.bit_i);
	HTRY_BOOL(bitpack_pack_uint32(&packer, 123, 7));
	HTRY_I(13 + 3 * 8 + 7, ==, packer.bit_i);

	/* Low-level bit check. */
	packer.bit_i = 0;
	HTRY_BOOL(bitpack_unpack_uint32(&packer, &u32, 13));
	HTRY_I(13, ==, packer.bit_i);
	HTRY_U(0x1924, ==, u32);
	HTRY_BOOL(bitpack_unpack_uint32(&packer, &u32, 8));
	HTRY_I(13 + 1 * 8, ==, packer.bit_i);
	HTRY_U('y', ==, u32);
	HTRY_BOOL(bitpack_unpack_uint32(&packer, &u32, 8));
	HTRY_I(13 + 2 * 8, ==, packer.bit_i);
	HTRY_U('o', ==, u32);
	HTRY_BOOL(bitpack_unpack_uint32(&packer, &u32, 8));
	HTRY_I(13 + 3 * 8, ==, packer.bit_i);
	HTRY_U('\0', ==, u32);
	HTRY_BOOL(bitpack_unpack_uint32(&packer, &u32, 7));
	HTRY_I(13 + 3 * 8 + 7, ==, packer.bit_i);
	HTRY_U(123, ==, u32);

	/* Higher-level unpacker check. */
	packer.bit_i = 0;
	HTRY_BOOL(bitpack_unpack_float(&packer, &flt, 3, 9));
	HTRY_I(13, ==, packer.bit_i);
	HTRY_FLT(-M_PI - 1e-3, <, flt);
	HTRY_FLT(-M_PI + 1e-3, >, flt);
	HTRY_BOOL(bitpack_unpack_string(&packer, &string));
	HTRY_I(13 + 3 * 8, ==, packer.bit_i);
	HTRY_STR("yo", ==, string);
	FREE(string);
	HTRY_BOOL(bitpack_unpack_uint32(&packer, &u32, 7));
	HTRY_I(13 + 3 * 8 + 7, ==, packer.bit_i);
	HTRY_U(123, ==, u32);
}

HTEST_SUITE(Bitpack)
{
	HTEST_ADD(Packers);
}
