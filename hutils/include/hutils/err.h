/*
 * Copyright (c) 2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#ifndef HUTILS_ERR_H
#define HUTILS_ERR_H

#include <hutils/macros.h>
#include <hconf/include/hutils/err.h>

#if defined(HCONF_ERR_H)
/* LIBS=dont */
# include <err.h>
#elif defined(HCONF_ERR_MSC)
/* LIBS=dont */
# include <windows.h>
# define err(code, str) do {\
		LPTSTR str_;\
		DWORD err_;\
		err_ = GetLastError();\
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | \
			      FORMAT_MESSAGE_FROM_SYSTEM | \
			      FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err_, \
			      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), \
			      (LPTSTR)&str_, 0, NULL);\
		MessageBox(NULL, str_, NULL, MB_OK);\
		LocalFree(str_);\
		exit(code);\
	} HUTILS_COND(while, 0)
#elif defined(HCONF_ERR_FPRINTF)
/* LIBS=dont */
# include <errno.h>
# include <stdio.h>
# include <string.h>
# define err(code, str) do {\
		fprintf(stderr, "%s: %s\n", str, strerror(errno));\
		exit(code);\
	} HUTILS_COND(while, 0)
#else
# error Not hconf:ed.
#endif

#define HCONF_TEST err(0, "a")

#endif
