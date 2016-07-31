/*
 * Copyright (c) 2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>

#define LENGTH(arr) (sizeof(arr) / sizeof *(arr))
#define STR_SIZ 1024
#define STRBCMP(str, ptn) strncmp(str, ptn, sizeof ptn - 1)
#define STRECMP(str, ptn) strcmp(str + strlen(str) - sizeof ptn + 1, ptn)
#define STRCTV_BEGIN strctv_(
#define STRCTV_END ,strctv_sentinel_)

enum VariableEnum {
	VAR_CC = 0,
	VAR_CPPFLAGS,
	VAR_CFLAGS,
	VAR_LDFLAGS,
	VAR_LIBS,
	VAR_OUTPUT_NUM,
	VAR_SRC,
	VAR_DOLINK,
	VAR_INPUT_NUM
};

struct Bucket {
	char	var[VAR_INPUT_NUM][STR_SIZ];
	int	do_link;
};

extern char const *strctv_sentinel_;

void	cat_str(char *, char const *, size_t);
void	err_(int, char const *, ...);
void	errx_(int, char const *, ...);
void	merge(struct Bucket *, int, char const *const *);
char	*strctv_(char const *, ...);
char	*strndup_(char const *, size_t);

#endif
