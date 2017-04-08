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

#if HCONF_BRANCH(NPRINTF, STDIO) || defined(_MSC_VER)
#	include <stdio.h>
#elif HCONF_BRANCH(NPRINTF, PROTOTYPE)
int snprintf(char *, size_t, char const *, ...) FUNC_PRINTF(3, 4);
int vsnprintf(char *, size_t, char const *, va_list) FUNC_PRINTF(3, 0);
#elif HCONF_BRANCH(NPRINTF, UNSAFE)
/* HCONF_SRC=src/string.c */
#	define snprintf hutils_snprintf_
#	define vsnprintf hutils_vsnprintf_
int hutils_snprintf_(char *, size_t, char const *, ...) FUNC_PRINTF(3, 4);
int hutils_vsnprintf_(char *, size_t, char const *, va_list) FUNC_PRINTF(3,
    0);
#endif
#if HCONFING(NPRINTF)
static int
vsnprintf_wrapper_(int a, ...)
{
	va_list args;
	char s[2];
	int ret;
	va_start(args, a);
	ret = vsnprintf(s, sizeof s, "%d", args);
	va_end(args);
	return ret;
}
HCONF_TEST
{
	char s[2];
	return snprintf(s, sizeof s, "%d", i) + vsnprintf_wrapper_(0, i);
}
#endif

#if HCONF_BRANCH(STRDUP, POSIX_200809)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#elif defined(_MSC_VER)
#	define strdup _strdup
#endif
#if HCONFING(STRDUP)
HCONF_TEST
{
	return NULL != strdup("") + 0 * i;
}
#endif

#if HCONF_BRANCH(STRLCPY, BSD_SOURCE)
/* HCONF_CPPFLAGS=-D_BSD_SOURCE */
#elif HCONF_BRANCH(STRLCPY, CUSTOM) || defined(_MSC_VER)
/* HCONF_OPT=nolink */
size_t strlcat(char *, char const *, size_t);
size_t strlcpy(char *, char const *, size_t);
#endif
#if HCONFING(STRLCPY)
HCONF_TEST
{
	char s[2];
	return strlcat(s, "", sizeof s) + strlcpy(s, "", sizeof s) + 0 * i;
}
#endif

#if HCONF_BRANCH(STRNDUP, POSIX_200809)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#elif HCONF_BRANCH(STRNDUP, CUSTOM)
/* HCONF_SRC=src/string.c */
#	define strndup hutils_strndup_
char *hutils_strndup_(char const *, size_t) FUNC_RETURNS;
#endif
#if HCONFING(STRNDUP)
HCONF_TEST
{
	return NULL != strndup("", 0) + 0 * i;
}
#endif

#if HCONF_BRANCH(STRSIGNAL, POSIX_200809)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#elif HCONF_BRANCH(STRSIGNAL, PROTOTYPE)
char *strsignal(int) FUNC_RETURNS;
#elif HCONF_BRANCH(STRSIGNAL, CUSTOM) || defined(_MSC_VER)
/* HCONF_SRC=src/string.c */
#	define strsignal hutils_strsignal_
char *hutils_strsignal_(int) FUNC_RETURNS;
#endif
#if HCONFING(STRSIGNAL)
#	include <signal.h>
HCONF_TEST
{
	return NULL != strsignal(SIGSEGV) + 0 * i;
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

int	strbcmp(char const *, char const *) FUNC_NONNULL(()) FUNC_RETURNS;
char	*strctv_(char const *, ...) FUNC_RETURNS;
int	strecmp(char const *, char const *) FUNC_NONNULL(()) FUNC_RETURNS;

CDECLS_END

#endif
