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

HTEST(PositiveDimensions)
{
	struct HWTWidget *grid;
	int i;

	for (i = -1; 1 > i; ++i) {
		int j;

		for (j = -1; 1 > j; ++j) {
			if (1 == i && 1 == j) {
				grid = hwt_grid_create(i, j);
			} else {
				HTRY_SIGNAL(grid = hwt_grid_create(i, j));
			}
		}
	}
	hwt_set_root(g_hwt, grid);
}

HTEST(Children)
{
	struct HWTWidget *grid;
	int row;

	grid = hwt_grid_create(2, 2);
	hwt_set_root(g_hwt, grid);
	for (row = -1; 3 > row; ++row) {
		int col;

		for (col = -1; 3 > col; ++col) {
			struct HWTHolder *holder;

			g_widget = hwt_panel_create();
			if (0 <= row && 2 > row &&
			    0 <= col && 2 > col) {
				HTRY_VOID(hwt_grid_set_child(grid, row, col,
				    g_widget));
			} else {
				HTRY_SIGNAL_DTOR(hwt_grid_set_child(grid, row,
				    col, g_widget), dtor);
			}
		}
	}
}

HTEST(MinSize)
{
	struct HWTWidget *grid;
	int row;

	grid = hwt_grid_create(2, 2);
	hwt_set_root(g_hwt, grid);
	for (row = -1; 3 > row; ++row) {
		int col;

		for (col = -1; 3 > col; ++col) {
			struct HWTHolder *holder;

			g_widget = hwt_panel_create();
			if (0 <= row && 2 > row &&
			    0 <= col && 2 > col) {
				HTRY_VOID(hwt_grid_set_child(grid, row, col,
				    g_widget));
			} else {
				HTRY_SIGNAL_DTOR(hwt_grid_set_child(grid, row,
				    col, g_widget), dtor);
			}
		}
	}
}

HTEST_SUITE(Grid)
{
	g_hwt = hwt_create(NULL);

	HTEST_ADD(PositiveDimensions);
	HTEST_ADD(Children);
	HTEST_ADD(MinSize);

	hwt_free(&g_hwt);
}
