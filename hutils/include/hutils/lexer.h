/*
 * Copyright (c) 2015-2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#ifndef HUTILS_LEXER_H
#define HUTILS_LEXER_H

#include <stdlib.h>
#include <hutils/cdecls.h>
#include <hutils/funcattr.h>

CDECLS_BEGIN

enum LexerError {
	LEXER_ERROR_NONE,
	LEXER_ERROR_INVALID_HEX,
	LEXER_ERROR_INVALID_NUMBER,
	LEXER_ERROR_UNTERMINATED_LITERAL
};
enum LexerType {
	LEXER_ALNUM,
	LEXER_EOF,
	LEXER_ERROR,
	LEXER_HEX,
	LEXER_LITERAL,
	LEXER_NUMBER,
	LEXER_SYMBOL
};

typedef size_t (*LexerCallback)(void *, char *, size_t);

struct Lexer;
struct LexerToken {
	enum	LexerType type;
	char	*str;
};

struct Lexer	*lexer_create(LexerCallback, void *) FUNC_RETURNS;
size_t		lexer_cstr_callback(void *, char *, size_t) FUNC_RETURNS;
int		lexer_expect_symbol(struct Lexer *, char) FUNC_RETURNS;
void		lexer_free(struct Lexer **);
int		lexer_get_col_no(struct Lexer const *) FUNC_PURE FUNC_RETURNS;
enum LexerError	lexer_get_error(struct Lexer const *) FUNC_RETURNS;
int		lexer_get_line_no(struct Lexer const *) FUNC_PURE
FUNC_RETURNS;
int		lexer_token_get(struct Lexer *, struct LexerToken *)
	FUNC_RETURNS;

CDECLS_END

#endif
