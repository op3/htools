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

#include <htest/htest.h>
#include <hwt/button.h>
#include <hwt/hwt.h>
#include <tests/mockwidget.h>

static void	destroy(void *);
static void	push_rect(struct HWTRect const *, void *);

static struct HWT *g_hwt;

void
destroy(void *const a_data)
{
	int *was_destroyed;

	was_destroyed = a_data;
	*was_destroyed = 1;
}

void
push_rect(struct HWTRect const *const a_rect, void *const a_data)
{
	struct HWTRect *rect;

	rect = a_data;
	COPY(*rect, *a_rect);
}

HTEST(ChildFreed)
{
	struct MockWidgetCallback cb;
	struct HWTWidget *button, *mock;
	int was_destroyed;

	button = hwt_button_create(g_hwt);

	cb.destroy = destroy;
	cb.pull_min = NULL;
	cb.push_rect = NULL;
	cb.data = &was_destroyed;
	mock = mockwidget_create(g_hwt, &cb);

	was_destroyed = 0;
	hwt_button_set_child(button, &mock);
	HTRY_BOOL(!was_destroyed);
	hwt_widget_free(g_hwt, &button);
	HTRY_BOOL(was_destroyed);
}

HTEST(ChildRect)
{
	struct MockWidgetCallback cb;
	struct HWTEvent event_resize;
	struct HWTRect rect;
	struct HWTWidget *button, *mock;

	button = hwt_button_create(g_hwt);
	hwt_set_root(g_hwt, button);

	cb.destroy = NULL;
	cb.pull_min = NULL;
	cb.push_rect = push_rect;
	cb.data = &rect;
	mock = mockwidget_create(g_hwt, &cb);

	event_resize.type = HWT_EVENT_RESIZE;
	event_resize.data.resize.size.width = 10.0f;
	event_resize.data.resize.size.height = 20.0f;

	ZERO(rect);
	hwt_button_set_child(button, &mock);
	hwt_send_event(g_hwt, &event_resize);
	HTRY_FLT( 0.0f, ==, rect.x);
	HTRY_FLT( 0.0f, ==, rect.y);
	HTRY_FLT(10.0f, ==, rect.width);
	HTRY_FLT(20.0f, ==, rect.height);

	/* TODO Test replacing child? */
}

HTEST_SUITE(Button)
{
	g_hwt = hwt_create(NULL);
	mockwidget_setup(g_hwt);

	HTEST_ADD(ChildFreed);
	HTEST_ADD(ChildRect);

	hwt_free(&g_hwt);
}
