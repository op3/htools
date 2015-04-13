/*
 * Copyright (c) 2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#ifndef HUTILS_COMMON_H
#define HUTILS_COMMON_H

#include <assert.h>
#include <stdint.h>
#include <string.h>

#define COPY(dst, src) do {\
	assert(sizeof dst == sizeof src);\
	memmove(&dst, &src, sizeof dst);\
} while (0)
#define IS_POW2(x) (0 == ((x) & ((x) - 1)))
#define LENGTH(x) (sizeof x / sizeof *x)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define SQR(x) ((x) * (x))
/* This swap gets optimized very efficiently for primitive types. */
#define SWAP(a, b) do {\
	uint8_t tmp_[sizeof a == sizeof b ? (signed)sizeof a : -1];\
	memmove(tmp_, &a, sizeof a);\
	memmove(&a, &b, sizeof a);\
	memmove(&b, tmp_, sizeof a);\
} while (0)
#define TRUNC(x, a, b) ((x) < (a) ? (a) : (x) > (b) ? (b) : (x))

#endif