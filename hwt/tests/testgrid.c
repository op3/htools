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
#include <hwt/grid.h>
#include <hwt/panel.h>
#include <htest/htest.h>

static void	dtor(void);

static struct HWT *g_hwt;
static struct HWTWidget *g_widget;

void
dtor()
{
	hwt_widget_free(g_hwt, &g_widget);
	hwt_free(&g_hwt);
}

HTEST(NotZero)
{
	struct HWTWidget *grid;

	(void)grid;
	HTRY_SIGNAL_DTOR(grid = hwt_grid_create(0, 0), dtor);
	HTRY_SIGNAL_DTOR(grid = hwt_grid_create(0, 1), dtor);
	HTRY_SIGNAL_DTOR(grid = hwt_grid_create(1, 0), dtor);
	grid = hwt_grid_create(1, 1);
	hwt_set_root(g_hwt, grid);
}

HTEST(Children)
{
	struct HWTWidget *grid;
	int row, col;

	grid = hwt_grid_create(2, 2);
	hwt_set_root(g_hwt, grid);
	for (row = 0; 2 > row; ++row) {
		for (col = 0; 2 > col; ++col) {
			struct HWTHolder *holder;
			struct HWTWidget *child;

			holder = hwt_grid_get_holder(grid, row, col);
			child = hwt_panel_create();
			HTRY_VOID(hwt_holder_set_child(holder, child));
		}
	}
}

HTEST_SUITE(Grid)
{
	g_hwt = hwt_create(NULL);

	HTEST_ADD(NotZero);
	HTEST_ADD(Children);

	hwt_free(&g_hwt);
}
