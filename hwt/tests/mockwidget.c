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

#include <mockwidget.h>
#include <hutils/memory.h>
#include <widget.h>

struct MockWidget {
	struct	HWTWidget widget;
	struct	MockWidgetCallback user_callback;
};

static void	mock_destroy(struct HWT *, struct HWTWidget *);
static void	mock_draw(struct HWT *, struct HWTWidget *);
static void	mock_propagate_min(struct HWT *, struct HWTWidget *, struct
    HWTRect *);
static void	mock_propagate_size(struct HWT *, struct HWTWidget *, struct
    HWTRect const *);

static struct HWTWidgetType const *g_type;

void
mock_destroy(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	(void)a_hwt;
	(void)a_widget;
}

void
mock_draw(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	(void)a_hwt;
	(void)a_widget;
}

void
mock_propagate_min(struct HWT *const a_hwt, struct HWTWidget *const a_widget,
    struct HWTRect *const a_min)
{
	(void)a_hwt;
	(void)a_widget;
	a_min->width = 0.0f;
	a_min->height = 0.0f;
}

void
mock_propagate_size(struct HWT *const a_hwt, struct HWTWidget *const a_widget,
    struct HWTRect const *const a_size)
{
	struct MockWidget *mock;

	(void)a_hwt;
	HWT_CAST(g_type, mock, a_widget);
	mock->user_callback.func(a_size, mock->user_callback.data);
}

struct HWTWidget *
mockwidget_create(struct MockWidgetCallback *const a_user_callback)
{
	struct MockWidget *mock;

	CALLOC(mock, 1);
	widget_setup(&mock->widget, g_type);
	memmove(&mock->user_callback, a_user_callback, sizeof
	    mock->user_callback);
	return &mock->widget;
}

void
mockwidget_setup(struct HWT *const a_hwt)
{
	struct HWTWidgetCallback callback;

	HWT_CALLBACK_SETUP(callback, mock);
	g_type = hwt_widget_register(a_hwt, "Mock", &callback);
}
