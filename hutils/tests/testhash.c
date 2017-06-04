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

#include <hutils/hash.h>
#include <htest/htest.h>

HTEST(FNV1A)
{
	char data0[] = "abc0";
	char data1[] = "abc1";
	uint32_t hash0, hash1;

#ifndef NDEBUG
	HTRY_SIGNAL(hash0 = hutils_fnv1a32(data0, 0));
#endif
	hash0 = hutils_fnv1a32(data0, strlen(data0));
	hash1 = hutils_fnv1a32(data1, strlen(data1));
	HTRY_I(hash0, !=, hash1);
}

HTEST_SUITE(Hash)
{
	HTEST_ADD(FNV1A);
}
