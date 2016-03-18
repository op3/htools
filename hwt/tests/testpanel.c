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

#include <htest/htest.h>
#include <hwt/common.h>
#include <hwt/hwt.h>
#include <hwt/panel.h>
#include <src/widget.h>
#include <tests/mockwidget.h>

static void	push_rect(struct HWTRect const *, void *);

static struct HWT *g_hwt;

void
push_rect(struct HWTRect const *const a_rect, void *const a_data)
{
	struct HWTRect *rect;

	rect = a_data;
	COPY(*rect, *a_rect);
}

HTEST(CreateAndFree)
{
	struct HWTWidget *panel;

	panel = hwt_panel_create();
	HTRY_PTR(NULL, !=, panel);

	hwt_widget_free(g_hwt, &panel);
	HTRY_PTR(NULL, ==, panel);
}

HTEST(Child)
{
	struct HWTRect const c_root_rect = {10, 20, 30, 40};
	struct HWTRect rect;
	struct MockWidgetCallback cb;
	struct HWTWidget *panel, *mock;
	struct HWTHolder *holder;

	panel = hwt_panel_create();
	hwt_set_root(g_hwt, panel);
	holder = hwt_panel_get_child(panel);

	cb.destroy = NULL;
	cb.pull_min = NULL;
	cb.push_rect = push_rect;
	cb.data = &rect;
	mock = mockwidget_create(&cb);

	ZERO(rect);
	hwt_holder_set_widget(holder, mock);
	hwt_update(g_hwt, &c_root_rect);
	HTRY_I(10, ==, rect.x);
	HTRY_I(20, ==, rect.y);
	HTRY_I(30, ==, rect.width);
	HTRY_I(40, ==, rect.height);

	ZERO(rect);
	hwt_holder_set_widget(holder, NULL);
	hwt_update(g_hwt, &c_root_rect);
	HTRY_I(0, ==, rect.x);
	HTRY_I(0, ==, rect.y);
	HTRY_I(0, ==, rect.width);
	HTRY_I(0, ==, rect.height);

	hwt_widget_free(g_hwt, &mock);
}

HTEST_SUITE(Panel)
{
	g_hwt = hwt_create(NULL);
	mockwidget_setup(g_hwt);

	HTEST_ADD(CreateAndFree);
	HTEST_ADD(Child);

	hwt_free(&g_hwt);
}
