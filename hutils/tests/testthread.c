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

#include <hutils/thread.h>
#include <htest/htest.h>

#define N 100

static void	runner(void *);
static void	runner_mutex(void *);

static struct Mutex *g_mutex;
static int g_flag;

void
runner(void *const a_data)
{
	g_flag = (intptr_t)a_data;
}

void
runner_mutex(void *const a_dummy)
{
	int i;

	(void)a_dummy;
	for (i = 0; N > i; ++i) {
		thread_mutex_lock(g_mutex);
		++g_flag;
		thread_mutex_unlock(g_mutex);
	}
}

HTEST(Runner)
{
	struct Thread *t;

	g_flag = 0;
	t = thread_create(runner, (void *)31415);
	HTRY_PTR(NULL, !=, t);
	thread_free(&t);
	HTRY_PTR(NULL, ==, t);
	HTRY_I(31415, ==, g_flag);
}

HTEST(Mutex)
{
	struct Thread *t;
	int i;

	g_mutex = thread_mutex_create();
	g_flag = 0;

	t = thread_create(runner_mutex, NULL);
	for (i = 0; N > i; ++i) {
		int prev;

		thread_mutex_lock(g_mutex);
		prev = g_flag;
		++g_flag;
		HTRY_I(g_flag, ==, prev + 1);
		thread_mutex_unlock(g_mutex);
	}
	thread_free(&t);

	thread_mutex_free(&g_mutex);
}

HTEST_SUITE(Thread)
{
	HTEST_ADD(Runner);
	HTEST_ADD(Mutex);
}
