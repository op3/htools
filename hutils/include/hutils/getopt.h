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

#ifndef HUTILS_GETOPT_H
#define HUTILS_GETOPT_H

#include <hconf/include/hutils/getopt.h>

#if defined(HCONF_mGETOPT_bUNISTD_H)
#	include <unistd.h>
#elif defined(HCONF_mGETOPT_bCUSTOM)
/* HCONF_SRC=src/getopt.c */
#	include <hutils/funcattr.h>
extern char *optarg;
extern int optind, optopt;
int	getopt(int, char const **, char const *) FUNC_RETURNS;
#endif

#if defined(HCONFING_mGETOPT)
HCONF_TEST(int, (int argc, char **argv))
{
	return getopt(argc, argv, "b");
}
#endif

#endif
