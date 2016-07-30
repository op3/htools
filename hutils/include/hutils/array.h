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

#ifndef HUTILS_ARRAY_H
#define HUTILS_ARRAY_H

#include <assert.h>
#include <hutils/memory.h>

#define ARRAY_HEAD(Name, Type) struct Name {\
	int	size;\
	Type	*p;\
}
#define ARRAY_CALLOC(name, siz) do {\
	ARRAY_TEST(name, ==);\
	(name).size = siz;\
	CALLOC((name).p, siz);\
} WHILE_0
#define ARRAY_FREE(name) do {\
	ARRAY_TEST(name, !=);\
	(name).size = -1;\
	FREE((name).p);\
} WHILE_0
#define ARRAY_INIT(name) do {\
	(name).size = -1;\
	(name).p = NULL;\
} WHILE_0
#define ARRAY_TEST(name, op) do {\
	assert(-1 op (name).size);\
	assert(NULL op (name).p);\
} WHILE_0
#define ARRAY_ZERO(name) do {\
	ARRAY_TEST(name, !=);\
	memset((name).p, 0, (name).size * sizeof *(name).p);\
} WHILE_0

#endif
