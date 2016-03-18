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

#include <src/widget.h>
#include <assert.h>
#include <hutils/memory.h>
#include <hwt/hwt.h>
#include <src/holder.h>

void
hwt_widget_init(struct HWTWidget *const a_widget, struct HWTWidgetType const
    *const a_type)
{
	assert(NULL != a_type);
	a_widget->type = a_type;
	a_widget->owner = NULL;
}

void
hwt_widget_push_rect(struct HWT *const a_hwt, struct HWTWidget *const
    a_widget, struct HWTRect const *const a_rect)
{
	if (NULL != a_widget) {
		a_widget->type->callback.push_rect(a_hwt, a_widget, a_rect);
	}
}

void
hwt_widget_pull_min(struct HWT *const a_hwt, struct HWTWidget *const a_widget,
    struct HWTSize *const a_min)
{
	if (NULL != a_widget) {
		a_widget->type->callback.pull_min(a_hwt, a_widget, a_min);
	}
}
