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

#ifndef HUTILS_GCC_H
#define HUTILS_GCC_H

#include <hconf/include/hutils/gcc.h>
/* FORTIFY_SOURCE only throws certain errors, when features.h is included */
#include <stdint.h>

#if HCONF_mCCACHE_bYES
/* HCONF_CC="ccache $CC" */
/* HCONF_OPT=nolink */
#elif HCONF_mCCACHE_bNO
/* HCONF_CC="$CC" */
/* HCONF_OPT=nolink */
#endif

#if HCONF_mWEXTRA_bWEXTRA
/* HCONF_CFLAGS=-Wextra */
/* HCONF_OPT=nolink */
#elif HCONF_mWEXTRA_bW
/* HCONF_CFLAGS=-W */
/* HCONF_OPT=nolink */
#endif

#if HCONF_mWSTRICT_ALIASING_bYES
/* HCONF_CFLAGS="-fstrict-aliasing -Wstrict-aliasing" */
/* HCONF_OPT=nolink */
#elif HCONF_mWSTRICT_ALIASING_bNO
/* HCONF_OPT=nolink */
#endif

#if HCONF_mWSTRICT_OVERFLOW_bYES
/* HCONF_CFLAGS=-Wstrict-overflow */
/* HCONF_OPT=nolink */
#elif HCONF_mWSTRICT_OVERFLOW_bNO
/* HCONF_OPT=nolink */
#endif

#if HCONF_mFORTIFY_SOURCE_bD1
/* HCONF_CFLAGS=-D_FORTIFY_SOURCE=1 */
/* HCONF_OPT=nolink */
#elif HCONF_mFORTIFY_SOURCE_bUD1
/* HCONF_CFLAGS=-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=1 */
/* HCONF_OPT=nolink */
#elif HCONF_mFORTIFY_SOURCE_bNO
/* HCONF_OPT=nolink */
#endif

#endif
