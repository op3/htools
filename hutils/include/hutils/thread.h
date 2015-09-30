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

#ifndef HUTILS_THREAD_H
#define HUTILS_THREAD_H

#include <hutils/macros.h>

struct CondVar;
struct Mutex;
struct Thread;

/* TODO: How to handle errors? */
struct CondVar	*thread_condvar_create(char **) FUNC_RETURNS;
int		thread_condvar_free(struct CondVar **, char **);
int		thread_condvar_signal(struct CondVar *, char **);
int		thread_condvar_wait(struct CondVar *, struct Mutex *, char
    **);
struct Thread	*thread_create(void (*)(void *), void *, char **)
	FUNC_RETURNS;
int		thread_free(struct Thread **, char **);
struct Mutex	*thread_mutex_create(char **) FUNC_RETURNS;
int		thread_mutex_free(struct Mutex **, char **);
int		thread_mutex_lock(struct Mutex *, char **);
int		thread_mutex_unlock(struct Mutex *, char **);

#endif
