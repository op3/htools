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

HTEST(Basic)
{
	uint8_t buf[100];
	struct BitPacker packer;

	bitpack_init(&packer, buf, sizeof buf);
	HTRY_U(sizeof buf, ==, packer.size);
	HTRY_PTR(buf, ==, packer.buf);
	HTRY_U(0, ==, packer.bit_i);
}

HTEST(Uint32Limits)
{
	uint8_t buf[5];
	struct BitPacker packer;
	uint32_t u32;
	int ret;

	bitpack_init(&packer, buf, sizeof buf);

	/* 0 bits make no sense. */
	HTRY_SIGNAL(ret = bitpack_uint32(&packer, 0, 0));
	HTRY_I(0, ==, packer.bit_i);
	ret = bitpack_uint32(&packer, 0, 1);
	HTRY_I(1, ==, packer.bit_i);
	ret = bitpack_uint32(&packer, 0, 32);
	HTRY_I(33, ==, packer.bit_i);
	/* 33 bits unsupported for now. */
	HTRY_SIGNAL(ret = bitpack_uint32(&packer, 0, 33));
	HTRY_I(33, ==, packer.bit_i);
	/* Overflow. */
	HTRY_SIGNAL(ret = bitpack_uint32(&packer, 0, 8));
	HTRY_I(33, ==, packer.bit_i);
	ret = bitpack_uint32(&packer, 0, 7);
	HTRY_I(40, ==, packer.bit_i);

	/* Analog unpack checks on rewound packer. */
	packer.bit_i = 0;
	HTRY_SIGNAL(ret = bitunpack_uint32(&packer, &u32, 0));
	HTRY_I(0, ==, packer.bit_i);
	ret = bitunpack_uint32(&packer, &u32, 1);
	HTRY_I(1, ==, packer.bit_i);
	ret = bitunpack_uint32(&packer, &u32, 32);
	HTRY_I(33, ==, packer.bit_i);
	HTRY_SIGNAL(ret = bitunpack_uint32(&packer, &u32, 8));
	HTRY_I(33, ==, packer.bit_i);
	ret = bitunpack_uint32(&packer, &u32, 7);
	HTRY_I(40, ==, packer.bit_i);
}

HTEST(Uint32)
{
	uint8_t buf[100];
	struct BitPacker packer;
	uint32_t ofs;
	int i, ret;

	bitpack_init(&packer, buf, sizeof buf);

	/* Packer must survive a dirty destination buffer. */
	memset(buf, 0xff, sizeof buf);

	ofs = 0;
	for (i = 1; i <= 32; ++i) {
		ret = bitpack_uint32(&packer, 0xaaaaaaaa, i);
		HTRY_BOOL(ret);
		ofs += i;
		HTRY_I(ofs, ==, packer.bit_i);
	}
	packer.bit_i = 0;
	ofs = 0;
	for (i = 1; i <= 32; ++i) {
		uint32_t val, u32;

		ret = bitunpack_uint32(&packer, &u32, i);
		HTRY_BOOL(ret);
		val = 0xaaaaaaaa & ((1 << (i - 1)) | ((1 << (i - 1)) - 1));
		HTRY_U(val, ==, u32);
		ofs += i;
		HTRY_I(ofs, ==, packer.bit_i);
	}
}

HTEST(FloatLimits)
{
	uint8_t buf[5];
	struct BitPacker packer;
	float flt;
	int ret;

	bitpack_init(&packer, buf, sizeof buf);

	/*
	 * Exponent = 0 or (1<<n)-1 are reserved for special cases, must have
	 * at least 2 bits.
	 * 0 bits mantissa also makes no sense.
	 * Sign bit always packed, so total bits = 1 + exp_bits + mant_bits.
	 */
	HTRY_SIGNAL(ret = bitpack_float(&packer, 0.0f, 0, 0));
	HTRY_SIGNAL(ret = bitpack_float(&packer, 0.0f, 1, 0));
	HTRY_SIGNAL(ret = bitpack_float(&packer, 0.0f, 2, 0));
	HTRY_SIGNAL(ret = bitpack_float(&packer, 0.0f, 0, 1));
	HTRY_SIGNAL(ret = bitpack_float(&packer, 0.0f, 1, 1));
	HTRY_I(0, ==, packer.bit_i);
	ret = bitpack_float(&packer, 0.0f, 2, 1);
	HTRY_I(1 + 2 + 1, ==, packer.bit_i);
	ret = bitpack_float(&packer, 0.0f, 8, 23);
	HTRY_I(1 + 2 + 1 + 1 + 8 + 23, ==, packer.bit_i);
	HTRY_SIGNAL(ret = bitpack_float(&packer, 0.0f, 9, 23));
	HTRY_SIGNAL(ret = bitpack_float(&packer, 0.0f, 8, 24));
	HTRY_I(1 + 2 + 1 + 1 + 8 + 23, ==, packer.bit_i);
	/* Overflow. */
	HTRY_SIGNAL(ret = bitpack_float(&packer, 0.0f, 2, 2));
	HTRY_I(1 + 2 + 1 + 1 + 8 + 23, ==, packer.bit_i);
	ret = bitpack_float(&packer, 0.0f, 2, 1);
	HTRY_I(40, ==, packer.bit_i);

	packer.bit_i = 0;
	HTRY_SIGNAL(ret = bitunpack_float(&packer, &flt, 0, 0));
	HTRY_SIGNAL(ret = bitunpack_float(&packer, &flt, 1, 0));
	HTRY_SIGNAL(ret = bitunpack_float(&packer, &flt, 2, 0));
	HTRY_SIGNAL(ret = bitunpack_float(&packer, &flt, 0, 1));
	HTRY_SIGNAL(ret = bitunpack_float(&packer, &flt, 1, 1));
	HTRY_I(0, ==, packer.bit_i);
	ret = bitunpack_float(&packer, &flt, 2, 1);
	HTRY_I(1 + 2 + 1, ==, packer.bit_i);
	ret = bitunpack_float(&packer, &flt, 8, 23);
	HTRY_I(1 + 2 + 1 + 1 + 8 + 23, ==, packer.bit_i);
	HTRY_SIGNAL(ret = bitunpack_float(&packer, &flt, 9, 23));
	HTRY_SIGNAL(ret = bitunpack_float(&packer, &flt, 8, 24));
	HTRY_I(1 + 2 + 1 + 1 + 8 + 23, ==, packer.bit_i);
	/* Overflow. */
	HTRY_SIGNAL(ret = bitunpack_float(&packer, &flt, 2, 2));
	HTRY_I(1 + 2 + 1 + 1 + 8 + 23, ==, packer.bit_i);
	ret = bitunpack_float(&packer, &flt, 2, 1);
	HTRY_I(40, ==, packer.bit_i);
}

HTEST(Float)
{
	uint8_t buf[1000];
	struct BitPacker packer;
	uint32_t exp_bits, mant_bits, ofs;

	bitpack_init(&packer, buf, sizeof buf);

	memset(buf, 0xff, sizeof buf);

	ofs = 0;
	for (exp_bits = 2; exp_bits <= 8; ++exp_bits) {
		for (mant_bits = 1; mant_bits <= 23; ++mant_bits) {
			int ret;

			ret = bitpack_float(&packer, M_PI, exp_bits,
			    mant_bits);
			HTRY_BOOL(ret);
			ofs += 1 + exp_bits + mant_bits;
			HTRY_I(ofs, ==, packer.bit_i);
		}
	}
	packer.bit_i = 0;
	ofs = 0;
	for (exp_bits = 2; exp_bits <= 8; ++exp_bits) {
		for (mant_bits = 1; mant_bits <= 23; ++mant_bits) {
			float flt, prec;
			int ret;

			ret = bitunpack_float(&packer, &flt, exp_bits,
			    mant_bits);
			HTRY_BOOL(ret);
			/* Exp = 1 && mant_bits = 1 -> prec ~= +-1. */
			prec = 2.0f / (1 << mant_bits);
			HTRY_FLT(M_PI - prec, <, flt);
			HTRY_FLT(M_PI + prec, >, flt);
			ofs += 1 + exp_bits + mant_bits;
			HTRY_I(ofs, ==, packer.bit_i);
		}
	}
}

HTEST(String)
{
	uint8_t buf[7];
	struct BitPacker packer;
	char *string;
	int ret;

	bitpack_init(&packer, buf, sizeof buf);

	memset(buf, 0xff, sizeof buf);

	ret = bitpack_string(&packer, "abc");
	HTRY_I(4 * 8, ==, packer.bit_i);
	ret = bitpack_string(&packer, "de");
	HTRY_I(7 * 8, ==, packer.bit_i);
	HTRY_SIGNAL(ret = bitpack_string(&packer, "a"));
	HTRY_I(7 * 8, ==, packer.bit_i);

	packer.bit_i = 0;
	ret = bitunpack_string(&packer, &string);
	HTRY_I(4 * 8, ==, packer.bit_i);
	HTRY_STR("abc", ==, string);
	FREE(string);
	ret = bitunpack_string(&packer, &string);
	HTRY_I(7 * 8, ==, packer.bit_i);
	HTRY_STR("de", ==, string);
	FREE(string);
	HTRY_SIGNAL(ret = bitunpack_string(&packer, &string));
	HTRY_I(7 * 8, ==, packer.bit_i);
	HTRY_PTR(NULL, ==, string);
}

HTEST_SUITE(Bitpack)
{
	HTEST_ADD(Basic);
	HTEST_ADD(Uint32Limits);
	HTEST_ADD(Uint32);
	HTEST_ADD(FloatLimits);
	HTEST_ADD(Float);
	HTEST_ADD(String);
}
