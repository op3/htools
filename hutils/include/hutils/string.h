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

#ifndef HUTILS_STRING_H
#define HUTILS_STRING_H

#include <string.h>
#include <hutils/funcattr.h>
#include <hconf/include/hutils/string.h>

#if defined(HCONF_mNPRINTF_bSTDIO)
#	include <stdio.h>
#elif defined(HCONF_mNPRINTF_bPROTOTYPE)
#	include <stdarg.h>
int snprintf(char *, size_t, char const *, ...) FUNC_PRINTF(3, 4);
int vsnprintf(char *, size_t, char const *, va_list);
#elif defined(HCONF_mNPRINTF_bUNSAFE)
/* HCONF_SRC=src/string.c */
#	include <stdarg.h>
#	define snprintf hutils_snprintf_
#	define vsnprintf hutils_vsnprintf_
int hutils_snprintf_(char *, size_t, char const *, ...) FUNC_PRINTF(3, 4);
int hutils_vsnprintf_(char *, size_t, char const *, va_list);
#endif
#if defined(HCONFING_mNPRINTF)
HCONF_TEST(int, (char *const a_s, size_t const a_n, char const *const a_fmt))
{
	return snprintf(a_s, a_n, a_fmt);
}
#endif

#if defined(HCONF_mSTRDUP_bPOSIX_200809)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#endif
#if defined(HCONFING_mSTRDUP)
HCONF_TEST(char *, (char *const a_s))
{
	return strdup(a_s);
}
#endif

#if defined(HCONF_mSTRL_bBSD_SOURCE)
/* HCONF_CPPFLAGS=-D_BSD_SOURCE */
#elif defined(HCONF_mSTRL_bCUSTOM)
/* HCONF_OPT=nolink */
size_t strlcat(char *, char const *, size_t);
size_t strlcpy(char *, char const *, size_t);
#endif
#if defined(HCONFING_mSTRL)
HCONF_TEST(size_t, (char *const a_dst, char const *const a_src, size_t const
    a_dst_size))
{
	return strlcat(a_dst, a_src, a_dst_size) +
	    strlcpy(a_dst, a_src, a_dst_size);
}
#endif

#if defined(HCONF_mSTRNDUP_bPOSIX_200809)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#elif defined(HCONF_mSTRNDUP_bCUSTOM)
/* HCONF_SRC=src/string.c */
#	define strndup hutils_strndup_
char *hutils_strndup_(char const *, size_t) FUNC_RETURNS;
#endif
#if defined(HCONFING_mSTRNDUP)
HCONF_TEST(char *, (char *const a_s, size_t const a_n))
{
	return strndup(a_s, a_n);
}
#endif

#if defined(HCONF_mSTRSIGNAL_bPOSIX_200809)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#elif defined(HCONF_mSTRSIGNAL_bPROTOTYPE)
char *strsignal(int) FUNC_RETURNS;
#elif defined(HCONF_mSTRSIGNAL_bCUSTOM)
/* HCONF_SRC=src/string.c */
#	define strsignal hutils_strsignal_
char *hutils_strsignal_(int) FUNC_RETURNS;
#endif
#if defined(HCONFING_mSTRSIGNAL)
HCONF_TEST(char *, (int const a_signum))
{
	return strsignal(a_signum);
}
#endif

#define STRCTV_BEGIN strctv_(
#define STRCTV_END ,strctv_sentinel_)

extern char const *strctv_sentinel_;

int	strbcmp(char const *, char const *) FUNC_RETURNS;
char	*strctv_(char const *, ...) FUNC_RETURNS;
int	strecmp(char const *, char const *) FUNC_RETURNS;

#endif
