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

#elif defined(HCONF_mTHREAD_bWINDOWS)
#	include <process.h>

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

static unsigned int __stdcall	run(void *);

unsigned int __stdcall
run(void *a_data)
{
	struct Starter starter;

	memmove(&starter, a_data, sizeof starter);
	FREE(a_data);
	starter.func(starter.data);
	return 0;
}

struct Thread *
thread_create(void (*const a_func)(void *), void *const a_data)
{
	struct Starter *starter;
	struct Thread *thread;
	uintptr_t ret;

	CALLOC(starter, 1);
	starter->func = a_func;
	starter->data = a_data;
	ret = _beginthreadex(NULL, 0, run, starter, 0, NULL);
	if (-1 == ret) {
		FREE(starter);
		return NULL;
	}
	CALLOC(thread, 1);
	thread->handle = (HANDLE)ret;
	return thread;
}

int
thread_free(struct Thread **const a_thread)
{
	struct Thread *thread;

	thread = *a_thread;
	if (NULL == thread) {
		return;
	}
	WaitForSingleObject(thread->handle, INFINITE);
	CloseHandle(thread->handle);
	FREE(*a_thread);
}

struct Mutex *
thread_mutex_create()
{
	struct Mutex *mutex;

	CALLOC(mutex, 1);
	if (!InitializeCriticalSectionAndSpinCount(&mutex->cs, 0x00000400)) {
		return NULL;
	}
	return mutex;
}

int
thread_mutex_free(struct Mutex **const a_mutex)
{
	struct Mutex *mutex;

	mutex = *a_mutex;
	if (NULL == mutex) {
		return;
	}
	DeleteCriticalSection(&mutex->cs);
	FREE(*a_mutex);
}

int 
thread_mutex_lock(struct Mutex *const a_mutex)
{
	EnterCriticalSection(&a_mutex->cs);
}

int
thread_mutex_unlock(struct Mutex *const a_mutex)
{
	LeaveCriticalSection(&a_mutex->cs);
}

#endif

#if DO_PTHREADS

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
thread_condvar_create(char **const a_err)
{
	struct CondVar *condvar;
	int ret;

	CALLOC(condvar, 1);
	ret = pthread_cond_init(&condvar->cond, NULL);
	if (0 != ret) {
		if (NULL != a_err) {
			*a_err = strdup(strerror(ret));
		}
		FREE(condvar);
	}
	return condvar;
}

int
thread_condvar_free(struct CondVar **const a_condvar, char **const a_err)
{
	struct CondVar *condvar;
	int ret;

	condvar = *a_condvar;
	if (NULL == condvar) {
		return 1;
	}
	ret = pthread_cond_destroy(&condvar->cond);
	FREE(*a_condvar);
	if (0 != ret) {
		if (NULL != a_err) {
			*a_err = strdup(strerror(ret));
		}
		return 0;
	}
	return 1;
}

int
thread_condvar_broadcast(struct CondVar *const a_condvar, char **const a_err)
{
	int ret;

	ret = pthread_cond_broadcast(&a_condvar->cond);
	if (0 != ret) {
		if (NULL != a_err) {
			*a_err = strdup(strerror(ret));
		}
		return 0;
	}
	return 1;
}

int
thread_condvar_signal(struct CondVar *const a_condvar, char **const a_err)
{
	int ret;

	ret = pthread_cond_signal(&a_condvar->cond);
	if (0 != ret) {
		if (NULL != a_err) {
			*a_err = strdup(strerror(ret));
		}
		return 0;
	}
	return 1;
}

int
thread_condvar_wait(struct CondVar *const a_condvar, struct Mutex *const
    a_mutex, char **const a_err)
{
	int ret;

	ret = pthread_cond_wait(&a_condvar->cond, &a_mutex->mutex);
	if (0 != ret) {
		if (NULL != a_err) {
			*a_err = strdup(strerror(ret));
		}
		return 0;
	}
	return 1;
}

struct Thread *
thread_create(void (*const a_func)(void *), void *const a_data, char **const
    a_err)
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
	if (NULL != a_err) {
		*a_err = strdup(strerror(ret));
	}
	FREE(starter);
	FREE(thread);
	return NULL;
}

int
thread_free(struct Thread **const a_thread, char **const a_err)
{
	struct Thread *thread;
	int ret;

	thread = *a_thread;
	if (NULL == thread) {
		return 1;
	}
	ret = pthread_join(thread->thread, NULL);
	ATTR_DESTROY(thread);
	FREE(*a_thread);
	if (0 != ret) {
		if (NULL != a_err) {
			*a_err = strdup(strerror(ret));
		}
		return 0;
	}
	return 1;
}

struct Mutex *
thread_mutex_create(char **const a_err)
{
	struct Mutex *mutex;
	int ret;

	CALLOC(mutex, 1);
	ret = pthread_mutex_init(&mutex->mutex, NULL);
	if (0 != ret) {
		if (NULL != a_err) {
			*a_err = strdup(strerror(ret));
		}
		FREE(mutex);
	}
	return mutex;
}

int
thread_mutex_free(struct Mutex **const a_mutex, char **const a_err)
{
	struct Mutex *mutex;
	int ret;

	mutex = *a_mutex;
	if (NULL == mutex) {
		return 1;
	}
	ret = pthread_mutex_destroy(&mutex->mutex);
	FREE(*a_mutex);
	if (0 != ret) {
		if (NULL != a_err) {
			*a_err = strdup(strerror(ret));
		}
		return 0;
	}
	return 1;
}

int 
thread_mutex_lock(struct Mutex *const a_mutex, char **const a_err)
{
	int ret;

	ret = pthread_mutex_lock(&a_mutex->mutex);
	if (0 != ret) {
		if (NULL != a_err) {
			*a_err = strdup(strerror(ret));
		}
		return 1;
	}
	return 0;
}

int
thread_mutex_unlock(struct Mutex *const a_mutex, char **const a_err)
{
	int ret;

	ret = pthread_mutex_unlock(&a_mutex->mutex);
	if (0 != ret) {
		if (NULL != a_err) {
			*a_err = strdup(strerror(ret));
		}
		return 1;
	}
	return 0;
}

#endif
