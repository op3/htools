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

#include <tests/mockwidget.h>
#include <hwt/widget.h>

struct MockWidget {
	struct	MockWidgetCallback user_callback;
};

static void			destroy(struct HWT *, struct HWTWidget *);
static void			draw(struct HWT *, struct HWTWidget *);
static void			pull_min(struct HWT *, struct HWTWidget *,
    struct HWTSize *);
static void			push_rect(struct HWT *, struct HWTWidget *);
static enum HWTEventFlow	respond(struct HWT *, struct HWTWidget *,
    struct HWTEvent const *);

static struct HWTWidgetType const *g_type;
HWT_CASTER(MockWidget, g_type);

void
destroy(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	struct MockWidget *mock;

	(void)a_hwt;
	mock = hwt_cast_MockWidget(a_widget);
	if (NULL != mock->user_callback.destroy) {
		mock->user_callback.destroy(mock->user_callback.data);
	}
}

void
draw(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	(void)a_hwt;
	(void)a_widget;
}

struct HWTWidget *
mockwidget_create(struct HWT *const a_hwt, struct MockWidgetCallback const
    *const a_user_callback)
{
	struct MockWidget *mock;
	struct HWTWidget *widget;

	widget = hwt_widget_create(a_hwt, g_type);
	mock = hwt_cast_MockWidget(widget);
	if (NULL != a_user_callback) {
		COPY(mock->user_callback, *a_user_callback);
	}
	return widget;
}

void
mockwidget_setup(struct HWT *const a_hwt)
{
	HWT_WIDGET_REGISTER(a_hwt, MockWidget, g_type);
}

void
pull_min(struct HWT *const a_hwt, struct HWTWidget *const a_widget, struct
    HWTSize *const a_min)
{
	struct MockWidget *mock;

	(void)a_hwt;
	mock = hwt_cast_MockWidget(a_widget);
	if (NULL == mock->user_callback.pull_min) {
		a_min->width = 0.0f;
		a_min->height = 0.0f;
	} else {
		mock->user_callback.pull_min(a_min, mock->user_callback.data);
	}
}

void
push_rect(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	struct MockWidget *mock;

	(void)a_hwt;
	mock = hwt_cast_MockWidget(a_widget);
	if (NULL != mock->user_callback.push_rect) {
		mock->user_callback.push_rect(hwt_widget_get_rect(a_widget),
		    mock->user_callback.data);
	}
}

enum HWTEventFlow
respond(struct HWT *const a_hwt, struct HWTWidget *const a_widget, struct
    HWTEvent const *const a_event)
{
	struct MockWidget *mock;

	(void)a_hwt;
	mock = hwt_cast_MockWidget(a_widget);
	if (NULL != mock->user_callback.pull_min) {
		mock->user_callback.respond(a_event,
		    mock->user_callback.data);
	}
	return HWT_CONTINUE;
}
