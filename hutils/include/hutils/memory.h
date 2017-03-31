/*
 * Copyright (c) 2014-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
#include <hutils/fmtmod.h>
#include <hutils/macros.h>

#define CALLOC(ptr, num) ptr = calloc(num, sizeof *ptr)
#define DUP(dst, src)\
	do {\
		ASSERT(size_t, PRIz, sizeof *dst, ==, sizeof *src);\
		dst = malloc(sizeof *dst);\
		COPY(*dst, *src);\
	} WHILE_0
#define FREE(ptr)\
	do {\
		free(ptr);\
		ptr = NULL;\
	} WHILE_0

/* The following from OpenBSD /usr/src/lib/libc/stdlib/reallocarray.c. */
#ifndef MUL_NO_OVERFLOW
#	define MUL_NO_OVERFLOW	((size_t)1 << (sizeof(size_t) * 4))
#endif
#define REALLOCARRAY(ptr, num)\
	do {\
		size_t n_ = num;\
		if ((n_ >= MUL_NO_OVERFLOW ||\
		    sizeof *ptr >= MUL_NO_OVERFLOW) &&\
		    n_ > 0 && SIZE_MAX / n_ < sizeof *ptr) {\
			fprintf(stderr, __FILE__":%d: REALLOCARRAY(%"PRIz","\
			    "%"PRIz") overflow.\n", __LINE__, n_,\
			    sizeof *ptr);\
			exit(EXIT_FAILURE);\
		}\
		ptr = realloc(ptr, n_ * sizeof *ptr);\
	} WHILE_0

#endif
