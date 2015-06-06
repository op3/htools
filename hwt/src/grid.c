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

#include <hwt/grid.h>
#include <hutils/memory.h>
#include <hwt/hwt.h>
#include <grid.h>
#include <holder.h>
#include <hwt.h>
#include <widget.h>

struct HWTGrid {
	struct	HWTWidget widget;
	int	row_num;
	int	col_num;
	struct	HWTHolder *holder_array;
};

static void	grid_destroy(struct HWT *, struct HWTWidget *);
static void	grid_draw(struct HWT *, struct HWTWidget *);
static void	grid_update(struct HWT *, struct HWTWidget *);

static struct HWTWidgetType const *g_type;

void
grid_destroy(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	struct HWTGrid *grid;
	int i;

	HWT_CAST(g_type, grid, a_widget);
	for (i = 0; grid->row_num * grid->col_num > i; ++i) {
		widget_free(a_hwt, &grid->holder_array[i].child);
	}
	FREE(grid->holder_array);
}

void
grid_draw(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	(void)a_hwt;
	(void)a_widget;
}

void
grid_update(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	struct HWTGrid *grid;
	struct HWTHolder *p;
	int row, col;

	HWT_CAST(g_type, grid, a_widget);
	p = grid->holder_array;
	for (row = 0; grid->row_num > row; ++row) {
		for (col = 0; grid->col_num > col; ++col) {
			widget_update(a_hwt, p->child);
			++p;
		}
	}
}

struct HWTWidget *
hwt_grid_create(int const a_row_num, int const a_col_num)
{
	struct HWTGrid *grid;

	if (0 >= a_row_num || 0 >= a_col_num) {
		fprintf(stderr, "Grid dimensions out of range.\n");
		abort();
	}
	CALLOC(grid, 1);
	widget_setup(&grid->widget, g_type);
	grid->row_num = a_row_num;
	grid->col_num = a_col_num;
	CALLOC(grid->holder_array, a_row_num * a_col_num);
	return &grid->widget;
}

void
hwt_grid_setup_(struct HWT *const a_hwt)
{
	struct HWTWidgetCallback callback;

	HWT_CALLBACK_SETUP(callback, grid);
	g_type = hwt_widget_register(a_hwt, "Grid", &callback);
}

struct HWTHolder *
hwt_grid_get_holder(struct HWTWidget *const a_parent, int const a_row, int
    const a_col)
{
	struct HWTGrid *grid;

	HWT_CAST(g_type, grid, a_parent);
	if (0 > a_row || grid->row_num <= a_row ||
	    0 > a_col || grid->col_num <= a_col) {
		fprintf(stderr, "Grid indices out of range.\n");
		abort();
	}
	return &grid->holder_array[grid->col_num * a_row + a_col];
}
