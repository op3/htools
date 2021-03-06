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

#include <hutils/utf8.h>
#include <htest/htest.h>

static int	mygets(uint8_t *, size_t *, FILE *);

static struct {
	int	line_no;
	int	replacement_num;
} const c_test_invalid[] = {
	/* 5-/6-byte seqs were fine in 2002, but not in 2003-. */
	{ 75, 5}, { 76, 6}, { 84, 5}, { 85, 6},
	/* Single continuation bytes. */
	{102, 1}, {103, 1},
	/* Several continuation bytes. */
	{105, 2}, {106, 3}, {107, 4}, {108, 5}, {109, 6}, {110, 7},
	/* All continutation bytes. */
	{114, 16}, {115, 16}, {116, 16}, {117, 16},
	/* Lonely start bytes. */
	{124, 16}, {125, 16}, {130, 16}, {135, 8}, {140, 4}, {145, 2},
	/* Multi-byte replacement. */
	{153, 1}, {154, 2}, {155, 3}, {156, 4}, {157, 5},
	{158, 1}, {159, 2}, {160, 3}, {161, 4}, {162, 5},
	/* Many incomplete seqs. */
	{169, 30},
	/* Invalid bytes. */
	{175, 1}, {176, 1}, {177, 4},
	/* Overlong. */
	{207, 2}, {208, 3}, {209, 4}, {210, 5}, {211, 6},
	/* Max overlong. */
	{220, 2}, {221, 3}, {222, 4}, {223, 5}, {224, 6},
	/* NUL overlong. */
	{232, 2}, {233, 3}, {234, 4}, {235, 5}, {236, 6},
	/* UTF-16 surrogates. */
	{247, 3}, {248, 3}, {249, 3}, {250, 3}, {251, 3}, {252, 3}, {253, 3},
	/* Paired UTF-16 surrogates. */
	{257, 6}, {258, 6}, {259, 6}, {260, 6}, {261, 6}, {262, 6}, {263, 6},
	{264, 6}
	/* The rest is legal and not the responsibility of this decoder. */
};

int
mygets(uint8_t *a_dst, size_t *a_bytes, FILE *a_file)
{
	size_t i;
	int c;

	for (i = 0;; ++i) {
		c = fgetc(a_file);
		if (EOF == c || '\n' == c) {
			break;
		}
		if (*a_bytes > i) {
			a_dst[i] = (uint8_t)c;
		}
	}
	*a_bytes = i;
	return EOF != c || 0 != i;
}

HTEST(Basic)
{
	char const c_str[] = "This is a boring but satisfying test.";
	struct UTF8 *utf8;
	size_t code_bytes, str_len;
	uint32_t code;

	str_len = strlen(c_str);
	/* ASCII string test. */
	utf8 = utf8_create((uint8_t const *)c_str, str_len);
	HTRY_I(str_len, ==, utf8->length);
	HTRY_I(str_len, ==, utf8->bytes);
	HTRY_I('\0', ==, utf8->data[utf8->bytes]);
	/* Range test. */
	utf8_get(utf8, 0, &code, &code_bytes);
	utf8_get(utf8, 36, &code, &code_bytes);
	HTRY_SIGNAL(utf8_get(utf8, 37, &code, &code_bytes));
	utf8_free(&utf8);
}

HTEST(Test)
{
	FILE *file;
	int line_no;

	file = fopen("tests/UTF-8-test.txt", "rb");
	HTRY_PTR(NULL, !=, file);
	for (line_no = 1;; ++line_no) {
		uint8_t line[256];
		struct UTF8 *utf8, *utf8_again;
		size_t i;

		i = sizeof line;
		if (!mygets(line, &i, file)) {
			break;
		}
		utf8 = utf8_create(line, i);
		for (i = 0; LENGTH(c_test_invalid) > i; ++i) {
			if (c_test_invalid[i].line_no == line_no) {
				break;
			}
		}
		if (LENGTH(c_test_invalid) > i) {
			HTRY_I(c_test_invalid[i].replacement_num, ==,
			    utf8->replacement_num);
		} else {
			HTRY_I(0, ==, utf8->replacement_num);
		}

		utf8_again = utf8_create(utf8->data, utf8->bytes);
		HTRY_I(0, ==, utf8_again->replacement_num);
		utf8_free(&utf8_again);
		utf8_free(&utf8);
	}
	HTRY_I(301, ==, line_no);
	fclose(file);
}

HTEST(Demo)
{
	FILE *file;
	int line_no;

	file = fopen("tests/UTF-8-demo.txt", "rb");
	HTRY_PTR(NULL, !=, file);
	for (line_no = 1;; ++line_no) {
		uint8_t line[256];
		struct UTF8 *utf8, *utf8_again;
		size_t i;

		i = sizeof line;
		if (!mygets(line, &i, file)) {
			break;
		}
		line[i] = '\0';
		utf8 = utf8_create(line, i);
		HTRY_I(0, ==, utf8->replacement_num);

		utf8_again = utf8_create(utf8->data, utf8->bytes);
		HTRY_I(0, ==, utf8_again->replacement_num);
		utf8_free(&utf8_again);
		utf8_free(&utf8);
	}
	HTRY_I(213, ==, line_no);
	fclose(file);
}

HTEST(ParseOk)
{
	uint8_t const c_str[] = {
		0x40,
		0xd0, 0x80,
		0xe8, 0x80, 0x80,
		0xf4, 0x80, 0x80, 0x80
	};
	struct UTF8 *utf8;
	uint32_t code;
	size_t bytes, ofs;

	utf8 = utf8_create(c_str, sizeof c_str);
	HTRY_I(4, ==, utf8->length);
	HTRY_I(sizeof c_str, ==, utf8->bytes);
	ofs = 0;
	utf8_get(utf8, ofs, &code, &bytes);
	HTRY_I(0x40, ==, code);
	HTRY_I(1, ==, bytes);
	ofs += bytes;
	utf8_get(utf8, ofs, &code, &bytes);
	HTRY_I(0x400, ==, code);
	HTRY_I(2, ==, bytes);
	ofs += bytes;
	utf8_get(utf8, ofs, &code, &bytes);
	HTRY_I(0x8000, ==, code);
	HTRY_I(3, ==, bytes);
	ofs += bytes;
	utf8_get(utf8, ofs, &code, &bytes);
	HTRY_I(0x100000, ==, code);
	HTRY_I(4, ==, bytes);
	ofs += bytes;
	HTRY_I(1 + 2 + 3 + 4, ==, ofs);
	HTRY_I('\0', ==, utf8->data[ofs]);
	utf8_free(&utf8);
}

HTEST(ParseBad)
{
	uint8_t const c_str[] = {
		0x80,
		0xc0, 0x80,
		0xe0, 0x80, 0x80,
		0xf0, 0x80, 0x80, 0x80
	};
	struct UTF8 *utf8;
	size_t ofs, i;

	utf8 = utf8_create(c_str, sizeof c_str);
	HTRY_I(sizeof c_str, ==, utf8->length);
	HTRY_I(3 * sizeof c_str, ==, utf8->bytes);
	ofs = 0;
	for (i = 0; i < sizeof c_str; ++i) {
		uint32_t code;
		size_t bytes;

		utf8_get(utf8, ofs, &code, &bytes);
		HTRY_I(0xfffd, ==, code);
		HTRY_I(3, ==, bytes);
		ofs += bytes;
	}
	HTRY_I('\0', ==, utf8->data[ofs]);
	utf8_free(&utf8);
}

HTEST(Concat)
{
	/* Swedish character, rather common. */
	uint8_t const c_s0[] = {0xc3, 0xa5};
	/* Tibetan 'olympic flame' character, pretty. */
	uint8_t const c_s1[] = {0xe0, 0xbc, 0x86};
	struct UTF8 *u0, *u1, *u2;

	u0 = utf8_create(c_s0, 2);
	u1 = utf8_create(c_s1, 3);
	u2 = utf8_concat(u0, u1);
	HTRY_I(0, ==, u2->replacement_num);
	HTRY_I(1 + 1, ==, u2->length);
	HTRY_I(2 + 3, ==, u2->bytes);
	HTRY_I(c_s0[0], ==, u2->data[0]);
	HTRY_I(c_s0[1], ==, u2->data[1]);
	HTRY_I(c_s1[0], ==, u2->data[2]);
	HTRY_I(c_s1[1], ==, u2->data[3]);
	HTRY_I(c_s1[2], ==, u2->data[4]);
	HTRY_I(0, ==, u2->data[5]);
	utf8_free(&u0);
	utf8_free(&u1);
	utf8_free(&u2);
}

HTEST_SUITE(UTF8)
{
	HTEST_ADD(Basic);
	HTEST_ADD(Test);
	HTEST_ADD(Demo);
	HTEST_ADD(ParseOk);
	HTEST_ADD(ParseBad);
	HTEST_ADD(Concat);
}
