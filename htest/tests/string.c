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

#include <htest/htest.h>

HTEST(SimpleStrings)
{
	char const c_hello[] = "Hello";

	HTRY_STR("Hello", ==, c_hello);
	HTRY_STR("abc", <, "def");
	HTRY_STR("def", >, "abc");
	HTRY_STRN("abc", ==, "abd", 1);
	HTRY_STRN("abc", ==, "abd", 2);
	HTRY_STRN("abc", !=, "abd", 3);
}

HTEST(NullStrings)
{
	HTRY_STR(NULL, ==, "a");
	HTRY_STR("a", ==, NULL);
	HTRY_STR(NULL, ==, NULL);
	HTRY_STRN(NULL, ==, "a", 0);
	HTRY_STRN("a", ==, NULL, 0);
	HTRY_STRN(NULL, ==, NULL, 0);
}

HTEST_SUITE(String)
{
	HTEST_ADD(SimpleStrings);
	HTEST_ADD(NullStrings);
}
