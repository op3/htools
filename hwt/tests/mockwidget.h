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

#ifndef MOCKWIDGET_H
#define MOCKWIDGET_H

#include <hutils/funcattr.h>

struct HWT;
struct HWTEvent;
struct HWTRect;
struct HWTSize;
struct HWTWidget;

struct MockWidgetCallback {
	void	(*destroy)(void *);
	void	(*pull_min)(struct HWTSize *, void *);
	void	(*push_rect)(struct HWTRect const *, void *);
	void	(*respond)(struct HWTEvent const *, void *);
	void	*data;
};

struct HWTWidget	*mockwidget_create(struct HWT *, struct
    MockWidgetCallback const *) FUNC_RETURNS;
void			mockwidget_setup(struct HWT *);

#endif
