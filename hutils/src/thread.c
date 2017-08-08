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

#include <hutils/thread.h>
#include <string.h>
#include <hutils/memory.h>

#if HCONF_mTHREAD_bST_OLD
#	define DO_PTHREADS 1
#	define PTHREAD_STACK_MIN THREAD_DEFAULT_STACK
#	define ATTR_CREATE(ret, a) ret = pthread_attr_create(&a)
#	define ATTR_DESTROY(a)
#	define THREAD_CREATE(ret, t)\
    ret = pthread_create(&t->thread, t->attr, run, starter)
#elif HCONF_mTHREAD_bST_NEW
#	define DO_PTHREADS 1
#	define PTHREAD_STACK_MIN THREAD_DEFAULT_STACK
#	define ATTR_CREATE(ret, a) ret = pthread_attr_init(&a)
#	define ATTR_DESTROY(a) pthread_attr_destroy(&a)
#	define THREAD_CREATE(ret, t)\
    ret = pthread_create(&t->thread, &t->attr, run, starter)
#elif HCONF_mTHREAD_bPTHREAD
#	define DO_PTHREADS 1
#	include <limits.h>
#	define ATTR_CREATE(ret, a) ret = pthread_attr_init(&a)
#	define ATTR_DESTROY(a) pthread_attr_destroy(&a)
#	define THREAD_CREATE(ret, t)\
    ret = pthread_create(&t->thread, &t->attr, run, starter)
#elif defined(_MSC_VER)
#	include <process.h>
#	include <hutils/string.h>

struct Starter {
	void	(*func)(void *);
	void	*data;
};

static void			print_last_error(void) FUNC_RETURNS;
static unsigned int __stdcall	run(void *);

void
print_last_error()
{
	LPTSTR str;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
	    GetLastError(), 0, (LPTSTR)&str, 0, NULL);
	hutils_warnx("%s", str);
	LocalFree(str);
}

unsigned int __stdcall
run(void *a_data)
{
	struct Starter starter;

	memmove(&starter, a_data, sizeof starter);
	FREE(a_data);
	starter.func(starter.data);
	return 0;
}

int
thread_condvar_broadcast(struct CondVar *a_condvar)
{
	WakeAllConditionVariable(&a_condvar->cv);
	return 1;
}

int
thread_condvar_clean(struct CondVar *a_condvar)
{
	(void)a_condvar;
	return 1;
}

int
thread_condvar_init(struct CondVar *a_condvar)
{
	InitializeConditionVariable(&a_condvar->cv);
	return 1;
}

int
thread_condvar_signal(struct CondVar *a_condvar)
{
	WakeConditionVariable(&a_condvar->cv);
	return 1;
}

int
thread_condvar_wait(struct CondVar *a_condvar, struct Mutex *a_mutex)
{
	if (0 == SleepConditionVariableCS(&a_condvar->cv, &a_mutex->cs,
	    INFINITE)) {
		print_last_error();
		return 0;
	}
	return 1;
}

int
thread_start(struct Thread *a_thread, void (*a_func)(void *), void *a_data)
{
	struct Starter *starter;
	uintptr_t ret;

	CALLOC(starter, 1);
	starter->func = a_func;
	starter->data = a_data;
	ret = _beginthreadex(NULL, 0, run, starter, 0, NULL);
	if (0 == ret) {
		FREE(starter);
		hutils_warn("_beginthreadex");
		return 0;
	}
	a_thread->handle = (HANDLE)ret;
	return 1;
}

int
thread_clean(struct Thread *a_thread)
{
	int ret;

	ret = 1;
	if (WAIT_FAILED == WaitForSingleObject(a_thread->handle, INFINITE)) {
		print_last_error();
		ret = 0;
	}
	if (!CloseHandle(a_thread->handle)) {
		print_last_error();
		ret = 0;
	}
	return ret;
}

int
thread_mutex_clean(struct Mutex *a_mutex)
{
	DeleteCriticalSection(&a_mutex->cs);
	return 1;
}

int
thread_mutex_init(struct Mutex *a_mutex)
{
	InitializeCriticalSectionAndSpinCount(&a_mutex->cs, 10);
	return 1;
}

int 
thread_mutex_lock(struct Mutex *a_mutex)
{
	EnterCriticalSection(&a_mutex->cs);
	return 1;
}

int
thread_mutex_unlock(struct Mutex *a_mutex)
{
	LeaveCriticalSection(&a_mutex->cs);
	return 1;
}

#endif

#if DO_PTHREADS

#	include <errno.h>

struct Starter {
	void	(*func)(void *);
	void	*data;
};

static void	*run(void *);

void *
run(void *a_data)
{
	struct Starter starter;

	memmove(&starter, a_data, sizeof starter);
	FREE(a_data);
	starter.func(starter.data);
	return NULL;
}

int
thread_condvar_broadcast(struct CondVar *a_condvar)
{
	if (0 != pthread_cond_broadcast(&a_condvar->cond)) {
		hutils_warn("pthread_cond_broadcast");
		return 0;
	}
	return 1;
}

int
thread_condvar_clean(struct CondVar *a_condvar)
{
	if (0 != pthread_cond_destroy(&a_condvar->cond)) {
		hutils_warn("pthread_cond_destroy");
		return 0;
	}
	return 1;
}

int
thread_condvar_init(struct CondVar *a_condvar)
{
	if (0 != pthread_cond_init(&a_condvar->cond, NULL)) {
		hutils_warn("pthread_cond_init");
		return 0;
	}
	return 1;
}

int
thread_condvar_signal(struct CondVar *a_condvar)
{
	if (0 != pthread_cond_signal(&a_condvar->cond)) {
		hutils_warn("pthread_cond_signal");
		return 0;
	}
	return 1;
}

int
thread_condvar_wait(struct CondVar *a_condvar, struct Mutex *a_mutex)
{
	if (0 != pthread_cond_wait(&a_condvar->cond, &a_mutex->mutex)) {
		hutils_warn("pthread_cond_wait");
		return 0;
	}
	return 1;
}

int
thread_mutex_clean(struct Mutex *a_mutex)
{
	if (0 != pthread_mutex_destroy(&a_mutex->mutex)) {
		hutils_warn("pthread_mutex_destroy");
		return 0;
	}
	return 1;
}

int
thread_mutex_init(struct Mutex *a_mutex)
{
	if (0 != pthread_mutex_init(&a_mutex->mutex, NULL)) {
		hutils_warn("pthread_mutex_init");
		return 0;
	}
	return 1;
}

int 
thread_mutex_lock(struct Mutex *a_mutex)
{
	if (0 != pthread_mutex_lock(&a_mutex->mutex)) {
		hutils_warn("pthread_mutex_lock");
		return 1;
	}
	return 0;
}

int
thread_mutex_unlock(struct Mutex *a_mutex)
{
	if (0 != pthread_mutex_unlock(&a_mutex->mutex)) {
		hutils_warn("pthread_mutex_unlock");
		return 1;
	}
	return 0;
}

int
thread_clean(struct Thread *a_thread)
{
	int ret;

	ret = 1;
	if (0 != pthread_join(a_thread->thread, NULL)) {
		hutils_warn("pthread_destroy");
		ret = 0;
	}
	ATTR_DESTROY(a_thread->attr);
	return ret;
}

int
thread_start(struct Thread *a_thread, void (*a_func)(void *), void *a_data)
{
	struct Starter *starter;
	int ret;

	CALLOC(starter, 1);
	starter->func = a_func;
	starter->data = a_data;
	ATTR_CREATE(ret, a_thread->attr);
	if (0 != ret) {
		goto thread_start_fail;
	}
	THREAD_CREATE(ret, a_thread);
	if (0 != ret) {
		goto thread_start_fail;
	}
	return 1;
thread_start_fail:
	hutils_warn("pthread_create");
	FREE(starter);
	return 0;
}

#endif
