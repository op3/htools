/*
 * Copyright (c) 2014 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
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

#define CALLOC(ptr, num) do {\
	size_t n;\
\
	n = num;\
	ptr = calloc(n, sizeof *ptr);\
	if (NULL == ptr) {\
		fprintf(stderr, "%s:%d: Could not calloc(%d#,%dB): %s.\n", \
		    __FILE__, __LINE__, (int)n, (int)sizeof *var, \
		    strerror(errno));\
		abort();\
	}\
} while (0)
#define MALLOC(ptr, size) do {\
	size_t s;\
\
	s = size;\
	ptr = malloc(s);\
	if (NULL == ptr) {\
		fprintf(stderr, "%s:%d: Could not malloc(%dB): %s.\n", \
		    __FILE__, __LINE__, (int)s, strerror(errno));\
		abort();\
	}\
} while (0)
#define FREE(ptr) do {\
	free(ptr);\
	ptr = NULL;\
} while (0)
#define STRDUP(dst, src) do {\
	dst = hutils_strdup_(src);\
	if (NULL == dst) {\
		fprintf(stderr, "%s:%d: Could not strdup(\"%s\"): %s.\n", \
		    __FILE__, __LINE__, src, strerror(errno));\
		abort();\
	}\
} while (0)

char *hutils_strdup_(char const *);

#endif
