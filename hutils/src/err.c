/*
 * Copyright (c) 2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/err.h>
#include <errno.h>
#include <stdlib.h>
#include <hutils/string.h>

static int	printer(char const *, ...);
static int	stdio(char const *, va_list);

static HutilsPrinter g_printer = stdio;

void
hutils_err(int a_eval, char const *a_fmt, ...)
{
	va_list args;
	int errno_;

	errno_ = errno;
	va_start(args, a_fmt);
	hutils_vwarnc(errno_, a_fmt, args);
	va_end(args);
	exit(a_eval);
}

void
hutils_errc(int a_eval, int a_code, char const *a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	hutils_vwarnc(a_code, a_fmt, args);
	va_end(args);
	exit(a_eval);
}

void
hutils_errx(int a_eval, char const *a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	hutils_vwarnx(a_fmt, args);
	va_end(args);
	exit(a_eval);
}

void
hutils_err_set_printer(HutilsPrinter a_printer)
{
	g_printer = NULL == a_printer ? stdio : a_printer;
}

void
hutils_verr(int a_eval, char const *a_fmt, va_list a_args)
{
	hutils_vwarnc(errno, a_fmt, a_args);
	exit(a_eval);
}

void
hutils_verrc(int a_eval, int a_code, char const *a_fmt, va_list a_args)
{
	hutils_vwarnc(a_code, a_fmt, a_args);
	exit(a_eval);
}

void
hutils_verrx(int a_eval, char const *a_fmt, va_list a_args)
{
	hutils_vwarnx(a_fmt, a_args);
	exit(a_eval);
}

void
hutils_vwarn(char const *a_fmt, va_list a_args)
{
	hutils_vwarnc(errno, a_fmt, a_args);
}

void
hutils_vwarnc(int a_code, char const *a_fmt, va_list a_args)
{
	g_printer(a_fmt, a_args);
	printer(": %s\n", strerror(a_code));
}

void
hutils_vwarnx(char const *a_fmt, va_list a_args)
{
	g_printer(a_fmt, a_args);
	printer("\n");
}

void
hutils_warn(char const *a_fmt, ...)
{
	va_list args;
	int errno_;

	errno_ = errno;
	va_start(args, a_fmt);
	hutils_vwarnc(errno_, a_fmt, args);
	va_end(args);
}

void
hutils_warnc(int a_code, char const *a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	hutils_vwarnc(a_code, a_fmt, args);
	va_end(args);
}

void
hutils_warnx(char const *a_fmt, ...)
{
	va_list args;
	int errno_;

	errno_ = errno;
	va_start(args, a_fmt);
	hutils_vwarnc(errno_, a_fmt, args);
	va_end(args);
}

int
printer(char const *a_fmt, ...)
{
	va_list args;
	int ret;

	va_start(args, a_fmt);
	ret = g_printer(a_fmt, args);
	va_end(args);
	return ret;
}

int
stdio(char const *a_fmt, va_list args)
{
	return fprintf(stderr, a_fmt, args);
}
