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

#include <hwt/hwt.h>
#include <htest/htest.h>
#include <tests/mockwidget.h>

static void	destroy(void *);

static int g_was_destroyed;

void
destroy(void *const a_data)
{
	(void)a_data;
	g_was_destroyed = 1;
}

HTEST(CreateAndFree)
{
	struct HWT *hwt;

	hwt = hwt_create(NULL);
	HTRY_PTR(NULL, !=, hwt);

	hwt_free(&hwt);
	HTRY_PTR(NULL, ==, hwt);
}

HTEST(RootLinking)
{
	struct MockWidgetCallback cb;
	struct HWT *hwt;
	struct HWTWidget *mock;

	ZERO(cb);
	cb.destroy = destroy;

	hwt = hwt_create(NULL);
	mockwidget_setup(hwt);
	mock = mockwidget_create(hwt, &cb);
	g_was_destroyed = 0;
	hwt_set_root(hwt, mock);
	HTRY_I(0, ==, g_was_destroyed);
	HTRY_VOID(hwt_free(&hwt));
	HTRY_I(1, ==, g_was_destroyed);

	hwt = hwt_create(NULL);
	mockwidget_setup(hwt);
	mock = mockwidget_create(hwt, &cb);
	g_was_destroyed = 0;
	hwt_set_root(hwt, mock);
	HTRY_I(0, ==, g_was_destroyed);
	hwt_widget_free(hwt, &mock);
	HTRY_I(1, ==, g_was_destroyed);
	HTRY_VOID(hwt_free(&hwt));
}

HTEST_SUITE(HWT)
{
	HTEST_ADD(CreateAndFree);
	HTEST_ADD(RootLinking);
}
