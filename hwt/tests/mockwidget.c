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
#include <assert.h>
#include <hutils/memory.h>
#include <hwt/common.h>
#include <src/widget.h>

struct MockWidget {
	struct	HWTWidget widget;
	struct	MockWidgetCallback user_callback;
};

static void	mock_destroy(struct HWT *, struct HWTWidget *);
static void	mock_draw(struct HWT *, struct HWTWidget *);
static void	mock_pull_min(struct HWT *, struct HWTWidget *, struct HWTSize
    *);
static void	mock_push_rect(struct HWT *, struct HWTWidget *, struct
    HWTRect const *);

static struct HWTWidgetType const *g_type;
HWT_CASTER(MockWidget, g_type);

void
mock_destroy(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	struct MockWidget *mock;

	(void)a_hwt;
	mock = hwt_cast_MockWidget(a_widget);
	if (NULL != mock->user_callback.destroy) {
		mock->user_callback.destroy(mock->user_callback.data);
	}
}

void
mock_draw(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	(void)a_hwt;
	(void)a_widget;
}

void
mock_pull_min(struct HWT *const a_hwt, struct HWTWidget *const a_widget,
    struct HWTSize *const a_min)
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
mock_push_rect(struct HWT *const a_hwt, struct HWTWidget *const a_widget,
    struct HWTRect const *const a_rect)
{
	struct MockWidget *mock;

	(void)a_hwt;
	mock = hwt_cast_MockWidget(a_widget);
	if (NULL != mock->user_callback.push_rect) {
		mock->user_callback.push_rect(a_rect,
		    mock->user_callback.data);
	}
}

struct HWTWidget *
mockwidget_create(struct HWT *const a_hwt, struct MockWidgetCallback const
    *const a_user_callback)
{
	struct MockWidget *mock;

	CALLOC(mock, 1);
	hwt_widget_init(a_hwt, &mock->widget, g_type);
	COPY(mock->user_callback, *a_user_callback);
	return &mock->widget;
}

void
mockwidget_setup(struct HWT *const a_hwt)
{
	HWT_WIDGET_REGISTER(g_type, a_hwt, mock);
}
