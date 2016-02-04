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

enum OptName {
	/* Name is used while confing... */
	OPT_NAME = 0,
	/* ... and cc while using the config. */
	OPT_CC = 0,
	OPT_CPPFLAGS,
	OPT_CFLAGS,
	OPT_LDFLAGS,
	OPT_LIBS,
	OPT_EXTRA,
	OPT_NUM
};
typedef char Options[OPT_NUM][STR_SIZ];

void	err_(int, char const *, ...);
void	errx_(int, char const *, ...);
void	hconf_merge(Options, int, char const **);

#endif
