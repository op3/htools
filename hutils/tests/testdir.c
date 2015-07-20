/*
 * Copyright (c) 2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/dir.h>
#include <htest/htest.h>

HTEST(InvalidDir)
{
	struct Dir *dir;

	dir = dir_open("patatos");
	HTRY_PTR(NULL, ==, dir);
}

HTEST(ListTests)
{
	struct DirEntry entry;
	struct Dir *dir;
	int ret;

	dir = dir_open("tests");
	HTRY_PTR(NULL, !=, dir);
	ret = dir_get(dir, &entry);
	HTRY_I(1, ==, ret);
	dir_close(&dir);
	HTRY_PTR(NULL, ==, dir);
}

HTEST_SUITE(Dir)
{
	HTEST_ADD(InvalidDir);
	HTEST_ADD(ListTests);
}
