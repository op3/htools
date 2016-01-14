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

#include <hutils/snprintf.h>

#if defined(HCONF_SNPRINTF_UNSAFE)

# include <stdarg.h>
# include <stdio.h>

int
snprintf_unsafe_(char *const a_dst, size_t const a_dst_size, char const *const
    a_fmt, ...)
{
	va_list args;
	size_t len;

	va_start(args, a_fmt);
	len = vsprintf(a_dst, a_fmt, args);
	if (a_dst_size < len) {
		fprintf(stderr, "Overrun in snprintf_unsafe_, abort!()\n");
		abort();
	}
	va_end(args);
	return len;
}

#endif
