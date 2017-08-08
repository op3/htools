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

#include <hutils/lexer.h>
#include <htest/htest.h>
#include <hutils/memory.h>

HTEST(BufMajeur)
{
	int const c_n = 10;
	struct LexerToken token;
	struct Lexer *lexer;
	char *text, *p;
	size_t ofs;
	int i;

	text = malloc(c_n * ('[' - 'A' + 1) + 1);
	/* Fill with cycles of capitals and '['. */
	ofs = 0;
	for (i = 0; c_n > i; ++i) {
		char j;

		for (j = 'A'; '[' >= j; ++j) {
			text[ofs++] = j;
		}
	}
	text[ofs] = '\0';
	p = text;
	lexer = lexer_create(lexer_cstr_callback, &p);
	for (i = 0; c_n > i; ++i) {
		HTRY_BOOL(lexer_token_get(lexer, &token));
		HTRY_I(LEXER_ALNUM, ==, token.type);
		HTRY_PTR(NULL, !=, token.str);
		free(token.str);
		HTRY_BOOL(lexer_token_get(lexer, &token));
		HTRY_I(LEXER_SYMBOL, ==, token.type);
		HTRY_STR("[", ==, token.str);
		free(token.str);
	}
	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);
	HTRY_PTR(NULL, ==, token.str);
	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);
	HTRY_PTR(NULL, ==, token.str);

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
	lexer = lexer_create(lexer_cstr_callback, (void *)&p);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("a", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(2, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("A", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(4, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("_a", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(7, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("a_", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(10, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("a0", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(13, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("0", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(15, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("a", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(16, ==, lexer->col_no);

	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);
	HTRY_PTR(NULL, ==, token.str);
	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);
	HTRY_PTR(NULL, ==, token.str);

	lexer_free(&lexer);
}

HTEST(HexVariations)
{
	char const c_text[] = "0x0 0xa 0xA 0x";
	struct LexerToken token;
	struct Lexer *lexer;
	char const *p;

	p = c_text;
	lexer = lexer_create(lexer_cstr_callback, (void *)&p);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_HEX, ==, token.type);
	HTRY_STR("0x0", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(4, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_HEX, ==, token.type);
	HTRY_STR("0xa", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(8, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_HEX, ==, token.type);
	HTRY_STR("0xA", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(12, ==, lexer->col_no);

	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ERROR, ==, token.type);
	HTRY_PTR(NULL, ==, token.str);
	HTRY_I(LEXER_ERROR_INVALID_HEX, ==, lexer->error);

	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);
	HTRY_PTR(NULL, ==, token.str);

	lexer_free(&lexer);
}

HTEST(NumberVariations)
{
	char const c_text[] = "1 -1. +.1 1e1 1.e+1 .1e-1 1ea";
	struct LexerToken token;
	struct Lexer *lexer;
	char const *p;

	p = c_text;
	lexer = lexer_create(lexer_cstr_callback, (void *)&p);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("1", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(2, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("-1.", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(6, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("+.1", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(10, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("1e1", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(14, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("1.e+1", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(20, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR(".1e-1", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(26, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("1", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(28, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("ea", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(30, ==, lexer->col_no);

	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);
	HTRY_PTR(NULL, ==, token.str);

	lexer_free(&lexer);
}

HTEST(LiteralVariations)
{
	char const c_text1[] = "\"text";
	char const c_text2[] = "\"text\"";
	struct LexerToken token;
	struct Lexer *lexer;
	char const *p;

	p = c_text1;
	lexer = lexer_create(lexer_cstr_callback, (void *)&p);
	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ERROR, ==, token.type);
	HTRY_PTR(NULL, ==, token.str);
	HTRY_I(LEXER_ERROR_UNTERMINATED_LITERAL, ==, lexer->error);
	lexer_free(&lexer);

	p = c_text2;
	lexer = lexer_create(lexer_cstr_callback, (void *)&p);
	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_LITERAL, ==, token.type);
	HTRY_STR("text", ==, token.str);
	FREE(token.str);
	lexer_free(&lexer);
}

HTEST(UglyText)
{
	char const c_text[] = "_alnum0x1 \t 0x2\n\"literal\n0x1+\"+0.1e-1%";
	struct LexerToken token;
	struct Lexer *lexer;
	char const *p;

	p = c_text;
	lexer = lexer_create(lexer_cstr_callback, (void *)&p);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_ALNUM, ==, token.type);
	HTRY_STR("_alnum0x1", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(10, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_HEX, ==, token.type);
	HTRY_STR("0x2", ==, token.str);
	FREE(token.str);
	HTRY_I(1, ==, lexer->line_no);
	HTRY_I(16, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_LITERAL, ==, token.type);
	HTRY_STR("literal\n0x1+", ==, token.str);
	FREE(token.str);
	HTRY_I(3, ==, lexer->line_no);
	HTRY_I(6, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_NUMBER, ==, token.type);
	HTRY_STR("+0.1e-1", ==, token.str);
	FREE(token.str);
	HTRY_I(3, ==, lexer->line_no);
	HTRY_I(13, ==, lexer->col_no);

	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_I(LEXER_SYMBOL, ==, token.type);
	HTRY_STR("%", ==, token.str);
	FREE(token.str);
	HTRY_I(3, ==, lexer->line_no);
	HTRY_I(14, ==, lexer->col_no);

	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_I(LEXER_EOF, ==, token.type);
	HTRY_PTR(NULL, ==, token.str);

	lexer_free(&lexer);
}

HTEST(Skip)
{
	char const c_text[] = "Line1\nLine2";
	struct LexerToken token;
	struct Lexer *lexer;
	char const *p;

	p = c_text;
	lexer = lexer_create(lexer_cstr_callback, (void *)&p);
	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_STR("Line1", ==, token.str);
	free(token.str);
	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_STR("Line2", ==, token.str);
	free(token.str);
	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_PTR(NULL, ==, token.str);
	lexer_free(&lexer);

	p = c_text;
	lexer = lexer_create(lexer_cstr_callback, (void *)&p);
	HTRY_BOOL(lexer_skip(lexer, '\n'));
	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_STR("Line2", ==, token.str);
	free(token.str);
	HTRY_BOOL(!lexer_skip(lexer, '\n'));
	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_PTR(NULL, ==, token.str);
	lexer_free(&lexer);

	p = c_text;
	lexer = lexer_create(lexer_cstr_callback, (void *)&p);
	HTRY_BOOL(lexer_skip(lexer, 'e'));
	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_STR("1", ==, token.str);
	free(token.str);
	HTRY_BOOL(lexer_token_get(lexer, &token));
	HTRY_STR("Line2", ==, token.str);
	free(token.str);
	HTRY_BOOL(!lexer_skip(lexer, '\n'));
	HTRY_BOOL(!lexer_token_get(lexer, &token));
	HTRY_PTR(NULL, ==, token.str);
	lexer_free(&lexer);
}

HTEST_SUITE(Lexer)
{
	HTEST_ADD(BufMajeur);
	HTEST_ADD(AlnumVariations);
	HTEST_ADD(HexVariations);
	HTEST_ADD(NumberVariations);
	HTEST_ADD(LiteralVariations);
	HTEST_ADD(UglyText);
	HTEST_ADD(Skip);
}
