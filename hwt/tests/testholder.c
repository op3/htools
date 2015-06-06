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

static struct HWT *g_hwt;

HTEST(CanOverwrite)
{
	struct HWTWidget *panel[3];
	struct HWTHolder *holder;

	panel[0] = hwt_panel_create();
	panel[1] = hwt_panel_create();
	panel[2] = hwt_panel_create();

	holder = hwt_panel_get_holder(panel[0]);
	hwt_holder_set_child(holder, panel[1]);
	hwt_holder_set_child(holder, panel[2]);
	hwt_set_root(g_hwt, panel[0]);

	hwt_widget_free(g_hwt, &panel[1]);

	HTRY_VOID((void)0);
}

HTEST_SUITE(Holder)
{
	g_hwt = hwt_create(NULL);

	HTEST_ADD(CanOverwrite);

	hwt_free(&g_hwt);
}
