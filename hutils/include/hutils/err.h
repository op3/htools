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

#ifndef HUTILS_ERR_H
#define HUTILS_ERR_H

#include <stdarg.h>
#include <hutils/funcattr.h>

typedef int (*HutilsPrinter)(char const *, va_list);

void	hutils_err(int, char const *, ...) FUNC_PRINTF(2, 3);
void	hutils_errc(int, int, char const *, ...) FUNC_PRINTF(3, 4);
void	hutils_errx(int, char const *, ...) FUNC_PRINTF(2, 3);
void	hutils_err_set_printer(HutilsPrinter);
void	hutils_verr(int, char const *, va_list);
void	hutils_verrc(int, int, char const *, va_list);
void	hutils_verrx(int, char const *, va_list);
void	hutils_vwarn(char const *, va_list);
void	hutils_vwarnc(int, char const *, va_list);
void	hutils_vwarnx(char const *, va_list);
void	hutils_warn(char const *, ...) FUNC_PRINTF(1, 2);
void	hutils_warnc(int, char const *, ...) FUNC_PRINTF(2, 3);
void	hutils_warnx(char const *, ...) FUNC_PRINTF(1, 2);

#endif
