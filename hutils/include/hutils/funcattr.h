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

#if defined(HCONF_mFUNC_NORETURN_bYES)
/* HCONF_OPT=nolink */
#	define FUNC_NORETURN __attribute__((noreturn))
#elif defined(HCONF_mFUNC_NORETURN_bNO) || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_NORETURN
#endif
#if defined(HCONFING_mFUNC_NORETURN)
void hconf_test_noreturn_(void) FUNC_NORETURN;
#endif

#if defined(HCONF_mFUNC_PRINTF_bYES)
/* HCONF_OPT=nolink */
#	define FUNC_PRINTF(fmt, args) \
    __attribute__((format(printf, fmt, args)))
#elif defined(HCONF_mFUNC_PRINTF_bNO) || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_PRINTF(fmt, args)
#endif
#if defined(HCONFING_mFUNC_PRINTF)
void hconf_test_printf_(int, char const *, ...) FUNC_PRINTF(2, 3);
#endif

#if defined(HCONF_mFUNC_FORMAT_ARG_bYES)
/* HCONF_OPT=nolink */
#	define FUNC_FORMAT_ARG(fmt) \
    __attribute__((format_arg(fmt)))
#elif defined(HCONF_mFUNC_FORMAT_ARG_bNO) || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_FORMAT_ARG(fmt)
#endif
#if defined(HCONFING_mFUNC_FORMAT_ARG)
char *hconf_test_format_arg_(int, char const *, ...) FUNC_FORMAT_ARG(2);
#endif

#if defined(HCONF_mFUNC_PURE_bYES)
/* HCONF_OPT=nolink */
#	define FUNC_PURE __attribute__((pure))
#elif defined(HCONF_mFUNC_PURE_bNO) || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_PURE
#endif
#if defined(HCONFING_mFUNC_PURE)
void hconf_test_pure_(void) FUNC_PURE;
#endif

#if defined(HCONF_mFUNC_RETURNS_bYES)
/* HCONF_OPT=nolink */
#	define FUNC_RETURNS __attribute__((warn_unused_result))
#elif defined(HCONF_mFUNC_RETURNS_bNO) || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_RETURNS
#endif
#if defined(HCONFING_mFUNC_RETURNS)
int hconf_test_returns_(void) FUNC_RETURNS;
#endif

#if defined(HCONF_mFUNC_UNUSED_bYES)
/* HCONF_OPT=nolink */
#	define FUNC_UNUSED __attribute__((unused))
#elif defined(HCONF_mFUNC_UNUSED_bNO) || defined(_MSC_VER)
/* HCONF_OPT=nolink */
#	define FUNC_UNUSED
#endif
#if defined(HCONFING_mFUNC_UNUSED)
int hconf_test_unused_(void) FUNC_UNUSED;
#endif

#endif
