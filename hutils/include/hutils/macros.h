/*
 * Copyright (c) 2015-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#ifndef HUTILS_MACROS_H
#define HUTILS_MACROS_H

#include <string.h>
#include <hconf/include/hutils/macros.h>

#if HCONF_mCONST_EXPR_bSIMPLE
/* HCONF_OPT=nolink */
#	define WHILE_0 while (0)
#	define IF_CONST(cond) if (cond)
#elif defined(_MSC_VER)
#	define WHILE_0\
		__pragma(warning(push))\
		__pragma(warning(disable:4127))\
		while (0)\
		__pragma(warning(pop))
#	define IF_CONST(cond)\
		__pragma(warning(push))\
		__pragma(warning(disable:4127))\
		if (cond)\
		__pragma(warning(pop))
#endif
#if HCONFING_bCONST_EXPR
#	define HCONF_TEST do {} WHILE_0
#endif

#define ABS(x) (0 > (x) ? -(x) : (x))
#define CLAMP(x, a, b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))
#define COPY(dst, src) do {\
	int copy_assert_size_[sizeof dst == sizeof src];\
	(void)copy_assert_size_;\
	memmove(&dst, &src, sizeof dst);\
} WHILE_0
#define IS_POW2(x) (0 == (((x) - 1) & (x)))
#define LENGTH(x) (sizeof x / sizeof *x)
#define MASK(lsb, msb) ((0xffffffff >> (32 - msb)) & \
    ((0xffffffff >> lsb) << lsb))
#ifndef MAX
#	define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif
#ifndef MIN
#	define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif
#define MOVE(dst, src) do {\
	COPY(dst, src);\
	ZERO(src);\
} WHILE_0
#define OFFSETOF(s, m) ((uintptr_t)&(s).m - (uintptr_t)&(s))
#define SGN(x) (0 > (x) ? -1 : 1)
#define SQR(x) ((x) * (x))
#define STRINGIFY(x) #x
#define STRINGIFY_VALUE(x) STRINGIFY(x)
#define SWAP(a, b) do {\
	char swap_assert_size_[sizeof a == sizeof b ? (signed)sizeof a : -1];\
	memmove(swap_assert_size_, &a, sizeof a);\
	memmove(&a, &b, sizeof a);\
	memmove(&b, swap_assert_size_, sizeof a);\
} WHILE_0
#define ZERO(x) memset(&x, 0, sizeof x)

#endif
