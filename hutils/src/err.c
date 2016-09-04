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

#include <hutils/err.h>
#include <hutils/string.h>

#if defined(_MSC_VER)

void
err(int a_eval, char const *a_fmt, ...)
{
	va_list args;
	char msg[1024];

	va_start(args, a_fmt);
	vsnprintf(msg, sizeof msg, a_fmt, args);
	va_end(args);
/*	LPTSTR str;
	DWORD code;
	code = GetLastError();
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
	    code, 0, (LPTSTR)&str, 0, NULL);
	LocalFree(str);*/
	MessageBox(NULL, msg, NULL, MB_OK);
	exit(a_eval);
}

#elif defined(HCONF_mERR_bCUSTOM)
#		include <errno.h>
#		include <stdarg.h>
#		include <stdio.h>
#		include <string.h>

void
err(int a_eval, char const *a_fmt, ...)
{
	va_list args;
	int errno_;

	errno_ = errno;
	va_start(args, a_fmt);
	vfprintf(stderr, a_fmt, args);
	fprintf(stderr, ": %s.\n", strerror(errno_));
	va_end(args);
	exit(a_eval);
}

#endif
