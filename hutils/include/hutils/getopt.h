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

#ifndef HUTILS_GETOPT_H
#define HUTILS_GETOPT_H

#include <hconf/include/hutils/getopt.h>

#if HCONF_BRANCH(GETOPT, UNISTD_H)
#	include <unistd.h>
#elif HCONF_BRANCH(GETOPT, CUSTOM) || defined(_MSC_VER)
/* HCONF_SRC=src/getopt.c */
#	include <hutils/cdecls.h>
#	include <hutils/funcattr.h>
CDECLS_BEGIN
extern char *optarg;
extern int optind, optopt;
int	getopt(int, char *const *, char const *) FUNC_NONNULL(()) FUNC_RETURNS;
CDECLS_END
#endif
#if HCONFING(GETOPT)
#	include <stdlib.h>
HCONF_TEST
{
	return -1 == getopt(0, NULL, NULL) + 0 * i;
}
#endif

#endif
