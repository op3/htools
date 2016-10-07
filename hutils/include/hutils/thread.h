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

#ifndef HUTILS_THREAD_H
#define HUTILS_THREAD_H

#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hconf/include/hutils/thread.h>

#if defined(HCONF_mTHREAD_bST_OLD)
/* HCONF_LDFLAGS=-mthreads */
#	include <pthread.h>
#	define PTHREADS
#	if defined(HCONFING_mTHREAD_bST_OLD)
HCONF_TEST(int, (void))
{
	return pthread_attr_create(NULL);
}
#	endif
#elif defined(HCONF_mTHREAD_bST_NEW)
/* HCONF_LDFLAGS=-mthreads */
#	include <pthread.h>
#	define PTHREADS
#	if defined(HCONFING_mTHREAD_bST_NEW)
HCONF_TEST(int, (void))
{
	return pthread_attr_init(NULL);
}
#	endif
#elif defined(HCONF_mTHREAD_bPHTREAD)
/* HCONF_LIBS=-lpthread */
#	include <pthread.h>
#	define PTHREADS
#	if defined(HCONFING_mTHREAD_bPTHREAD)
HCONF_TEST(int, (void))
{
	return pthread_attr_init(NULL);
}
#	endif
#endif

CDECLS_BEGIN

#if defined(PTHREADS)
struct CondVar {
	pthread_cond_t	cond;
};
struct Mutex {
	pthread_mutex_t	mutex;
};
struct Thread {
	pthread_attr_t	attr;
	pthread_t	thread;
};
#elif defined(_MSC_VER)
#endif

int	thread_condvar_broadcast(struct CondVar *);
int	thread_condvar_clean(struct CondVar *);
int	thread_condvar_init(struct CondVar *) FUNC_RETURNS;
int	thread_condvar_signal(struct CondVar *);
int	thread_condvar_wait(struct CondVar *, struct Mutex *);
int	thread_mutex_clean(struct Mutex *);
int	thread_mutex_init(struct Mutex *) FUNC_RETURNS;
int	thread_mutex_lock(struct Mutex *);
int	thread_mutex_unlock(struct Mutex *);
int	thread_clean(struct Thread *);
int	thread_start(struct Thread *, void (*)(void *), void *) FUNC_RETURNS;

CDECLS_END

#endif
