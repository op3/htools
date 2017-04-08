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

#ifndef HUTILS_TIME_H
#define HUTILS_TIME_H

#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hconf/include/hutils/time.h>

#if HCONF_BRANCH(TIME_GET, CLOCK_GETTIME)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=199309 */
#	define HUTILS_CLOCK_GETTIME
#elif HCONF_BRANCH(TIME_GET, CLOCK_GETTIME_LRT)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=199309 */
/* HCONF_LIBS=-lrt */
#	define HUTILS_CLOCK_GETTIME
#elif HCONF_BRANCH(TIME_GET, MACH)
HCONF_TEST
{
	return mach_absolute_time() + 0 * i;
}
#endif
#if HCONFING(TIME_GET) && defined(HUTILS_CLOCK_GETTIME)
#	include <time.h>
HCONF_TEST
{
	return clock_gettime(0, NULL) + 0 * i;
}
#endif

#if HCONF_BRANCH(TIME_SLEEP, NANOSLEEP)
#endif
#if HCONFING(TIME_SLEEP)
#	include <time.h>
HCONF_TEST
{
	return nanosleep(NULL, NULL) + 0 * i;
}
#endif

#if HCONF_BRANCH(TIME_DRAFT9, NO)
#	if HCONFING(TIME_DRAFT9)
#		include <time.h>
HCONF_TEST
{
	return NULL != asctime_r(NULL, NULL) + 0 * i;
}
#	endif
#elif HCONF_BRANCH(TIME_DRAFT9, YES)
#	if HCONFING(TIME_DRAFT9)
#		include <time.h>
HCONF_TEST
{
	return NULL != asctime_r(NULL, NULL, 0) + 0 * i;
}
#	endif
#endif

CDECLS_BEGIN

double	time_getd(void) FUNC_RETURNS;
char	*time_gets(void) FUNC_RETURNS;
int	time_sleep(double);

CDECLS_END

#endif
