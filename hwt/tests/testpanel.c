/*
 * Copyright (c) 2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
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

#include <hwt/hwt.h>
#include <hwt/panel.h>
#include <htest/htest.h>
#include <mockwidget.h>
#include <widget.h>

static void	touch_child(void *);

static struct HWT *g_hwt;

void
touch_child(void *const a_data)
{
	*((int *)a_data) = 1;
}

HTEST(CreateAndFree)
{
	struct HWTWidget *panel;

	panel = hwt_panel_create();
	HTRY_VOID(hwt_set_root(g_hwt, panel));
}

HTEST(Child)
{
	struct MockWidgetCallback cb;
	struct HWTWidget *panel, *mock;
	struct HWTHolder *holder;
	int touched;

	panel = hwt_panel_create();
	hwt_set_root(g_hwt, panel);
	holder = hwt_panel_get_holder(panel);

	cb.func = touch_child;
	cb.data = &touched;
	mock = mockwidget_create(&cb);

	touched = 0;
	hwt_holder_set_child(holder, mock);
	hwt_update(g_hwt);
	HTRY_BOOL(touched);

	touched = 0;
	hwt_holder_set_child(holder, NULL);
	hwt_update(g_hwt);
	HTRY_BOOL(!touched);

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
