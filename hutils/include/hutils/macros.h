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

#ifndef HUTILS_MACROS_H
#define HUTILS_MACROS_H

#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <hconf/include/hutils/macros.h>

#define ABS(x) (0 > x ? -x : x)
#define CLAMP(x, a, b) (x < a ? a : x > b ? b : x)
#define COPY(dst, src) do {\
	assert(sizeof dst == sizeof src);\
	memmove(&dst, &src, sizeof dst);\
} HUTILS_COND(while, 0)
#if defined(__GNUC__)
# define GCC_IS_ATLEAST(major, minor) ((__GNUC__ > major) || (__GNUC__ == major && __GNUC_MINOR__ >= minor))
#else
# define GCC_IS_ATLEAST(major, minor) 0
#endif
#if GCC_IS_ATLEAST(3, 0)
# define FUNC_PRINTF(fmt, args) __attribute__((format(printf, fmt, args)))
#else
# define FUNC_PRINTF(fmt, args)
#endif
#if GCC_IS_ATLEAST(2, 96)
# define FUNC_PURE __attribute__((pure))
#else
# define FUNC_PURE
#endif
#if GCC_IS_ATLEAST(2, 5)
# define FUNC_NORETURN __attribute__((noreturn))
#else
# define FUNC_NORETURN
#endif
#if GCC_IS_ATLEAST(3, 4)
# define FUNC_RETURNS __attribute__((warn_unused_result))
#else
# define FUNC_RETURNS
#endif
#define IS_POW2(x) (0 == ((x - 1) & x))
#define LENGTH(x) (sizeof x / sizeof *x)
#define MASK(lsb, msb) ((0xffffffff >> (32 - msb)) & \
    ((0xffffffff >> lsb) << lsb))
#if !defined(MAX)
# define MAX(a, b) (a > b ? a : b)
#endif
#if !defined(MIN)
# define MIN(a, b) (a < b ? a : b)
#endif
#define SQR(x) (x * x)
#define STRINGIFY(x) #x
#define STRINGIFY_VALUE(x) STRINGIFY(x)
#define SWAP(a, b) do {\
	uint8_t tmp_[sizeof a == sizeof b ? (signed)sizeof a : -1];\
	memmove(tmp_, &a, sizeof a);\
	memmove(&a, &b, sizeof a);\
	memmove(&b, tmp_, sizeof a);\
} HUTILS_COND(while, 0)
#if !defined(TRUE)
# define TRUE (0 == 0)
# define FALSE (!TRUE)
#endif
#define ZERO(x) memset(&x, 0, sizeof x)

#if defined(HCONF_WARNING_4127)
# define HUTILS_COND(stmt, cond)\
	__pragma(warning(push))\
	__pragma(warning(disable:4127))\
	stmt (cond)\
	__pragma(warning(pop))
#elif defined(HCONF_OTHERS)
# define HUTILS_COND(stmt, cond) stmt (cond)
#else
# error Not hconf:ed.
#endif

#define HCONF_TEST do {} HUTILS_COND(while, 0)

#endif
