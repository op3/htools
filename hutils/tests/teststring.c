/*
 * Copyright (c) 2016-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
	char *cat, *ret;

	cat = NULL;
	ret = STRCTV_BEGIN &cat, "This ", "is " STRCTV_END;
	HTRY_STR("This is ", ==, cat);
	HTRY_STR(ret, ==, cat);
	ret = STRCTV_BEGIN &cat, "a", NULL, " bunch", " of", " strings."
	    STRCTV_END;
	HTRY_STR("This is aNULL bunch of strings.", ==, cat);
	HTRY_STR(ret, ==, cat);
	FREE(cat);
}

HTEST(Strbcmp)
{
	HTRY_I(strbcmp("abc", ""),    ==, 0);
	HTRY_I(strbcmp("abc", "a"),   ==, 0);
	HTRY_I(strbcmp("abc", "ab"),  ==, 0);
	HTRY_I(strbcmp("abc", "abc"), ==, 0);
	HTRY_I(strbcmp("abc", "abcd"), <, 0);
	HTRY_I(strbcmp("abc", "abb"),  >, 0);
	HTRY_I(strbcmp("abc", "abd"),  <, 0);
}

HTEST(Strecmp)
{
	HTRY_I(strecmp("abc",  ""),    ==, 0);
	HTRY_I(strecmp("abc",  "c"),   ==, 0);
	HTRY_I(strecmp("abc",  "bc"),  ==, 0);
	HTRY_I(strecmp("abc",  "abc"), ==, 0);
	HTRY_I(strecmp("abcd", "abc"),  >, 0);
	HTRY_I(strecmp("zabb", "abc"),  <, 0);
	HTRY_I(strecmp("zabd", "abc"),  >, 0);
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
	HTEST_ADD(Strbcmp);
	HTEST_ADD(Strecmp);
	HTEST_ADD(Strl);
}
