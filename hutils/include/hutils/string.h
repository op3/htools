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

#include <stdarg.h>
#include <string.h>
#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hconf/include/hutils/string.h>

CDECLS_BEGIN

#if defined(HCONF_mNPRINTF_bSTDIO) || defined(_MSC_VER)
#	include <stdio.h>
#elif defined(HCONF_mNPRINTF_bPROTOTYPE)
int snprintf(char *, size_t, char const *, ...) FUNC_PRINTF(3, 4);
int vsnprintf(char *, size_t, char const *, va_list);
#elif defined(HCONF_mNPRINTF_bUNSAFE)
/* HCONF_SRC=src/string.c */
#	define snprintf hutils_snprintf_
#	define vsnprintf hutils_vsnprintf_
int hutils_snprintf_(char *, size_t, char const *, ...) FUNC_PRINTF(3, 4);
int hutils_vsnprintf_(char *, size_t, char const *, va_list);
#endif
#if defined(HCONFING_mNPRINTF)
HCONF_TEST(int, (char *a_s, size_t a_n, va_list a_args))
{
	return snprintf(a_s, a_n, "snprintf") +
	    vsnprintf(a_s, a_n, "vsnprintf", a_args);
}
#endif

#if defined(HCONF_mSTRDUP_bPOSIX_200809)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#elif defined(_MSC_VER)
#	define strdup _strdup
#endif
#if defined(HCONFING_mSTRDUP)
HCONF_TEST(char *, (char *a_s))
{
	return strdup(a_s);
}
#endif

#if defined(HCONF_mSTRL_bBSD_SOURCE)
/* HCONF_CPPFLAGS=-D_BSD_SOURCE */
#elif defined(HCONF_mSTRL_bCUSTOM) || defined(_MSC_VER)
/* HCONF_OPT=nolink */
size_t strlcat(char *, char const *, size_t);
size_t strlcpy(char *, char const *, size_t);
#endif
#if defined(HCONFING_mSTRL)
HCONF_TEST(size_t, (char *a_dst, char const *a_src, size_t a_dst_size))
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
HCONF_TEST(char *, (char *a_s, size_t a_n))
{
	return strndup(a_s, a_n);
}
#endif

#if defined(HCONF_mSTRSIGNAL_bPOSIX_200809)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#elif defined(HCONF_mSTRSIGNAL_bPROTOTYPE)
char *strsignal(int) FUNC_RETURNS;
#elif defined(HCONF_mSTRSIGNAL_bCUSTOM) || defined(_MSC_VER)
/* HCONF_SRC=src/string.c */
#	define strsignal hutils_strsignal_
char *hutils_strsignal_(int) FUNC_RETURNS;
#endif
#if defined(HCONFING_mSTRSIGNAL)
HCONF_TEST(char *, (int a_signum))
{
	return strsignal(a_signum);
}
#endif

#define STRAPP(dst, dstlen, ofs, src)\
	do {\
		ofs += strlcpy(dst + ofs, src, dstlen - ofs);\
		ofs = MIN(ofs, dstlen - 1);\
	} WHILE_0
#define STRCTV_BEGIN strctv_(
#define STRCTV_END ,strctv_sentinel_)

extern char const *strctv_sentinel_;

int	strbcmp(char const *, char const *) FUNC_RETURNS;
char	*strctv_(char const *, ...) FUNC_RETURNS;
int	strecmp(char const *, char const *) FUNC_RETURNS;

CDECLS_END

#endif
