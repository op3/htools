/*
 * Copyright (c) 2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/string.h>
#include <htest/htest.h>
#include <hutils/memory.h>

HTEST(Strcxt)
{
	char const *arr[] = {
		"This ",
		"is ",
		"a",
		" bunch",
		" of ",
		"strings."
	};
	char *cat;

	cat = STRCXT(arr);
	HTRY_STR("This is a bunch of strings.", ==, cat);
	FREE(cat);
}

HTEST(Strecmp)
{
	char const s11[] = "abc";
	char const s12[] = "aabc";
	char const s13[] = "abcd";
	char const s14[] = "abb";
	char const s15[] = "abd";
	char const s2[] = "abc";

	HTRY_I(0, ==, strecmp(s11, s2));
	HTRY_I(0, ==, strecmp(s12, s2));
	HTRY_I(0, !=, strecmp(s13, s2));
	HTRY_I(0, >, strecmp(s14, s2));
	HTRY_I(0, <, strecmp(s15, s2));
}

HTEST_SUITE(String)
{
	HTEST_ADD(Strcxt);
	HTEST_ADD(Strecmp);
}
