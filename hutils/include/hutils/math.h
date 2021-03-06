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

#ifndef HUTILS_MATH_H
#define HUTILS_MATH_H

#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hutils/stdint.h>
#include <hconf/include/hutils/math.h>

#if HCONF_mMATH_bNOTHING
#elif HCONF_mMATH_bLM
/* HCONF_LIBS=-lm */
#elif HCONF_mMATH_bBSD_SOURCE_LM
/* HCONF_CPPFLAGS=-D_BSD_SOURCE */
/* HCONF_LIBS=-lm */
#elif HCONF_mMATH_bDEFAULT_SOURCE_LM
/* HCONF_CPPFLAGS=-D_DEFAULT_SOURCE */
/* HCONF_LIBS=-lm */
#elif HCONF_mMATH_bNOFLOATFUNC
#	define SINF_ (float)sin
#elif defined(_MSC_VER)
#	define _USE_MATH_DEFINES
#endif
#include <math.h>
#if HCONFING_mMATH
#	ifndef SINF_
#		define SINF_ sinf
#	endif
#	define HCONF_TEST return 2.0 > SINF_(M_PI * argc) ? 0 : 1
#endif

CDECLS_BEGIN

uint16_t	double2half(double) FUNC_RETURNS;
double	half2double(uint16_t) FUNC_RETURNS;

CDECLS_END

#endif
