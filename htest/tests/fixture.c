/*
 * Copyright (c) 2014-2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <stdlib.h>
#include <htest/htest.h>

static int *g_mojo;

HTEST(SimpleFixture1)
{
	HTRY_U(*g_mojo, ==, 0x12345678);
	++(*g_mojo);
	HTRY_U(*g_mojo, ==, 0x12345679);
}

HTEST(SimpleFixture2)
{
	HTRY_U(*g_mojo, ==, 0x12345678);
}

HTEST_SUITE(SimpleFixture)
{
	g_mojo = malloc(sizeof *g_mojo);
	*g_mojo = 0x12345678;

	HTEST_ADD(SimpleFixture1);
	HTEST_ADD(SimpleFixture2);

	free(g_mojo);
}
