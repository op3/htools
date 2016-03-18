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

#ifndef HUTILS_PRINTF_H
#define HUTILS_PRINTF_H

#include <stdio.h>
#include <hconf/include/hutils/fmtmod.h>

#if defined(HCONF_PRINTF_C90_ANCIENT)
# define PRIz "d"
# define PRIzx "x"
#elif defined(HCONF_PRINTF_C90_DECENT)
# define PRIz "ld"
# define PRIzx "lx"
#else
# error Not hconf:ed.
#endif

#define HCONF_TEST printf("%"PRIz"%"PRIzx, (size_t)0, (size_t)0)

#endif
