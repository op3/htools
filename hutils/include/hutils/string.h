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

#ifndef HUTILS_STRING_H
#define HUTILS_STRING_H

#include <stdarg.h>
#include <string.h>
#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hconf/include/hutils/string.h>

CDECLS_BEGIN

#if HCONF_mNPRINTF_bSTDIO || defined(_MSC_VER)
#	include <stdio.h>
#elif HCONF_mNPRINTF_bPROTOTYPE
int snprintf(char *, size_t, char const *, ...) FUNC_PRINTF(3, 4);
int vsnprintf(char *, size_t, char const *, va_list) FUNC_PRINTF(3, 0);
#elif HCONF_mNPRINTF_bUNSAFE
/* HCONF_SRC=src/string.c */
#	define snprintf hutils_snprintf_
#	define vsnprintf hutils_vsnprintf_
int hutils_snprintf_(char *, size_t, char const *, ...) FUNC_PRINTF(3, 4);
int hutils_vsnprintf_(char *, size_t, char const *, va_list) FUNC_PRINTF(3,
    0);
#endif
#if HCONFING_mNPRINTF
#	define HCONF_TEST return 0 == nprintf_test_(argc, "")
static int nprintf_test_(int i, ...) {
	va_list args;
	char s[10];
	int ret;
	va_start(args, i);
	ret = vsnprintf(s, sizeof s, "%d", args);
	va_end(args);
	return snprintf(s, sizeof s, "%d", i) + ret;
}
#endif

#if HCONF_mSTRDUP_bPOSIX_200809
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#elif defined(_MSC_VER)
#	define strdup _strdup
#endif
#if HCONFING_mSTRDUP
#	define HCONF_TEST return NULL == strdup(argv[0])
#endif

#if HCONF_mSTRL_bBSD_SOURCE
/* HCONF_CPPFLAGS=-D_BSD_SOURCE */
#elif HCONF_mSTRL_bCUSTOM || defined(_MSC_VER)
/* HCONF_OPT=nolink */
size_t strlcat(char *, char const *, size_t);
size_t strlcpy(char *, char const *, size_t);
#endif
#if HCONFING_mSTRL
#	define HCONF_TEST return strl_test_(argv[0])
static int strl_test_(char const *str) {
	char s[2];
	return strlcat(s, str, sizeof s) + strlcpy(s, str, sizeof s);
}
#endif

#if HCONF_mSTRNDUP_bPOSIX_200809
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#elif HCONF_mSTRNDUP_bCUSTOM
/* HCONF_SRC=src/string.c */
#	define strndup hutils_strndup_
char *hutils_strndup_(char const *, size_t) FUNC_RETURNS;
#endif
#if HCONFING_mSTRNDUP
#	define HCONF_TEST return NULL == strndup(argv[0], 1)
#endif

#if HCONF_mSTRSIGNAL_bPOSIX_200809
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#elif HCONF_mSTRSIGNAL_bPROTOTYPE
char *strsignal(int) FUNC_RETURNS;
#elif HCONF_mSTRSIGNAL_bCUSTOM || defined(_MSC_VER)
/* HCONF_SRC=src/string.c */
#	define strsignal hutils_strsignal_
char *hutils_strsignal_(int) FUNC_RETURNS;
#endif
#if HCONFING_mSTRSIGNAL
#	include <signal.h>
#	define HCONF_TEST return NULL == strsignal(argc)
#endif

#define STRCTV_BEGIN strctv_(
#define STRCTV_END ,strctv_sentinel_)

extern char const *strctv_sentinel_;

int	strbcmp(char const *, char const *) FUNC_NONNULL(()) FUNC_RETURNS;
char	*strctv_(char **, ...) FUNC_NONNULL((1));
int	strecmp(char const *, char const *) FUNC_NONNULL(()) FUNC_RETURNS;

CDECLS_END

#endif
