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

#ifndef HUTILS_ERR_H
#define HUTILS_ERR_H

#include <hconf/include/hutils/err.h>

#if defined(HCONF_mERR_bERR_H)
/* HCONF_OPT=nolink */
#	include <err.h>
#elif defined(HCONF_mERR_bMSC)
/* HCONF_SRC=src/err.c */
#	include <windows.h>
void err(int, const char *, ...);
#elif defined(HCONF_mERR_bCUSTOM)
/* HCONF_SRC=src/err.c */
void err(int, const char *, ...);
#endif

#if defined(HCONF_TESTING)
void
test(void)
{
	err(0, "a");
}
#endif

#endif
