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
#include <hutils/assert.h>
#include <hutils/err.h>
#include <hutils/memory.h>

/* TODO: Use 32-bit packs/unpacks rather than 8? */

void
bitpack_init(struct BitPacker *a_packer, uint8_t *a_buf, size_t a_bytes)
{
	a_packer->size = a_bytes;
	a_packer->buf = a_buf;
	a_packer->bit_i = 0;
}

int
bitpack_float(struct BitPacker *a_packer, float a_float, unsigned
    a_exponent_bits, unsigned a_mantissa_bits)
{
	union {
		uint32_t	u32;
		float	flt;
	} u;
	uint32_t mantissa, sign, uflt;
	int exponent;

	if (a_exponent_bits < 2 || 8 < a_exponent_bits) {
		hutils_errx(EXIT_FAILURE, "bitpack_float invalid # exponent "
		    "bits = %d != {2..8}.", a_exponent_bits);
	}
	if (a_mantissa_bits < 1 || 23 < a_mantissa_bits) {
		hutils_errx(EXIT_FAILURE, "bitpack_float invalid # mantissa "
		    "bits = %d != {2..8}.", a_mantissa_bits);
	}
	u.flt = a_float;
	/* Check valid ranges? */
	sign = u.u32 >> 31;
	exponent = (0xff & (u.u32 >> 23))
	    - 128 + (1 << (a_exponent_bits - 1));
	mantissa = 0x7fffff & u.u32;
	uflt =
	    sign << (a_exponent_bits + a_mantissa_bits) |
	    exponent << a_mantissa_bits |
	    mantissa >> (23 - a_mantissa_bits);
	return bitpack_uint32(a_packer, uflt, 1 + a_exponent_bits +
	    a_mantissa_bits);
}

int
bitpack_string(struct BitPacker *a_packer, char const *a_s)
{
	char const *p;

	for (p = a_s; '\0' != *p; ++p) {
		if (!bitpack_uint32(a_packer, *p, 8)) {
			return 0;
		}
	}
	return bitpack_uint32(a_packer, 0, 8);
}

int
bitpack_uint32(struct BitPacker *a_packer, uint32_t a_u32, unsigned a_bits)
{
	if (0 == a_bits || 32 < a_bits) {
		hutils_errx(EXIT_FAILURE, "bitpack_uint32 invalid # bits = "
		    "%d != {1..32}.", a_bits);
	}
	if (a_packer->bit_i + a_bits > 8 * a_packer->size) {
		hutils_errx(EXIT_FAILURE, "bitpack_uint32 overflow.");
	}
	do {
		unsigned avail, ofs, shift;

		shift = 7 & a_packer->bit_i;
		ofs = a_packer->bit_i / 8;
		if (0 == shift) {
			a_packer->buf[ofs] = 0;
		}
		avail = 8 - shift;
		avail = MIN(avail, a_bits);
		a_packer->buf[ofs] |= (((1 << avail) - 1) & a_u32) << shift;
		a_packer->bit_i += avail;
		a_u32 >>= avail;
		a_bits -= avail;
	} while (0 < a_bits);
	return 1;
}

int
bitunpack_float(struct BitPacker *a_packer, float *a_float, unsigned
    a_exponent_bits, unsigned a_mantissa_bits)
{
	union {
		uint32_t	u32;
		float	flt;
	} u;
	uint32_t exponent, mantissa, sign;

	if (a_exponent_bits < 2 || 8 < a_exponent_bits) {
		hutils_errx(EXIT_FAILURE, "bitpack_float invalid # exponent "
		    "bits = %d != {2..8}.", a_exponent_bits);
	}
	if (a_mantissa_bits < 1 || 23 < a_mantissa_bits) {
		hutils_errx(EXIT_FAILURE, "bitpack_float invalid # mantissa "
		    "bits = %d != {2..8}.", a_mantissa_bits);
	}
	if (!bitunpack_uint32(a_packer, &u.u32, 1 + a_exponent_bits +
	    a_mantissa_bits)) {
		return 0;
	}
	sign = u.u32 >> (a_exponent_bits + a_mantissa_bits);
	exponent = ((u.u32 >> a_mantissa_bits) & ((1 << a_exponent_bits) - 1))
	    - (1 << (a_exponent_bits - 1)) + 128;
	mantissa = u.u32 & ((1 << a_mantissa_bits) - 1);
	u.u32 =
	    sign << 31 |
	    exponent << 23 |
	    mantissa << (23 - a_mantissa_bits);
	*a_float = u.flt;
	return 1;
}

int
bitunpack_string(struct BitPacker *a_packer, char **a_string)
{
	char *s;
	size_t len, bit_i_old;

	bit_i_old = a_packer->bit_i;
	for (len = 0;; ++len) {
		uint32_t c;

		if (!bitunpack_uint32(a_packer, &c, 8)) {
			a_packer->bit_i = bit_i_old;
			return 0;
		}
		if (0 == c) {
			break;
		}
	}
	MALLOC(s, len + 1);
	*a_string = s;
	a_packer->bit_i = bit_i_old;
	for (;;) {
		uint32_t c;

		if (!bitunpack_uint32(a_packer, &c, 8)) {
			hutils_errx(EXIT_FAILURE, "This should not happen.");
		}
		*s++ = (uint8_t)c;
		if (0 == c) {
			break;
		}
	}
	return 1;
}

int
bitunpack_uint32(struct BitPacker *a_packer, uint32_t *a_u32, unsigned a_bits)
{
	uint32_t u32;
	unsigned i;

	if (0 == a_bits || 32 < a_bits) {
		hutils_errx(EXIT_FAILURE, "bitunpack_uint32 invalid # bits "
		    "= %d != {1..32}.", a_bits);
	}
	if (a_packer->bit_i + a_bits > 8 * a_packer->size) {
		hutils_errx(EXIT_FAILURE, "bitunpack_uint32 overflow.");
	}
	u32 = 0;
	for (i = 0; 0 < a_bits;) {
		unsigned avail, ofs, shift;
		uint8_t u8;

		shift = 7 & a_packer->bit_i;
		ofs = a_packer->bit_i / 8;
		avail = 8 - shift;
		avail = MIN(avail, a_bits);
		u8 = a_packer->buf[ofs];
		u8 = ((1 << avail) - 1) & (u8 >> shift);
		u32 |= u8 << i;
		a_packer->bit_i += avail;
		a_bits -= avail;
		i += avail;
	}
	*a_u32 = u32;
	return 1;
}
