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
#include <htest/htest.h>
#include <hconf/src/thread.h>

#define N 1000

static void	runner(void *);
static void	runner_condvar(void *);
static void	runner_mutex(void *);

static struct CondVar *g_condvar;
static struct Mutex *g_mutex;
static int g_messenger, g_task_exists;

void
runner(void *const a_data)
{
	g_messenger = (intptr_t)a_data;
}

void
runner_condvar(void *const a_dummy)
{
	(void)a_dummy;
	while (N > g_messenger) {
		thread_mutex_lock(g_mutex, NULL);
		while (g_task_exists) {
			thread_condvar_wait(g_condvar, g_mutex, NULL);
		}
		g_task_exists = 1;
		thread_condvar_broadcast(g_condvar, NULL);
		thread_mutex_unlock(g_mutex, NULL);
	}
}

void
runner_mutex(void *const a_dummy)
{
	int i;

	(void)a_dummy;
	for (i = 0; N > i; ++i) {
		thread_mutex_lock(g_mutex, NULL);
		++g_messenger;
		thread_mutex_unlock(g_mutex, NULL);
	}
}

HTEST(Runner)
{
	struct Thread *t;

	g_messenger = 0;
	t = thread_create(runner, (void *)31415, NULL);
	HTRY_PTR(NULL, !=, t);
	thread_free(&t, NULL);
	HTRY_PTR(NULL, ==, t);
	HTRY_I(31415, ==, g_messenger);
}

HTEST(Mutex)
{
	struct Thread *t;
	int i;

	g_mutex = thread_mutex_create(NULL);
	g_messenger = 0;
	t = thread_create(runner_mutex, NULL, NULL);
	for (i = 0; N > i; ++i) {
		int prev;

		thread_mutex_lock(g_mutex, NULL);
		prev = g_messenger;
		++g_messenger;
		HTRY_I(g_messenger, ==, prev + 1);
		thread_mutex_unlock(g_mutex, NULL);
	}
	thread_free(&t, NULL);
	thread_mutex_free(&g_mutex, NULL);
}

HTEST(ConditionVariable)
{
	struct Thread *t;

	g_condvar = thread_condvar_create(NULL);
	g_mutex = thread_mutex_create(NULL);
	g_task_exists = 0;
	t = thread_create(runner_condvar, NULL, NULL);
	for (g_messenger = 0; N > g_messenger; ++g_messenger) {
		thread_mutex_lock(g_mutex, NULL);
		while (!g_task_exists) {
			thread_condvar_wait(g_condvar, g_mutex, NULL);
		}
		HTRY_I(1, ==, g_task_exists);
		g_task_exists = 0;
		thread_condvar_broadcast(g_condvar, NULL);
		thread_mutex_unlock(g_mutex, NULL);
	}
	thread_free(&t, NULL);
	thread_mutex_free(&g_mutex, NULL);
	thread_condvar_free(&g_condvar, NULL);
}

HTEST_SUITE(Thread)
{
	HTEST_ADD(Runner);
	HTEST_ADD(Mutex);
#if !defined(HCONF_SINGLE)
	/* Waiting for external conditions in one thread doesn't work... */
	HTEST_ADD(ConditionVariable);
#endif
}
