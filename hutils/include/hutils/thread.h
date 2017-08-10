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

#ifndef HUTILS_THREAD_H
#define HUTILS_THREAD_H

#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hconf/include/hutils/thread.h>

#if HCONF_mTHREAD_bST_OLD
/* HCONF_LDFLAGS=-mthreads */
#	include <pthread.h>
#	define PTHREADS
#	if HCONFING_mTHREAD
#		define HCONF_TEST return hconf_test_()
static int hconf_test_(void) {
	pthread_attr_t a;
	return pthread_attr_create(&a);
}
#	endif
#elif HCONF_mTHREAD_bST_NEW
/* HCONF_LDFLAGS=-mthreads */
#	include <pthread.h>
#	define PTHREADS
#	if HCONFING_mTHREAD
#		define HCONF_TEST return hconf_test_()
static void *runner_(void *a_dummy) { return a_dummy; }
static int hconf_test_(void) {
	pthread_t thread;
	return pthread_create(&thread, NULL, runner_, NULL);
}
#	endif
#elif HCONF_mTHREAD_bPTHREAD
/* HCONF_CFLAGS=-pthread */
/* HCONF_LIBS=-pthread */
#	include <pthread.h>
#	define PTHREADS
#	if HCONFING_mTHREAD
#		define HCONF_TEST return hconf_test_()
static void *runner_(void *a_dummy) { return a_dummy; }
static int hconf_test_(void) {
	pthread_t thread;
	return pthread_create(&thread, NULL, runner_, NULL);
}
#	endif
#endif

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
#	include <windows.h>
struct CondVar {
	CONDITION_VARIABLE	cv;
};
struct Mutex {
	CRITICAL_SECTION	cs;
};
struct Thread {
	HANDLE	handle;
};
#endif

CDECLS_BEGIN

int	thread_condvar_broadcast(struct CondVar *) FUNC_NONNULL(());
int	thread_condvar_clean(struct CondVar *) FUNC_NONNULL(());
int	thread_condvar_init(struct CondVar *) FUNC_NONNULL(()) FUNC_RETURNS;
int	thread_condvar_signal(struct CondVar *) FUNC_NONNULL(());
int	thread_condvar_wait(struct CondVar *, struct Mutex *)
	FUNC_NONNULL(());
int	thread_mutex_clean(struct Mutex *) FUNC_NONNULL(());
int	thread_mutex_init(struct Mutex *) FUNC_NONNULL(()) FUNC_RETURNS;
int	thread_mutex_lock(struct Mutex *) FUNC_NONNULL(());
int	thread_mutex_unlock(struct Mutex *) FUNC_NONNULL(());
int	thread_clean(struct Thread *) FUNC_NONNULL(());
int	thread_start(struct Thread *, void (*)(void *), void *)
	FUNC_NONNULL((1, 2)) FUNC_RETURNS;

CDECLS_END

#endif
