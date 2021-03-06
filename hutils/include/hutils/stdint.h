/*
 * Copyright (c) 2016-2017, 2019
 * Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#ifndef HUTILS_STDINT_H
#define HUTILS_STDINT_H

#include <hconf/include/hutils/stdint.h>
#include <stdint.h>
#include <stdlib.h>

#if HCONF_mSTDINT_LIMITS_bSTD_H
/* HCONF_OPT="nolink" */
#elif HCONF_mSTDINT_LIMITS_bLAST_RESORT
/* HCONF_OPT="nolink" */
#	undef SIZE_MAX
#	undef INT32_MAX
#	undef UINT32_MAX
#	undef INT64_MAX
#	undef UINT64_MAX
#	define INT32_MAX 2147483647L
#	define UINT32_MAX 4294967295UL
#	define INT64_MAX 9223372036854775807LL
#	define UINT64_MAX 18446744073709551615ULL
#	if __WORDSIZE == 64
#		define SIZE_MAX UINT64_MAX
#	else
#		define SIZE_MAX UINT32_MAX
#	endif
#endif
#if HCONFING_mSTDINT_LIMITS
#	define HCONF_TEST return 0 & SIZE_MAX
#endif

#if HCONF_mSTDINT_TYPES_bNOTHING
/* HCONF_OPT="nolink" */
#elif HCONF_mSTDINT_TYPES_bTYPEDEF_SYS_TYPES_H
/* HCONF_OPT="nolink" */
#	include <sys/types.h>
typedef u_int8_t  uint8_t;
typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;
typedef u_int64_t uint64_t;
typedef int32_t   intptr_t;
typedef uint32_t  uintptr_t;
#endif
#if HCONFING_mSTDINT_TYPES
#	define HCONF_TEST return 0 < hconf_test_()
static size_t hconf_test_(void)
{
	char pi8 [             1 ==    sizeof(int8_t) ? 1 : -1];
	char pu8 [             1 ==   sizeof(uint8_t) ? 1 : -1];
	char pi16[             2 ==   sizeof(int16_t) ? 1 : -1];
	char pu16[             2 ==  sizeof(uint16_t) ? 1 : -1];
	char pi32[             4 ==   sizeof(int32_t) ? 1 : -1];
	char pu32[             4 ==  sizeof(uint32_t) ? 1 : -1];
	char pi64[             8 ==   sizeof(int64_t) ? 1 : -1];
	char pu64[             8 ==  sizeof(uint64_t) ? 1 : -1];
	char pip [sizeof(void *) ==  sizeof(intptr_t) ? 1 : -1];
	char pup [sizeof(void *) == sizeof(uintptr_t) ? 1 : -1];
	return
	    sizeof(pi8) +
	    sizeof(pu8) +
	    sizeof(pi16) +
	    sizeof(pu16) +
	    sizeof(pi32) +
	    sizeof(pu32) +
	    sizeof(pi64) +
	    sizeof(pu64) +
	    sizeof(pip) +
	    sizeof(pup);
}
#endif

#endif
