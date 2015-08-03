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

#define ABS(x) (0 > x ? -x : x)
#define CLAMP(x, a, b) (x < a ? a : x > b ? b : x)
#define COPY(dst, src) do {\
	assert(sizeof dst == sizeof src);\
	memmove(&dst, &src, sizeof dst);\
} HUTILS_COND(while, 0)
#define FUNC_NORETURN
#define FUNC_PRINTF(fmt, args)
#define FUNC_PURE
#define FUNC_RETURNS
#if defined(__GNUC_PREREQ)
# if __GNUC_PREREQ(2, 5)
#  undef FUNC_NORETURN
#  define FUNC_NORETURN __attribute__((noreturn))
# endif
# if __GNUC_PREREQ(2, 96)
#  undef FUNC_PURE
#  define FUNC_PURE __attribute__((pure))
# endif
# if __GNUC_PREREQ(3, 0)
#  undef FUNC_PRINTF
#  define FUNC_PRINTF(fmt, args) __attribute__((format(printf, fmt, args)))
# endif
# if __GNUC_PREREQ(3, 4)
#  undef FUNC_RETURNS
#  define FUNC_RETURNS __attribute__((warn_unused_result))
# endif
#endif
#define IS_POW2(x) (0 == (x & (x - 1)))
#define LENGTH(x) (sizeof x / sizeof *x)
#define MASK(lsb, msb) ((0xffffffff >> (32 - msb)) & \
    ((0xffffffff >> lsb) << lsb))
#define MAX(a, b) (a > b ? a : b)
#define MIN(a, b) (a < b ? a : b)
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

#if defined(_MSC_VER)
# include <windows.h>
# define HUTILS_COND(stmt, cond)\
	__pragma(warning(push))\
	__pragma(warning(disable:4127))\
	stmt (cond)\
	__pragma(warning(pop))
# define err(code, str) do {\
	LPTSTR str_;\
	DWORD err_;\
	err_ = GetLastError();\
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | \
		      FORMAT_MESSAGE_FROM_SYSTEM | \
		      FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err_,\
		      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),\
		      (LPTSTR)&str_, 0, NULL);\
	MessageBox(NULL, str_, NULL, MB_OK);\
	LocalFree(str_);\
	exit(code);\
} HUTILS_COND(while, 0)
#else
# define HUTILS_COND(stmt, cond) stmt (cond)
# if defined(__linux__) || defined(__OpenBSD__)
#  include <err.h>
# else
#  define err(code, str) do {\
	fprintf(stderr, "%s: %s\n", str, strerror(errno));\
	exit(code);\
} HUTILS_COND(while, 0)
# endif
#endif

#endif
