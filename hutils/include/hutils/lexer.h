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

struct Lexer {
	LexerCallback	callback;
	void	*callback_data;
	enum	LexerError error;
	unsigned	int line_no;
	unsigned	int col_no;
	char	buf[256];
	size_t	ofs;
	size_t	buf_end;
	int	buf_ended;
};
struct LexerToken {
	enum	LexerType type;
	char	*str;
};

struct Lexer	*lexer_create(LexerCallback, void *) FUNC_RETURNS;
size_t		lexer_cstr_callback(void *, char *, size_t) FUNC_RETURNS;
void		lexer_free(struct Lexer **);
char const	*lexer_get_strerror(struct Lexer const *) FUNC_RETURNS;
int		lexer_skip(struct Lexer *, char);
int		lexer_token_get(struct Lexer *, struct LexerToken *)
	FUNC_RETURNS;

CDECLS_END

#endif
