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

#include <hutils/thread.h>
#include <string.h>
#include <hutils/memory.h>

#if defined(HCONF_mTHREAD_bST_OLD)
#	define DO_PTHREADS 1
#	define PTHREAD_STACK_MIN THREAD_DEFAULT_STACK
#	define ATTR_CREATE(ret, t) ret = pthread_attr_create(&t->attr)
#	define ATTR_DESTROY(t)
#	define STACK_GET_SIZE(ret, stack_size, t) do {\
	ret = 0;\
	stack_size = pthread_attr_getstacksize(&t->attr);\
} WHILE_0
#	define THREAD_CREATE(ret, t)\
    ret = pthread_create(&t->thread, t->attr, run, starter)

#elif defined(HCONF_mTHREAD_bST_NEW)
#	define DO_PTHREADS 1
#	define PTHREAD_STACK_MIN THREAD_DEFAULT_STACK
#	define ATTR_CREATE(ret, t) ret = pthread_attr_init(&t->attr)
#	define ATTR_DESTROY(t) pthread_attr_destroy(&t->attr)
#	define STACK_GET_SIZE(ret, stack_size, t) do {\
	stack_size = 0;\
	ret = pthread_attr_getstacksize(&t->attr, &stack_size);\
} WHILE_0
#	define THREAD_CREATE(ret, t)\
    ret = pthread_create(&t->thread, &t->attr, run, starter)

#elif defined(HCONF_mTHREAD_bPHTREAD)
#	define DO_PTHREADS 1
#	include <limits.h>
#	define ATTR_CREATE(ret, t) ret = pthread_attr_init(&t->attr)
#	define ATTR_DESTROY(t) pthread_attr_destroy(&t->attr)
#	define STACK_GET_SIZE(ret, stack_size, t) do {\
	stack_size = 0;\
	ret = pthread_attr_getstacksize(&t->attr, &stack_size);\
} WHILE_0
#	define THREAD_CREATE(ret, t)\
    ret = pthread_create(&t->thread, &t->attr, run, starter)

#elif defined(_MSC_VER)
#	include <process.h>
#	include <hutils/string.h>

struct CondVar {
	CONDITION_VARIABLE	cv;
};
struct Mutex {
	CRITICAL_SECTION	cs;
};
struct Starter {
	void	(*func)(void *);
	void	*data;
};
struct Thread {
	HANDLE	handle;
};

static char			*dup_last_error() FUNC_RETURNS;
static unsigned int __stdcall	run(void *);

char *
dup_last_error()
{
	LPTSTR str;
	char *cstr;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
	    GetLastError(), 0, (LPTSTR)&str, 0, NULL);
	cstr = strdup(str);
	LocalFree(str);
	return cstr;
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

struct CondVar *
thread_condvar_create()
{
	struct CondVar *condvar;

	CALLOC(condvar, 1);
	InitializeConditionVariable(&condvar->cv);
	return condvar;
}

int
thread_condvar_free(struct CondVar **a_condvar)
{
	if (*a_condvar) {
		FREE(*a_condvar);
	}
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
	if (!SleepConditionVariableCS(&a_condvar->cv, &a_mutex->cs, INFINITE))
	{
		print_last_error();
		return 0;
	}
	return 1;
}

struct Thread *
thread_create(void (*a_func)(void *), void *a_data)
{
	struct Starter *starter;
	struct Thread *thread;
	uintptr_t ret;

	CALLOC(starter, 1);
	starter->func = a_func;
	starter->data = a_data;
	ret = _beginthreadex(NULL, 0, run, starter, 0, NULL);
	if (0 == ret) {
		FREE(starter);
		hutils_warn("_beginthreadex");
		return NULL;
	}
	CALLOC(thread, 1);
	thread->handle = (HANDLE)ret;
	return thread;
}

int
thread_free(struct Thread **a_thread)
{
	struct Thread *thread;
	int ret;

	thread = *a_thread;
	if (NULL == thread) {
		return 1;
	}
	ret = 1;
	if (WAIT_FAILED == WaitForSingleObject(thread->handle, INFINITE)) {
		print_last_error();
		ret = 0;
	}
	if (!CloseHandle(thread->handle)) {
		print_last_error();
		ret = 0;
	}
	FREE(*a_thread);
	return ret;
}

struct Mutex *
thread_mutex_create()
{
	struct Mutex *mutex;

	CALLOC(mutex, 1);
	InitializeCriticalSectionAndSpinCount(&mutex->cs, 0x00000400);
	return mutex;
}

int
thread_mutex_free(struct Mutex **a_mutex)
{
	struct Mutex *mutex;

	mutex = *a_mutex;
	if (NULL == mutex) {
		return 1;
	}
	DeleteCriticalSection(&mutex->cs);
	FREE(*a_mutex);
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

#include <errno.h>

struct CondVar {
	pthread_cond_t	cond;
};
struct Mutex {
	pthread_mutex_t	mutex;
};
struct Starter {
	void	(*func)(void *);
	void	*data;
};
struct Thread {
	pthread_attr_t	attr;
	pthread_t	thread;
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

struct CondVar *
thread_condvar_create()
{
	struct CondVar *condvar;
	int ret;

	CALLOC(condvar, 1);
	ret = pthread_cond_init(&condvar->cond, NULL);
	if (0 != ret) {
		hutils_warn("pthread_cond_init");
		FREE(condvar);
	}
	return condvar;
}

int
thread_condvar_free(struct CondVar **a_condvar)
{
	struct CondVar *condvar;
	int ret, errno_;

	condvar = *a_condvar;
	if (NULL == condvar) {
		return 1;
	}
	ret = pthread_cond_destroy(&condvar->cond);
	errno_ = errno;
	FREE(*a_condvar);
	if (0 != ret) {
		hutils_warnc(errno_, "pthread_cond_destroy");
		return 0;
	}
	return 1;
}

int
thread_condvar_broadcast(struct CondVar *a_condvar)
{
	int ret;

	ret = pthread_cond_broadcast(&a_condvar->cond);
	if (0 != ret) {
		hutils_warn("pthread_cond_broadcast");
		return 0;
	}
	return 1;
}

int
thread_condvar_signal(struct CondVar *a_condvar)
{
	int ret;

	ret = pthread_cond_signal(&a_condvar->cond);
	if (0 != ret) {
		hutils_warn("pthread_cond_signal");
		return 0;
	}
	return 1;
}

int
thread_condvar_wait(struct CondVar *a_condvar, struct Mutex *a_mutex)
{
	int ret;

	ret = pthread_cond_wait(&a_condvar->cond, &a_mutex->mutex);
	if (0 != ret) {
		hutils_warn("pthread_cond_wait");
		return 0;
	}
	return 1;
}

struct Thread *
thread_create(void (*a_func)(void *), void *a_data)
{
	struct Starter *starter;
	struct Thread *thread;
	int ret;

	CALLOC(starter, 1);
	starter->func = a_func;
	starter->data = a_data;
	CALLOC(thread, 1);
	ATTR_CREATE(ret, thread);
	if (0 != ret) {
		goto thread_create_fail;
	}
	THREAD_CREATE(ret, thread);
	if (0 != ret) {
		goto thread_create_fail;
	}
	return thread;
thread_create_fail:
	hutils_warn("pthread_create");
	FREE(starter);
	FREE(thread);
	return NULL;
}

int
thread_free(struct Thread **a_thread)
{
	struct Thread *thread;
	int ret, errno_;

	thread = *a_thread;
	if (NULL == thread) {
		return 1;
	}
	ret = pthread_join(thread->thread, NULL);
	errno_ = errno;
	ATTR_DESTROY(thread);
	FREE(*a_thread);
	if (0 != ret) {
		hutils_warnc(errno_, "pthread_destroy");
		return 0;
	}
	return 1;
}

struct Mutex *
thread_mutex_create()
{
	struct Mutex *mutex;
	int ret;

	CALLOC(mutex, 1);
	ret = pthread_mutex_init(&mutex->mutex, NULL);
	if (0 != ret) {
		hutils_warn("pthread_mutex_init");
		FREE(mutex);
	}
	return mutex;
}

int
thread_mutex_free(struct Mutex **a_mutex)
{
	struct Mutex *mutex;
	int ret, errno_;

	mutex = *a_mutex;
	if (NULL == mutex) {
		return 1;
	}
	ret = pthread_mutex_destroy(&mutex->mutex);
	errno_ = errno;
	FREE(*a_mutex);
	if (0 != ret) {
		hutils_warnc(errno_, "pthread_mutex_destroy");
		return 0;
	}
	return 1;
}

int 
thread_mutex_lock(struct Mutex *a_mutex)
{
	int ret;

	ret = pthread_mutex_lock(&a_mutex->mutex);
	if (0 != ret) {
		hutils_warn("pthread_mutex_lock");
		return 1;
	}
	return 0;
}

int
thread_mutex_unlock(struct Mutex *a_mutex)
{
	int ret;

	ret = pthread_mutex_unlock(&a_mutex->mutex);
	if (0 != ret) {
		hutils_warn("pthread_mutex_unlock");
		return 1;
	}
	return 0;
}

#endif
