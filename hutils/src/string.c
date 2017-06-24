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

#include <hutils/string.h>
#include <stdarg.h>
#include <hutils/memory.h>

#if HCONF_BRANCH(NPRINTF, UNSAFE)
#	include <stdarg.h>
#	include <stdio.h>
int
hutils_snprintf_(char *a_dst, size_t a_dst_size, char const *a_fmt, ...)
{
	va_list args;
	int len;

	va_start(args, a_fmt);
	len = hutils_vsnprintf_(a_dst, a_dst_size, a_fmt, args);
	va_end(args);
	return len;
}

int
hutils_vsnprintf_(char *a_dst, size_t a_dst_size, char const *a_fmt, va_list
    a_args)
{
	int len;

	len = vsprintf(a_dst, a_fmt, a_args);
	if (a_dst_size < (size_t)len) {
		fprintf(stderr, "Overrun in hutils_vsnprintf_, abort!()\n");
		abort();
	}
	return len;
}
#endif

#if HCONF_BRANCH(STRNDUP, CUSTOM)
char *
hutils_strndup_(char const *a_s, size_t a_maxlen)
{
	char *s;
	size_t len;

	len = strlen(a_s);
	len = MIN(len, a_maxlen);
	s = malloc(len + 1);
	if (NULL == s) {
		return NULL;
	}
	if (NULL != s) {
		memmove(s, a_s, len);
		s[len] = '\0';
	}
	return s;
}
#endif

#if HCONF_BRANCH(STRSIGNAL, CUSTOM)
#	include <sigcodes.h>

char *
hutils_strsignal_(int a_signum)
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
	return "Unknown";
}
#elif defined(_MSC_VER)
#	include <signal.h>

char *
hutils_strsignal_(int a_signum)
{
#	define TRANSLATE(name) case SIG##name: return #name
	switch (a_signum) {
		TRANSLATE(ABRT);
		TRANSLATE(FPE);
		TRANSLATE(ILL);
		TRANSLATE(INT);
		TRANSLATE(SEGV);
		TRANSLATE(TERM);
	}
	return "Unknown";
}
#endif

char const *strctv_sentinel_ = (void *)&strctv_sentinel_;
static char const c_NULL[] = "NULL";

/*
 * Compares the beginning of the big string with the pattern.
 *  a_big: Big string.
 *  a_pattern: Pattern string.
 *  Returns: Similar to strcmp.
 */
int
strbcmp(char const *a_big, char const *a_pattern)
{
	char const *b;
	char const *p;

	b = a_big;
	p = a_pattern;
	for (;; ++b, ++p) {
		if ('\0' == *p) {
			return 0;
		}
		if ('\0' == *b) {
			return -1;
		}
		if (*b < *p) {
			return -1;
		}
		if (*b > *p) {
			return 1;
		}
	}
}

/*
 * Concatenates variadic strings into a malloc:d string.
 *  a_dst: Location of potentially non-empty string, may be NULL, may be
 *         realloc:ed.
 *  Returns: *a_dst on success, NULL on failure.
 */
char *
strctv_(char **a_dst, ...)
{
	va_list args;
	char const *from;
	char *dst, *ndst, *to;
	size_t dstlen, len;

	dst = *a_dst;
	len = dstlen = NULL == dst ? 0 : strlen(dst);
	va_start(args, a_dst);
	for (;;) {
		from = va_arg(args, char const *);
		if (strctv_sentinel_ == from) {
			break;
		}
		if (NULL == from) {
			len += sizeof c_NULL - 1;
		} else {
			len += strlen(from);
		}
	}
	va_end(args);
	ndst = realloc(dst, len + 1);
	if (NULL == ndst) {
		return NULL;
	}
	to = ndst + dstlen;
	va_start(args, a_dst);
	for (;;) {
		from = va_arg(args, char const *);
		if (strctv_sentinel_ == from) {
			break;
		}
		if (NULL == from) {
			memmove(to, c_NULL, sizeof c_NULL - 1);
			to += sizeof c_NULL - 1;
		} else {
			while ('\0' != *from) {
				*to++ = *from++;
			}
		}
	}
	va_end(args);
	*to = '\0';
	return *a_dst = ndst;
}

/*
 * Compares the end of the big string with the pattern.
 *  a_big: Big string.
 *  a_pattern: Pattern string.
 *  Returns: Similar to strcmp.
 */
int
strecmp(char const *a_big, char const *a_pattern)
{
	size_t big_len, pattern_len;

	big_len = strlen(a_big);
	pattern_len = strlen(a_pattern);
	if (big_len < pattern_len) {
		return -1;
	}
	return strcmp(a_big + big_len - pattern_len, a_pattern);
}
