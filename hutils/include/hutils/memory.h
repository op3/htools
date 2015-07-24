/*
 * Copyright (c) 2014-2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hutils/macros.h>

#define CALLOC(ptr, num) do {\
	size_t n_;\
\
	n_ = num;\
	ptr = calloc(n_, sizeof *ptr);\
	if (NULL == ptr) {\
		fprintf(stderr, "%s:%d: Could not calloc(%d#,%dB): %s.\n", \
		    __FILE__, __LINE__, (int)n_, (int)sizeof *ptr, \
		    strerror(errno));\
		abort();\
	}\
} HUTILS_COND(while, 0)
#define MALLOC(ptr, size) do {\
	size_t s_;\
\
	s_ = size;\
	ptr = malloc(s_);\
	if (NULL == ptr) {\
		fprintf(stderr, "%s:%d: Could not malloc(%dB): %s.\n", \
		    __FILE__, __LINE__, (int)s_, strerror(errno));\
		abort();\
	}\
} HUTILS_COND(while, 0)
#define FREE(ptr) do {\
	free(ptr);\
	ptr = NULL;\
} HUTILS_COND(while, 0)
#define STRDUP(dst, src) do {\
	dst = strdup(src);\
	if (NULL == dst) {\
		fprintf(stderr, "%s:%d: Could not strdup(\"%s\"): %s.\n", \
		    __FILE__, __LINE__, src, strerror(errno));\
		abort();\
	}\
} HUTILS_COND(while, 0)

#endif
