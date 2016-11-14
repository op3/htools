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

#ifndef HUTILS_GCC_H
#define HUTILS_GCC_H

#include <hconf/include/hutils/gcc.h>

#if defined(HCONF_mCCACHE_bYES)
/* HCONF_CC="ccache $CC" */
/* HCONF_OPT=nolink */
#elif defined(HCONF_mCCACHE_bNO)
/* HCONF_CC="$CC" */
/* HCONF_OPT=nolink */
#endif

#if defined(HCONF_mWEXTRA_bWEXTRA)
/* HCONF_CFLAGS=-Wextra */
/* HCONF_OPT=nolink */
#elif defined(HCONF_mWEXTRA_bW)
/* HCONF_CFLAGS=-W */
/* HCONF_OPT=nolink */
#endif

#if defined(HCONF_mWFORMAT_bWFORMAT2)
/* HCONF_CFLAGS=-Wformat=2 */
/* HCONF_OPT=nolink */
#endif

#endif
