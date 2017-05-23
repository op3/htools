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

HTEST(Strctv)
{
	char *cat;

	cat = STRCTV_BEGIN "This ", "is ", "a", NULL, " bunch", " of",
	    " strings." STRCTV_END;
	HTRY_STR("This is aNULL bunch of strings.", ==, cat);
	FREE(cat);
}

HTEST(Strecmp)
{
	char const s11[] = "abc";
	char const s12[] = "aabc";
	char const s13[] = "abcd";
	char const s14[] = "zabb";
	char const s15[] = "zabd";
	char const s2[] = "abc";

	HTRY_I(0, ==, strecmp(s11, s2));
	HTRY_I(0, ==, strecmp(s12, s2));
	HTRY_I(0, !=, strecmp(s13, s2));
	HTRY_I(0, >, strecmp(s14, s2));
	HTRY_I(0, <, strecmp(s15, s2));
}

HTEST(Strl)
{
	char s[3];
	size_t ret;

	s[0] = '\0';
	ret = strlcat(s, "a", sizeof s);
	HTRY_I(1, ==, ret);
	HTRY_STR("a", ==, s);
	ret = strlcat(s, "b", sizeof s);
	HTRY_I(2, ==, ret);
	HTRY_STR("ab", ==, s);
	ret = strlcat(s, "c", sizeof s);
	HTRY_I(3, ==, ret);
	HTRY_STR("ab", ==, s);

	s[0] = '\0';
	ret = strlcpy(s, "a", sizeof s);
	HTRY_I(1, ==, ret);
	HTRY_STR("a", ==, s);
	ret = strlcpy(s, "ab", sizeof s);
	HTRY_I(2, ==, ret);
	HTRY_STR("ab", ==, s);
	ret = strlcpy(s, "abc", sizeof s);
	HTRY_I(3, ==, ret);
	HTRY_STR("ab", ==, s);
}

HTEST_SUITE(String)
{
	HTEST_ADD(Strctv);
	HTEST_ADD(Strecmp);
	HTEST_ADD(Strl);
}
