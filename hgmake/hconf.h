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

#ifndef HCONF_H
#define HCONF_H

#define LENGTH(arr) (sizeof arr / sizeof *arr)
#define STR_SIZ 1024
#define STRBCMP(s, p) strncmp(s, p, sizeof p - 1)

enum VarName {
	/* Name is used while confing... */
	VAR_NAME = 0,
	/* ... and cc while using the config. */
	VAR_CC = 0,
	VAR_CPPFLAGS,
	VAR_CFLAGS,
	VAR_LDFLAGS,
	VAR_LIBS,
	VAR_EXTRA,
	VAR_NUM
};
struct Options {
	char	var[VAR_NUM][STR_SIZ];
	int	do_link;
	int	penalty;
};

void	err_(int, char const *, ...);
void	errx_(int, char const *, ...);
void	hconf_merge(struct Options *, int, char const **);

#endif
