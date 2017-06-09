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

#if HCONF_BRANCH(CCACHE, YES)
/* HCONF_CC="ccache $CC" */
/* HCONF_OPT=nolink */
#elif HCONF_BRANCH(CCACHE, NO)
/* HCONF_CC="$CC" */
/* HCONF_OPT=nolink */
#endif

#if HCONF_BRANCH(WEXTRA, WEXTRA)
/* HCONF_CFLAGS=-Wextra */
/* HCONF_OPT=nolink */
#elif HCONF_BRANCH(WEXTRA, W)
/* HCONF_CFLAGS=-W */
/* HCONF_OPT=nolink */
#endif

#if HCONF_BRANCH(WSTRICT_ALIASING, YES)
/* HCONF_CFLAGS=-Wstrict-aliasing */
/* HCONF_OPT=nolink */
#elif HCONF_BRANCH(WSTRICT_ALIASING, NO)
/* HCONF_OPT=nolink */
#endif

#if HCONF_BRANCH(WSTRICT_OVERFLOW, YES)
/* HCONF_CFLAGS=-Wstrict-overflow */
/* HCONF_OPT=nolink */
#elif HCONF_BRANCH(WSTRICT_OVERFLOW, NO)
/* HCONF_OPT=nolink */
#endif

#if HCONF_BRANCH(FORTIFY_SOURCE, D1)
/* HCONF_CFLAGS=-D_FORTIFY_SOURCE=1 */
/* HCONF_OPT=nolink */
#elif HCONF_BRANCH(FORTIFY_SOURCE, UD1)
/* HCONF_CFLAGS=-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=1 */
/* HCONF_OPT=nolink */
#elif HCONF_BRANCH(FORTIFY_SOURCE, NO)
/* HCONF_OPT=nolink */
#endif

#endif
