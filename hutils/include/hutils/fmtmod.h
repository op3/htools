/*
 * Copyright (c) 2016-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#ifndef HUTILS_FMTMOD_H
#define HUTILS_FMTMOD_H

#include <stdio.h>
#include <stdlib.h>
#include <hutils/stdint.h>
#include <hconf/include/hutils/fmtmod.h>

#if HCONF_BRANCH(FMTMOD_SIZET, INT)
/* HCONF_OPT=nolink */
#	define PRIz "d"
#	define PRIzx "x"
#elif HCONF_BRANCH(FMTMOD_SIZET, Z)
/* HCONF_OPT=nolink */
#	define PRIz "z"
#	define PRIzx "zx"
#elif HCONF_BRANCH(FMTMOD_SIZET, LONG)
/* HCONF_OPT=nolink */
#	define PRIz "ld"
#	define PRIzx "lx"
#elif defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define PRIz "lu"
#	define PRIzx "lx"
#endif
#if HCONFING(FMTMOD_SIZET)
#	define HCONF_TEST return hconf_test_(argc)
static int hconf_test_(size_t i) {
	return printf("%"PRIz" %"PRIzx, i, i);
}
#endif

#if HCONF_BRANCH(FMTMOD_INTPTRT, INT)
/* HCONF_OPT=nolink */
#	define PRIp "d"
#	define PRIpx "x"
#elif HCONF_BRANCH(FMTMOD_INTPTRT, LONG)
/* HCONF_OPT=nolink */
#	define PRIp "ld"
#	define PRIpx "lx"
#endif
#if HCONFING(FMTMOD_INTPTRT)
#	define HCONF_TEST return hconf_test_(argc)
static int hconf_test_(intptr_t i) {
	return printf("%"PRIp" %"PRIpx, i, i);
}
#endif

#endif
