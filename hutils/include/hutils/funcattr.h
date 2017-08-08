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

#ifndef HUTILS_FUNCATTR_H
#define HUTILS_FUNCATTR_H

#include <hconf/include/hutils/funcattr.h>

#if HCONF_mFUNC_NONNULL_bYES
/* HCONF_OPT=nolink */
#	define FUNC_NONNULL(list) __attribute__((nonnull list))
#elif HCONF_mFUNC_NONNULL_bNO || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_NONNULL(list)
#endif
#if HCONFING_mFUNC_NONNULL
char *hconf_test1_(void *) FUNC_NONNULL(());
char *hconf_test2_(void *) FUNC_NONNULL((1));
#endif

#if HCONF_mFUNC_NORETURN_bYES
/* HCONF_OPT=nolink */
#	define FUNC_NORETURN __attribute__((noreturn))
#elif HCONF_mFUNC_NORETURN_bNO || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_NORETURN
#endif
#if HCONFING_mFUNC_NORETURN
void hconf_test_(void) FUNC_NORETURN;
#endif

#if HCONF_mFUNC_PRINTF_bYES
/* HCONF_OPT=nolink */
#	define FUNC_PRINTF(fmt, args) \
    __attribute__((format(printf, fmt, args)))
#elif HCONF_mFUNC_PRINTF_bNO || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_PRINTF(fmt, args)
#endif
#if HCONFING_bFUNC_PRINTF
void hconf_test_(int, char const *, ...) FUNC_PRINTF(2, 3);
#endif

#if HCONF_mFUNC_PURE_bYES
/* HCONF_OPT=nolink */
#	define FUNC_PURE __attribute__((pure))
#elif HCONF_mFUNC_PURE_bNO || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_PURE
#endif
#if HCONFING_bFUNC_PURE
void hconf_test_(void) FUNC_PURE;
#endif

#if HCONF_mFUNC_RETURNS_bYES
/* HCONF_OPT=nolink */
#	define FUNC_RETURNS __attribute__((warn_unused_result))
#elif HCONF_mFUNC_RETURNS_bNO || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_RETURNS
#endif
#if HCONFING_bFUNC_RETURNS
int hconf_test_(void) FUNC_RETURNS;
#endif

#if HCONF_mFUNC_UNUSED_bYES
/* HCONF_OPT=nolink */
#	define FUNC_UNUSED __attribute__((unused))
#elif HCONF_mFUNC_UNUSED_bNO || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_UNUSED
#endif
#if HCONFING_bFUNC_UNUSED
int hconf_test_(void) FUNC_UNUSED;
#endif

#endif
