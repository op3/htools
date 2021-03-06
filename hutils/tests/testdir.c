/*
 * Copyright (c) 2015-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
#include <hutils/memory.h>

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
	int num, ret;

	dir = dir_open("tests");
	HTRY_PTR(NULL, !=, dir);
	/* Check that we can find a few files. */
	num = 0;
	for (;;) {
		ret = dir_get(dir, &entry);
		if (0 == ret) {
			HTRY_PTR(NULL, ==, entry.name);
			break;
		}
		HTRY_PTR(NULL, !=, entry.name);
		++num;
	}
	HTRY_I(5, <, num);
	/* Check that the fail status keeps coming back. */
	for (num = 0; 5 > num; ++num) {
		ret = dir_get(dir, &entry);
		HTRY_I(0, ==, ret);
	}
	dir_close(&dir);
	HTRY_PTR(NULL, ==, dir);
	dir_close(&dir);
	HTRY_PTR(NULL, ==, dir);
}

HTEST(Names)
{
	char *dir, *base, *ext;

	dir_getnames("dir/file.txt", &dir, NULL);
	HTRY_STR("dir", ==, dir); FREE(dir);

	dir_getnames("dir/file.txt", NULL, &base);
	HTRY_STR("file.txt", ==, base); FREE(base);

	dir_getnames("dir/file.txt", &dir, &base);
	HTRY_STR("dir", ==, dir); FREE(dir);
	HTRY_STR("file.txt", ==, base); FREE(base);

	dir_getnames("", &dir, &base);
	HTRY_STR(".", ==, dir); FREE(dir);
	HTRY_STR("", ==, base); FREE(base);

	dir_getnames("dir/", &dir, &base);
	HTRY_STR("dir", ==, dir); FREE(dir);
	HTRY_STR("", ==, base); FREE(base);

	dir_getnames("file.txt", &dir, &base);
	HTRY_STR(".", ==, dir); FREE(dir);
	HTRY_STR("file.txt", ==, base); FREE(base);

	dir_getnames("/file.txt", &dir, &base);
	HTRY_STR("", ==, dir); FREE(dir);
	HTRY_STR("file.txt", ==, base); FREE(base);

	dir_getnames("/dir/file.txt", &dir, &base);
	HTRY_STR("/dir", ==, dir); FREE(dir);
	HTRY_STR("file.txt", ==, base); FREE(base);

	dir_getnames("/ugly.dir/file.txt", &dir, &base);
	HTRY_STR("/ugly.dir", ==, dir); FREE(dir);
	HTRY_STR("file.txt", ==, base); FREE(base);

	ext = dir_substext("dir", "coo");
	HTRY_STR("dir", ==, ext); FREE(ext);

	ext = dir_substext("file.txt", "coo");
	HTRY_STR("file.coo", ==, ext); FREE(ext);

	ext = dir_substext("dir/file.txt", "coo");
	HTRY_STR("dir/file.coo", ==, ext); FREE(ext);

	ext = dir_substext("ugly.dir/file.txt", "coo");
	HTRY_STR("ugly.dir/file.coo", ==, ext); FREE(ext);

	ext = dir_substext("ugly.dir/file", "coo");
	HTRY_STR("ugly.dir/file", ==, ext); FREE(ext);
}

HTEST_SUITE(Dir)
{
	HTEST_ADD(InvalidDir);
	HTEST_ADD(ListTests);
	HTEST_ADD(Names);
}
