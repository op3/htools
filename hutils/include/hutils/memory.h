/*
 * Copyright (c) 2014-2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#ifndef HUTILS_MEMORY_H
#define HUTILS_MEMORY_H

#include <stdlib.h>
#include <hutils/err.h>
#include <hutils/macros.h>

#define CALLOC(ptr, num)\
	do {\
		int n_ = num;\
		ptr = calloc(n_, sizeof *ptr);\
		if (NULL == ptr) {\
			hutils_err(EXIT_FAILURE, __FILE__":%d: "\
			    "calloc(%d,%d)", __LINE__, n_, (int)sizeof *ptr);\
		}\
	} WHILE_0
#define DUP(dst, src)\
	do {\
		MALLOC(dst, sizeof *dst);\
		COPY(*dst, src);\
	} WHILE_0
#define FREE(ptr)\
	do {\
		free(ptr);\
		ptr = NULL;\
	} WHILE_0
#define MALLOC(ptr, size)\
	do {\
		size_t s_ = size;\
		ptr = malloc(s_);\
		if (NULL == ptr) {\
			hutils_err(EXIT_FAILURE, __FILE__":%d: malloc(%d)",\
			    __LINE__, (int)s_);\
		}\
	} WHILE_0

#endif
