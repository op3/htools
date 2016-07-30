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

#include <hutils/string.h>
#include <stdarg.h>
#include <hutils/memory.h>

#if defined(HCONF_mSNPRINTF_bUNSAFE)
#	include <stdarg.h>
#	include <stdio.h>
int
snprintf_unsafe_(char *const a_dst, size_t const a_dst_size, char const *const
    a_fmt, ...)
{
	va_list args;
	size_t len;

	va_start(args, a_fmt);
	len = vsprintf(a_dst, a_fmt, args);
	if (a_dst_size < len) {
		fprintf(stderr, "Overrun in snprintf_unsafe_, abort!()\n");
		abort();
	}
	va_end(args);
	return len;
}
#endif

#if defined(HCONF_mSTRNDUP_bCUSTOM)
#	include <string.h>
char *
strndup_custom_(char const *const a_s, size_t const a_maxlen)
{
	char *s;
	size_t len;

	len = strlen(a_s);
	len = MIN(len, a_maxlen);
	MALLOC(s, len + 1);
	if (NULL != s) {
		memmove(s, a_s, len);
		s[len] = '\0';
	}
	return s;
}
#endif

#if defined(HCONF_mSTRSIGNAL_bCUSTOM)
char *
strsignal(int const a_signum)
{
#	define TRANSLATE(name) case SIG##name: return #name
	switch (a_signum) {
		TRANSLATE(HUP);
		TRANSLATE(INT);
		TRANSLATE(QUIT);
		TRANSLATE(ILL);
		TRANSLATE(TRAP);
		TRANSLATE(ABRT);
		TRANSLATE(FPE);
		TRANSLATE(KILL);
		TRANSLATE(BUS);
		TRANSLATE(SEGV);
		TRANSLATE(SYS);
		TRANSLATE(PIPE);
		TRANSLATE(ALRM);
		TRANSLATE(TERM);
		TRANSLATE(URG);
		TRANSLATE(STOP);
		TRANSLATE(TSTP);
		TRANSLATE(CONT);
		TRANSLATE(CHLD);
		TRANSLATE(TTIN);
		TRANSLATE(TTOU);
		TRANSLATE(XCPU);
		TRANSLATE(XFSZ);
		TRANSLATE(VTALRM);
		TRANSLATE(PROF);
		TRANSLATE(USR1);
		TRANSLATE(USR2);
	}
}
#endif

char const *strctv_sentinel_ = (char const *)&strctv_sentinel_;

/*
 * Compares the beginning of the big string with the pattern.
 *  a_big: Big string.
 *  a_pattern: Pattern string.
 *  Returns: Similar to strcmp.
 */
int
strbcmp(char const *const a_big, char const *const a_pattern)
{
	size_t pattern_len;

	pattern_len = strlen(a_pattern);
	return strncmp(a_big, a_pattern, pattern_len);
}

/*
 * Concatenates variadic strings into a malloc:d string.
 *  a_s1: Variadic list of strings.
 *  Returns: Concatenated string.
 */
char *
strctv_(char const *a_s1, ...)
{
	va_list args;
	char const *from;
	char *dst, *to;
	size_t len;

	len = 0;
	va_start(args, a_s1);
	from = a_s1;
	do {
		if (NULL != from) {
			len += strlen(from);
		}
		from = va_arg(args, char const *);
	} while (strctv_sentinel_ != from);
	va_end(args);
	MALLOC(dst, len + 1);
	to = dst;
	va_start(args, a_s1);
	from = a_s1;
	do {
		if (NULL != from) {
			while ('\0' != *from) {
				*to++ = *from++;
			}
		}
		from = va_arg(args, char const *);
	} while (strctv_sentinel_ != from);
	va_end(args);
	*to = '\0';
	return dst;
}

/*
 * Compares the end of the big string with the pattern.
 *  a_big: Big string.
 *  a_pattern: Pattern string.
 *  Returns: Similar to strcmp.
 */
int
strecmp(char const *const a_big, char const *const a_pattern)
{
	size_t big_len, pattern_len;

	big_len = strlen(a_big);
	pattern_len = strlen(a_pattern);
	if (big_len < pattern_len) {
		return -1;
	}
	return strcmp(a_big + big_len - pattern_len, a_pattern);
}
