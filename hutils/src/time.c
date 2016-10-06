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

#include <hutils/time.h>
#include <hutils/memory.h>

#if defined(HUTILS_CLOCK_GETTIME)
#	include <stdlib.h>
#	include <time.h>
#	include <hutils/err.h>
#elif defined(HCONF_mTIME_GET_bMACH)
#	include <stdlib.h>
#	include <mach/mach_time.h>
#	include <hutils/err.h>
#elif defined(_MSC_VER)
#	include <assert.h>
#endif

#if defined(HCONF_mTIME_DRAFT9_bNO)
#	define ASCTIME_R(tm, buf) asctime_r(tm, buf)
#	define GMTIME_R(tt, tm) gmtime_r(tt, tm)
#elif defined(HCONF_mTIME_DRAFT9_bYES)
#	define ASCTIME_R(tm, buf) asctime_r(tm, buf, 26)
#	define GMTIME_R(tt, tm) gmtime_r(tm, tt)
#elif defined(_MSC_VER)
#	define ASCTIME_R(tm, buf) asctime_s(buf, 26, tm)
#	define GMTIME_R(tt, tm) gmtime_s(tm, tt)
#endif

double
time_getd()
{
#if defined(HUTILS_CLOCK_GETTIME)
	clockid_t const c_clockid[] = {
#	if defined(CLOCK_MONOTONIC_RAW)
		CLOCK_MONOTONIC_RAW,
#	endif
		CLOCK_MONOTONIC,
		CLOCK_REALTIME
	};
	static size_t s_clocki = 0;
	struct timespec tp;

	for (;;) {
		if (0 == clock_gettime(c_clockid[s_clocki], &tp)) {
			break;
		}
		++s_clocki;
		if (LENGTH(c_clockid) <= s_clocki) {
			hutils_err(EXIT_FAILURE, "clock_gettime");
		}
	}
	return tp.tv_sec + 1e-9 * tp.tv_nsec;
#elif defined(TIME_MACH)
	uint64_t mach_time;
	static double scaling_factor = -1.0;

	mach_time = mach_absolute_time();
	if (0.0 > scaling_factor) {
		mach_timebase_info_data_t timebase_info;
		kern_return_t ret;

		ret = mach_timebase_info(&timebase_info);
		if (0 != ret) {
			hutils_err(EXIT_FAILURE, "mach_timebase_info");
		}
		scaling_factor = 1e-9 * timebase_info.numer /
		    timebase_info.denom;
	}

	return mach_time * scaling_factor;
#elif defined(_MSC_VER)
	static double time_unit = -1.0;
	LARGE_INTEGER li;

	if (0.0 > time_unit) {
		assert(QueryPerformanceFrequency(&li));
		time_unit = 1.0 / li.QuadPart;
	}
	QueryPerformanceCounter(&li);
	return li.QuadPart * time_unit;
#endif
}

char *
time_gets()
{
	struct tm tm;
	time_t tt;
	char *buf;

	time(&tt);
	GMTIME_R(&tt, &tm);
	MALLOC(buf, 26);
	ASCTIME_R(&tm, buf);
	return buf;
}

int
time_sleep(double a_s)
{
#if defined(HCONF_mTIME_SLEEP_bNANOSLEEP)
	struct timespec ts;

	ts.tv_sec = 0;
	ts.tv_nsec = 1e9 * a_s;
	if (0 != nanosleep(&ts, NULL)) {
		hutils_warn("nanosleep");
		return 0;
	}
#elif defined(_MSC_VER)
	Sleep((DWORD)(1e3 * a_s));
#endif
	return 1;
}
