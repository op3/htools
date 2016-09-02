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

#ifndef HUTILS_ASSERT_H
#define HUTILS_ASSERT_H

#include <assert.h>
#include <stdio.h>
#include <hutils/macros.h>

#ifdef NDEBUG
#	define ASSERT(Type, fmt, lhs, op, rhs)
#else
#	define ASSERT(Type, fmt, lhs, op, rhs) do {\
	Type lhs_ = lhs;\
	Type rhs_ = rhs;\
	if (!(lhs_ op rhs_)) {\
		fprintf(stderr, "%s:%d: "#lhs"=%"#fmt" "#op" "#rhs"=%"#fmt\
		    " failed.", __FILE__, __LINE__, lhs_, rhs_);\
		abort();\
	}\
} WHILE_0
#endif

#endif
