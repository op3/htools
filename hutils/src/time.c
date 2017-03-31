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

#include <hutils/time.h>
#include <hutils/memory.h>

#if defined(HUTILS_CLOCK_GETTIME)
#	include <stdlib.h>
#	include <time.h>
#	include <hutils/err.h>
#	include <hutils/thread.h>
#elif defined(HCONF_mTIME_GET_bMACH)
#	include <stdlib.h>
#	include <mach/mach_time.h>
#	include <hutils/err.h>
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
	enum MutexState {
		MUTEX_UNINITED,
		MUTEX_OK,
		MUTEX_FAIL
	};
	static enum MutexState s_mutex_state = MUTEX_UNINITED;
	static struct Mutex s_mutex;
	clockid_t s_clockid =
#	if defined(CLOCK_MONOTONIC_RAW)
	    CLOCK_MONOTONIC_RAW;
#	elif defined(CLOCK_MONOTONIC)
	    CLOCK_MONOTONIC;
#	else
	    CLOCK_REALTIME;
#	endif
	struct timespec tp;

	if (MUTEX_UNINITED == s_mutex_state) {
		s_mutex_state = thread_mutex_init(&s_mutex) ? MUTEX_OK :
		    MUTEX_FAIL;
		/*
		 * If the mutex init failed, then we shouldn't see more than
		 * one thread entering here, so let's keep going without using
		 * the mutex.
		 */
	}
	if (MUTEX_OK == s_mutex_state) {
		thread_mutex_lock(&s_mutex);
	}
	for (;;) {
		if (0 == clock_gettime(s_clockid, &tp)) {
			break;
		}
#define TRANSITION(from, to) do {\
	if (from == s_clockid) {\
		s_clockid = to;\
		continue;\
	}\
} WHILE_0
#	if defined(CLOCK_MONOTONIC_RAW)
		TRANSITION(CLOCK_MONOTONIC_RAW, CLOCK_MONOTONIC);
#	elif defined(CLOCK_MONOTONIC)
		TRANSITION(CLOCK_MONOTONIC, CLOCK_REALTIME);
#	endif
		hutils_err(EXIT_FAILURE, "clock_gettime");
	}
	if (MUTEX_OK == s_mutex_state) {
		thread_mutex_unlock(&s_mutex);
	}
	return tp.tv_sec + 1e-9 * tp.tv_nsec;
#elif defined(HCONF_mTIME_GET_bMACH)
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
		if (!QueryPerformanceFrequency(&li)) {
			hutils_errx(EXIT_FAILURE,
			    "QueryPerformanceFrequency");
		}
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
	buf = malloc(26);
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
