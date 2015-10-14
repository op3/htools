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

#include <hutils/time.h>
#include <hconf/src/time.c.h>

#if defined(HCONF_POSIX)

# define SLEEP_NANOSLEEP
# define TIME_CLOCK

#elif defined(HCONF_WINDOWS)

# define SLEEP_SLEEP
# define TIME_PERF

#elif defined(HCONF_MAC)

# define SLEEP_NANOSLEEP
# define TIME_MACH

#else
# error Not hconf:ed.
#endif

#if defined(SLEEP_NANOSLEEP)
# include <errno.h>
# include <time.h>
#endif
#if defined(TIME_CLOCK)
# include <err.h>
# include <stdlib.h>
#endif
#if defined(TIME_MACH)
# include <err.h>
# include <stdlib.h>
# include <mach/mach_time.h>
#endif

int
hutils_sleep(double const a_s)
{
#if defined(SLEEP_SLEEP)
	Sleep((DWORD)(1e3 * a_s));
#elif defined(SLEEP_NANOSLEEP)
	struct timespec ts;

	ts.tv_sec = 0;
	ts.tv_nsec = 1e9 * a_s;
	if (0 != nanosleep(&ts, NULL)) {
		return errno;
	}
#endif
	return 0;
}

double
hutils_time()
{
#if defined(TIME_PERF)
	static double time_unit = -1.0;
	LARGE_INTEGER li;

	if (0.0 > time_unit) {
		assert(QueryPerformanceFrequency(&li));
		time_unit = 1.0 / li.QuadPart;
	}
	QueryPerformanceCounter(&li);
	return li.QuadPart * time_unit;
#elif defined(TIME_MACH)
	uint64_t mach_time;
	static double scaling_factor = -1.0;

	mach_time = mach_absolute_time();
	if (0.0 > scaling_factor) {
		mach_timebase_info_data_t timebase_info;
		kern_return_t ret;

		ret = mach_timebase_info(&timebase_info);
		if (0 != ret) {
			err(EXIT_FAILURE, "mach_timebase_info");
		}

		scaling_factor = 1e-9 * timebase_info.numer /
		    timebase_info.denom;
	}

	return mach_time * scaling_factor;
#elif defined(TIME_CLOCK)
	struct timespec tp;

	if (0 != clock_gettime(CLOCK_MONOTONIC, &tp)) {
		err(EXIT_FAILURE, "clock_gettime");
	}
	return tp.tv_sec + 1e-9 * tp.tv_nsec;
#else
# error "No time implementation on this platform!"
#endif
}
