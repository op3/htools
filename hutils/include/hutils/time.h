/*
 * Copyright (c) 2015-2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#ifndef HUTILS_TIME_H
#define HUTILS_TIME_H

#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hconf/include/hutils/time.h>

CDECLS_BEGIN

#if defined(HCONF_mTIME_GET_bCLOCK_GETTIME)
#	define HUTILS_CLOCK_GETTIME
#elif defined(HCONF_mTIME_GET_bCLOCK_GETTIME_LRT)
/* HCONF_LIBS=-lrt */
#	define HUTILS_CLOCK_GETTIME
#elif defined(HCONF_mTIME_GET_bMACH)
#	if defined(HCONFING_mTIME_GET)
HCONF_TEST(uint64_t, (void))
{
	return mach_absolute_time();
}
#	endif
#endif
#if defined(HCONFING_mTIME_GET) && defined(CLOCK_GETTIME)
HCONF_TEST(int, (void))
{
	return clock_gettime(0, NULL);
}
#endif

#if defined(HCONF_mTIME_SLEEP_bNANOSLEEP)
#	if defined(HCONFING_mTIME_SLEEP)
#		include <time.h>
HCONF_TEST(int, (struct timespec ts))
{
	return nanosleep(&ts, NULL);
}
#	endif
#endif

#if defined(HCONF_mTIME_DRAFT9_bNO)
#	if defined(HCONFING_mTIME_DRAFT9)
#		include <time.h>
HCONF_TEST(char *, (void))
{
	return asctime_r(NULL, NULL);
}
#	endif
#elif defined(HCONF_mTIME_DRAFT9_bYES)
#	if defined(HCONFING_mTIME_DRAFT9)
#		include <time.h>
HCONF_TEST(char *, (void))
{
	return asctime_r(NULL, NULL, 0);
}
#	endif
#endif

double	time_getd(void) FUNC_RETURNS;
char	*time_gets(void) FUNC_RETURNS;
int	time_sleep(double);

CDECLS_END

#endif
