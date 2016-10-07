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

#define N 1000

static void	runner(void *);
static void	runner_condvar(void *);
static void	runner_mutex(void *);

static struct Mutex g_mutex;
static struct CondVar g_condvar;
static int g_messenger;

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
		thread_mutex_lock(&g_mutex);
		++g_messenger;
		thread_condvar_broadcast(&g_condvar);
		thread_mutex_unlock(&g_mutex);
	}
}

void
runner_mutex(void *const a_dummy)
{
	int i;

	(void)a_dummy;
	for (i = 0; N > i; ++i) {
		thread_mutex_lock(&g_mutex);
		++g_messenger;
		thread_mutex_unlock(&g_mutex);
	}
}

HTEST(Runner)
{
	struct Thread t;

	g_messenger = 0;
	HTRY_BOOL(thread_start(&t, runner, (void *)31415));
	thread_clean(&t);
	HTRY_I(31415, ==, g_messenger);
}

HTEST(Mutex)
{
	struct Thread t;
	int i;

	HTRY_BOOL(thread_mutex_init(&g_mutex));
	g_messenger = 0;
	HTRY_BOOL(thread_start(&t, runner_mutex, NULL));
	for (i = 0; N > i; ++i) {
		int prev;

		thread_mutex_lock(&g_mutex);
		prev = g_messenger;
		++g_messenger;
		HTRY_I(g_messenger, ==, prev + 1);
		thread_mutex_unlock(&g_mutex);
	}
	thread_clean(&t);
	thread_mutex_clean(&g_mutex);
}

HTEST(ConditionVariable)
{
	struct Thread t;
	int previous_messenger;

	HTRY_BOOL(thread_condvar_init(&g_condvar));
	HTRY_BOOL(thread_mutex_init(&g_mutex));
	thread_mutex_lock(&g_mutex);
	g_messenger = 0;
	HTRY_BOOL(thread_start(&t, runner_condvar, NULL));
	while (N > g_messenger) {
		previous_messenger = g_messenger;
		while (g_messenger == previous_messenger) {
			thread_condvar_wait(&g_condvar, &g_mutex);
		}
		HTRY_I(previous_messenger, <, g_messenger);
	}
	thread_mutex_unlock(&g_mutex);
	thread_clean(&t);
	thread_mutex_clean(&g_mutex);
	thread_condvar_clean(&g_condvar);
}

HTEST_SUITE(Thread)
{
	HTEST_ADD(Runner);
	HTEST_ADD(Mutex);
	HTEST_ADD(ConditionVariable);
}
