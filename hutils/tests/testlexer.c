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

#include <hutils/lexer.h>
#include <htest/htest.h>
#include <hutils/memory.h>

static size_t	my_read(void *, char *, size_t) FUNC_RETURNS;

size_t
my_read(void *const a_data, char *const a_dst, size_t const a_max)
{
	char *p;
	size_t len;

	p = *(char **)a_data;
	len = strlen(p);
	len = MIN(a_max, len);
	memmove(a_dst, p, len);
	return len;
}

HTEST(BufMajeur)
{
	struct LexerToken token;
	struct Lexer *lexer;
	char *text;
	int i;

	MALLOC(text, 10000);
	for (i = 0; 10000 > i; ++i) {
		/* Fill with cycles of capitals and '['. */
		text[i] = 'A' + i % ('Z' - 'A' + 2);
	}
	lexer = lexer_create(my_read, &text);

	for (i = 0; 37 > i; ++i) {
		HTRY_I(0, !=, lexer_token_get(lexer, &token));
		HTRY_I(LEXER_ALNUM, ==, token.type);
		HTRY_I(0, !=, lexer_token_get(lexer, &token));
		HTRY_I(LEXER_SYMBOL, ==, token.type);
	}

	lexer_free(&lexer);
	FREE(text);
}

HTEST(AlnumVariations)
{
	char const c_text[] = "a A _a a_ a0 0a";
	struct LexerToken token;
	struct Lexer *lexer;
	char const *p;

	p = c_text;
	lexer = lexer_create(my_read, &p);

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("a", ==, token.str);
	FREE(token.str);
	HTRY_I(2, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("A", ==, token.str);
	FREE(token.str);
	HTRY_I(4, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("_a", ==, token.str);
	FREE(token.str);
	HTRY_I(7, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("a_", ==, token.str);
	FREE(token.str);
	HTRY_I(10, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("a0", ==, token.str);
	FREE(token.str);
	HTRY_I(13, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("0", ==, token.str);
	FREE(token.str);
	HTRY_I(15, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("a", ==, token.str);
	FREE(token.str);
	HTRY_I(16, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, ==, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);
	HTRY_I(0, ==, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);

	lexer_free(&lexer);
}

HTEST(HexVariations)
{
	char const c_text[] = "0x0 0xa 0xA 0x";
	struct LexerToken token;
	struct Lexer *lexer;
	char const *p;

	p = c_text;
	lexer = lexer_create(my_read, &p);

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_HEX, ==, token.type);
	HTRY_STR("0x0", ==, token.str);
	FREE(token.str);
	HTRY_I(4, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_HEX, ==, token.type);
	HTRY_STR("0xa", ==, token.str);
	FREE(token.str);
	HTRY_I(8, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_HEX, ==, token.type);
	HTRY_STR("0xA", ==, token.str);
	FREE(token.str);
	HTRY_I(12, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, ==, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ERROR, ==, token.type);
	HTRY_I(LEXER_ERROR_INVALID_HEX, ==, lexer_get_error(lexer));

	HTRY_I(0, ==, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);

	lexer_free(&lexer);
}

HTEST(NumberVariations)
{
	char const c_text[] = "1 -1. +.1 1e1 1.e+1 .1e-1";
	struct LexerToken token;
	struct Lexer *lexer;
	char const *p;

	p = c_text;
	lexer = lexer_create(my_read, &p);

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("1", ==, token.str);
	FREE(token.str);
	HTRY_I(2, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("-1.", ==, token.str);
	FREE(token.str);
	HTRY_I(6, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("+.1", ==, token.str);
	FREE(token.str);
	HTRY_I(10, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("1e1", ==, token.str);
	FREE(token.str);
	HTRY_I(14, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("1.e+1", ==, token.str);
	FREE(token.str);
	HTRY_I(20, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR(".1e-1", ==, token.str);
	FREE(token.str);
	HTRY_I(26, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, ==, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);

	lexer_free(&lexer);
}

HTEST(UglyText)
{
	char const c_text[] = "_alnum0x1 \t 0x2\n\"literal\n0x1+\"+0.1e-1%";
	struct LexerToken token;
	struct Lexer *lexer;
	char const *p;

	p = c_text;
	lexer = lexer_create(my_read, &p);

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("_alnum0x1", ==, token.str);
	FREE(token.str);
	HTRY_I(10, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_HEX, ==, token.type);
	HTRY_STR("0x2", ==, token.str);
	FREE(token.str);
	HTRY_I(16, ==, lexer_get_col_no(lexer));
	HTRY_I(1, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_LITERAL, ==, token.type);
	HTRY_STR("literal\n0x1+", ==, token.str);
	FREE(token.str);
	HTRY_I(6, ==, lexer_get_col_no(lexer));
	HTRY_I(3, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("+0.1e-1", ==, token.str);
	FREE(token.str);
	HTRY_I(13, ==, lexer_get_col_no(lexer));
	HTRY_I(3, ==, lexer_get_line_no(lexer));

	HTRY_I(0, !=, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_SYMBOL, ==, token.type);
	HTRY_STR("%", ==, token.str);
	FREE(token.str);
	HTRY_I(14, ==, lexer_get_col_no(lexer));
	HTRY_I(3, ==, lexer_get_line_no(lexer));

	HTRY_I(0, ==, lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);

	lexer_free(&lexer);
}

HTEST_SUITE(Lexer)
{
	HTEST_ADD(BufMajeur);
	HTEST_ADD(AlnumVariations);
	HTEST_ADD(HexVariations);
	HTEST_ADD(NumberVariations);
	HTEST_ADD(UglyText);
}
