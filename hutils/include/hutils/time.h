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

#	include <time.h>
#include <hconf/include/hutils/time.h>
#include <hutils/funcattr.h>

#if defined(HCONF_mTIME_bPOSIX_MONOTONIC)
#	define SLEEP_NANOSLEEP
#	define TIME_CLOCK
#	define CLOCK_SOURCE CLOCK_MONOTONIC
#	if defined(HCONFING_mTIME_bPOSIX_MONOTONIC)
HCONF_TEST(int, (void))
{
	asctime_r(NULL, NULL);
	return clock_gettime(CLOCK_SOURCE, NULL);
}
#	endif
#elif defined(HCONF_mTIME_bPOSIX_MONOTONIC_LRT)
/* HCONF_LIBS=-lrt */
#	define SLEEP_NANOSLEEP
#	define TIME_CLOCK
#	define CLOCK_SOURCE CLOCK_MONOTONIC
#	if defined(HCONFING_mTIME_bPOSIX_MONOTONIC_LRT)
HCONF_TEST(int, (void))
{
	asctime_r(NULL, NULL);
	return clock_gettime(CLOCK_SOURCE, NULL);
}
#	endif
#elif defined(HCONF_mTIME_bPOSIX_REALTIME)
#	define SLEEP_NANOSLEEP
#	define TIME_CLOCK
#	define CLOCK_SOURCE CLOCK_REALTIME
#	if defined(HCONFING_mTIME_bPOSIX_REALTIME)
HCONF_TEST(int, (void))
{
	asctime_r(NULL, NULL);
	return clock_gettime(CLOCK_SOURCE, NULL);
}
#	endif
#elif defined(HCONF_mTIME_bPOSIX_REALTIME_DRAFT9)
#	define SLEEP_NANOSLEEP
#	define TIME_CLOCK
#	define CLOCK_SOURCE CLOCK_REALTIME
#	if defined(HCONFING_mTIME_bPOSIX_REALTIME_DRAFT9)
HCONF_TEST(int, (void))
{
	asctime_r(NULL, NULL, 0);
	return clock_gettime(CLOCK_SOURCE, NULL);
}
#	endif
#elif defined(HCONF_mTIME_bWINDOWS)
#	define SLEEP_SLEEP
#	define TIME_PERF 1
#	if defined(HCONFING_mTIME_bWINDOWS)
HCONF_TEST(BOOL, (void))
{
	return QueryPerformanceCounter(NULL);
}
#	endif
#elif defined(HCONF_mTIME_bMAC)
#	define SLEEP_NANOSLEEP
#	define TIME_MACH
#	if defined(HCONFING_mTIME_bMAC)
HCONF_TEST(uint64_t, (void))
{
	return mach_absolute_time();
}
#	endif
#endif

double	time_getd(void) FUNC_RETURNS;
char	*time_gets(void) FUNC_RETURNS;
int	time_sleep(double);

#endif
