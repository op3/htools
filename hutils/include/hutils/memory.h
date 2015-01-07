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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define MALLOC(var, num) do {\
	size_t size;\
\
	size = (num) * sizeof *var;\
	var = malloc(size);\
	if (NULL == var) {\
		fprintf(stderr, "Could not malloc (%d B).\n", (int)size);\
		abort();\
	}\
} while (0)
#define MALLOC_BYTES(var, num) do {\
	size_t size;\
\
	size = num;\
	var = malloc(size);\
	if (NULL == var) {\
		fprintf(stderr, "Could not malloc (%d B).\n", (int)size);\
		abort();\
	}\
} while (0)
#define FREE(var) do {\
	free(var);\
	var = NULL;\
} while (0)
#define STRDUP(dst, src) do {\
	dst = hutils_strdup_(src);\
	if (NULL == dst) {\
		fprintf(stderr, "Could not strdup (%s).\n", src);\
		abort();\
	}\
} while (0)

char *hutils_strdup_(char const *);

#endif
