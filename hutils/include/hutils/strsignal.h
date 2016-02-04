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

#ifndef HUTILS_STRSIGNAL_H
#define HUTILS_STRSIGNAL_H

#include <hconf/include/hutils/strsignal.h>

#if defined(HCONF_STRSIGNAL_POSIX_200809)
/* CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
/* LIBS=dont */
#elif defined(HCONF_STRSIGNAL_PROTOTYPE)
char *strsignal(int);
#elif defined(HCONF_STRSIGNAL_CUSTOM)
# include <signal.h>
# define strsignal(signum) (\
	SIGABRT == signum ? "SIGABRT" :\
	SIGFPE == signum ? "SIGFPE" :\
	SIGILL == signum ? "SIGILL" :\
	SIGINT == signum ? "SIGINT" :\
	SIGSEGV == signum ? "SIGSEGV" :\
	SIGTERM == signum ? "SIGTERM" : "SIGUnknown")
#else
# error Not hconf:ed.
#endif

#include <string.h>

#define HCONF_TEST (void)strsignal(0)

#endif
