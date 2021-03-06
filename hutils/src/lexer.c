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
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <hutils/memory.h>

enum {
	PEEK_EOF = -1,
	PEEK_OVERFLOW = -2
};

static char	*extract(struct Lexer *, size_t) FUNC_RETURNS;
static int	peek_char(struct Lexer *, size_t) FUNC_RETURNS;
static size_t	stdio_fread(void *, char *, size_t) FUNC_RETURNS;

char *
extract(struct Lexer *a_lexer, size_t a_len)
{
	char *str;
	size_t i;

	str = malloc(a_len + 1);
	for (i = 0; a_len > i; ++i) {
		int c;

		c = peek_char(a_lexer, 0);
		assert(PEEK_OVERFLOW != c);
		if (PEEK_EOF == c) {
			break;
		}
		str[i] = (char)c;
		++a_lexer->ofs;
		if ('\n' == c) {
			++a_lexer->line_no;
			a_lexer->col_no = 1;
		} else {
			++a_lexer->col_no;
		}
	}
	str[i] = '\0';
	return str;
}

struct Lexer *
lexer_create(LexerCallback a_callback, void *a_callback_data)
{
	struct Lexer *lexer;

	CALLOC(lexer, 1);
	lexer->callback = NULL == a_callback ? stdio_fread : a_callback;
	lexer->callback_data = a_callback_data;
	lexer->error = LEXER_ERROR_NONE;
	lexer->line_no = 1;
	lexer->col_no = 1;
	return lexer;
}

/*
 * Read given number of chars from pointed to c-string and advances the
 * pointer, e.g.:
 *  char *str, *p;
 *  p = str = some_string();
 *  lexer_create(lexer_cstr_callback, &p);
 *  lexer_token_get(lexer, &token);
 *  assert(p != str);
 */
size_t
lexer_cstr_callback(void *a_src, char *a_dst, size_t a_dst_size)
{
	char **src;
	size_t len;

	src = a_src;
	len = strlen(*src);
	len = MIN(len, a_dst_size);
	memmove(a_dst, *src, len);
	*src += len;
	return len;
}

void
lexer_free(struct Lexer **a_lexer)
{
	struct Lexer *lexer;

	lexer = *a_lexer;
	if (NULL == lexer) {
		return;
	}
	FREE(*a_lexer);
}

int
lexer_skip(struct Lexer *a_lexer, char a_c)
{
	a_lexer->error = LEXER_ERROR_NONE;
	for (;;) {
		int c;

		c = peek_char(a_lexer, 0);
		if (PEEK_EOF == c) {
			return 0;
		}
		++a_lexer->ofs;
		++a_lexer->col_no;
		if ('\n' == c) {
			++a_lexer->line_no;
			a_lexer->col_no = 1;
		}
		if (a_c == c) {
			return 1;
		}
	}
}

int
lexer_token_get(struct Lexer *a_lexer, struct LexerToken *a_token)
{
	a_lexer->error = LEXER_ERROR_NONE;
	a_token->str = NULL;
	for (;;) {
		size_t i;
		int c, is_number;

		for (;;) {
			c = peek_char(a_lexer, 0);
			if (' ' != c && '\t' != c) {
				break;
			}
			++a_lexer->ofs;
			++a_lexer->col_no;
		}
		if ('#' == c) {
			do {
				++a_lexer->ofs;
				++a_lexer->col_no;
				c = peek_char(a_lexer, 0);
			} while (0 < c && '\n' != c);
		}
		if (PEEK_EOF == c) {
			a_token->type = LEXER_EOF;
			return 0;
		}
		if ('\n' == c) {
			++a_lexer->ofs;
			++a_lexer->line_no;
			a_lexer->col_no = 1;
			continue;
		}
		if ('_' == c || isalpha(c)) {
			size_t len;

			a_token->type = LEXER_ALNUM;
			for (len = 1; LENGTH(a_lexer->buf) > len; ++len) {
				c = peek_char(a_lexer, len);
				if ('_' != c && !isalnum(c)) {
					break;
				}
			}
			a_token->str = extract(a_lexer, len);
			return 1;
		}
		if ('"' == c) {
			size_t len;

			a_token->type = LEXER_LITERAL;
			++a_lexer->ofs;
			++a_lexer->col_no;
			for (len = 0; LENGTH(a_lexer->buf) > len; ++len) {
				c = peek_char(a_lexer, len);
				if (PEEK_EOF == c) {
					a_token->type = LEXER_ERROR;
					a_lexer->error =
					    LEXER_ERROR_UNTERMINATED_LITERAL;
					return 0;
				}
				if ('"' == c) {
					break;
				}
			}
			a_token->str = extract(a_lexer, len);
			++a_lexer->ofs;
			++a_lexer->col_no;
			return 1;
		}
		if ('0' == c && 'x' == peek_char(a_lexer, 1)) {
			size_t len;

			if (!isxdigit(peek_char(a_lexer, 2))) {
				a_token->type = LEXER_ERROR;
				a_lexer->error = LEXER_ERROR_INVALID_HEX;
				a_lexer->ofs += 2;
				a_lexer->col_no += 2;
				return 0;
			}
			a_token->type = LEXER_HEX;
			for (len = 3; LENGTH(a_lexer->buf) > len; ++len) {
				c = peek_char(a_lexer, len);
				if (!isxdigit(c)) {
					break;
				}
			}
			a_token->str = extract(a_lexer, len);
			return 1;
		}
		is_number = 0;
		i = 0;
		if ('-' == c || '+' == c) {
			++i;
		}
		if (isdigit(peek_char(a_lexer, i))) {
			is_number = 1;
			for (++i; isdigit(peek_char(a_lexer, i)); ++i)
				;
		}
		if ('.' == peek_char(a_lexer, i)) {
			++i;
		}
		if (isdigit(peek_char(a_lexer, i))) {
			is_number = 1;
			for (++i; isdigit(peek_char(a_lexer, i)); ++i)
				;
		}
		if (is_number) {
			a_token->type = LEXER_NUMBER;
			c = peek_char(a_lexer, i);
			if ('e' == c || 'E' == c) {
				int j;

				j = i + 1;
				c = peek_char(a_lexer, j);
				if ('+' == c || '-' == c) {
					++j;
				}
				if (isdigit(peek_char(a_lexer, j))) {
					for (i = j; isdigit(peek_char(a_lexer,
					    i)); ++i)
						;
				}
			}
			a_token->str = extract(a_lexer, i);
			return 1;
		}
		a_token->type = LEXER_SYMBOL;
		a_token->str = extract(a_lexer, 1);
		return 1;
	}
}

int
peek_char(struct Lexer *a_lexer, size_t a_ofs)
{
	size_t ofs;

	if (LENGTH(a_lexer->buf) <= a_ofs) {
		return PEEK_OVERFLOW;
	}
	ofs = a_lexer->ofs + a_ofs;
	if (ofs >= a_lexer->buf_end) {
		char buf[LENGTH(a_lexer->buf)];
		size_t buf_first, buf_last, len, ret;

		if (a_lexer->buf_ended) {
			return PEEK_EOF;
		}
		len = LENGTH(a_lexer->buf) - (a_lexer->buf_end -
		    a_lexer->ofs);
		ret = a_lexer->callback(a_lexer->callback_data, buf, len);
		if (0 == ret) {
			return PEEK_EOF;
		}
		buf_first = (LENGTH(a_lexer->buf) - 1) & a_lexer->buf_end;
		buf_last = (LENGTH(a_lexer->buf) - 1) & (a_lexer->buf_end +
		    len - 1);
		if (buf_first > buf_last) {
			size_t len1;

			len1 = LENGTH(a_lexer->buf) - buf_first;
			memmove(a_lexer->buf + buf_first, buf, len1);
			memmove(a_lexer->buf, buf + len1, len - len1);
		} else {
			memmove(a_lexer->buf + buf_first, buf, len);
		}
		a_lexer->buf_end += ret;
		if (ret < len) {
			a_lexer->buf_ended = 1;
		}
	}
	return a_lexer->buf[(LENGTH(a_lexer->buf) - 1) & ofs];
}

size_t
stdio_fread(void *a_user_data, char *a_dst, size_t a_max)
{
	return fread(a_dst, 1, a_max, a_user_data);
}
