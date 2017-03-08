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

#ifndef HUTILS_FMTMOD_H
#define HUTILS_FMTMOD_H

#include <stdio.h>
#include <stdlib.h>
#include <hutils/stdint.h>
#include <hconf/include/hutils/fmtmod.h>

#if defined(HCONF_mFMTMOD_SIZET_bINT)
/* HCONF_OPT=nolink */
#	define PRIz "d"
#	define PRIzx "x"
#elif defined(HCONF_mFMTMOD_SIZET_bZ)
/* HCONF_OPT=nolink */
#	define PRIz "z"
#	define PRIzx "zx"
#elif defined(HCONF_mFMTMOD_SIZET_bLONG)
/* HCONF_OPT=nolink */
#	define PRIz "ld"
#	define PRIzx "lx"
#elif defined(_MSC_VER)
#	define PRIz "lu"
#	define PRIzx "lx"
#endif
#if defined(HCONFING_mFMTMOD_SIZET)
HCONF_TEST
{
	return printf("%"PRIz" %"PRIzx, (size_t)1, (size_t)1);
}
#endif

#if defined(HCONF_mFMTMOD_INTPTRT_bINT)
/* HCONF_OPT=nolink */
#	define PRIp "d"
#	define PRIpx "x"
#elif defined(HCONF_mFMTMOD_INTPTRT_bLONG)
/* HCONF_OPT=nolink */
#	define PRIp "ld"
#	define PRIpx "lx"
#endif
#if defined(HCONFING_mFMTMOD_INTPTRT)
HCONF_TEST
{
	return printf("%"PRIp" %"PRIpx" %"PRIp" %"PRIpx,
	    (uintptr_t)1, (uintptr_t)1, (intptr_t)1, (intptr_t)1);
}
#endif

#endif
