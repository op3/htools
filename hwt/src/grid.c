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

#include <hwt/grid.h>
#include <assert.h>
#include <hutils/fmtmod.h>
#include <hutils/memory.h>
#include <hwt/common.h>
#include <hwt/hwt.h>
#include <src/grid.h>
#include <src/holder.h>
#include <src/widget.h>

struct Grid {
	struct	HWTWidget widget;
	size_t	row_num;
	size_t	col_num;
	float	*row_min;
	float	*col_min;
	struct	HWTSize min;
	struct	HWTHolder *child;
};

static void	grid_destroy(struct HWT *, struct HWTWidget *);
static void	grid_draw(struct HWT *, struct HWTWidget *);
static void	grid_pull_min(struct HWT *, struct HWTWidget *, struct HWTSize
    *);
static void	grid_push_rect(struct HWT *, struct HWTWidget *, struct
    HWTRect const *);

static struct HWTWidgetType const *g_type;
HWT_CASTER(Grid, g_type);

void
grid_destroy(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	struct Grid *grid;
	size_t i;

	grid = hwt_cast_Grid(a_widget);
	for (i = 0; grid->row_num * grid->col_num > i; ++i) {
		hwt_widget_free(a_hwt, &grid->child[i].widget);
	}
	FREE(grid->row_min);
	FREE(grid->col_min);
	FREE(grid->child);
}

void
grid_draw(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	(void)a_hwt;
	(void)a_widget;
}

void
grid_pull_min(struct HWT *const a_hwt, struct HWTWidget *const a_widget,
    struct HWTSize *const a_min)
{
	struct Grid *grid;
	struct HWTHolder *p;
	size_t col, row;

	grid = hwt_cast_Grid(a_widget);
	memset(grid->row_min, 0, sizeof *grid->row_min * grid->row_num);
	memset(grid->col_min, 0, sizeof *grid->col_min * grid->col_num);
	p = grid->child;
	for (row = 0; grid->row_num > row; ++row) {
		for (col = 0; grid->col_num > col; ++col) {
			struct HWTSize min;

			if (NULL != p->widget) {
				hwt_widget_pull_min(a_hwt, p->widget, &min);
			}
			grid->row_min[row] = MAX(grid->row_min[row],
			    min.height);
			grid->col_min[col] = MAX(grid->col_min[col],
			    min.width);
			++p;
		}
	}
	grid->min.width = 0.0f;
	for (col = 0; grid->col_num > col; ++col) {
		grid->min.width += grid->col_min[col];
	}
	grid->min.height = 0.0f;
	for (row = 0; grid->row_num > row; ++row) {
		grid->min.height += grid->row_min[row];
	}
	COPY(*a_min, grid->min);
}

void
grid_push_rect(struct HWT *const a_hwt, struct HWTWidget *const a_widget,
    struct HWTRect const *const a_rect)
{
	struct HWTRect rect;
	struct HWTSize fill;
	struct Grid *grid;
	struct HWTHolder *p;
	size_t row;

	grid = hwt_cast_Grid(a_widget);
	fill.width = (a_rect->width - grid->min.width) / grid->col_num;
	fill.height = (a_rect->height - grid->min.height) / grid->row_num;
	p = grid->child;
	rect.y = a_rect->y;
	for (row = 0; grid->row_num > row; ++row) {
		size_t col;

		rect.height = grid->row_min[row] + fill.height;
		rect.x = a_rect->x;
		for (col = 0; grid->col_num > col; ++col) {
			rect.width = grid->col_min[col] + fill.width;
			hwt_widget_push_rect(a_hwt, p->widget, &rect);
			rect.x += rect.width;
			++p;
		}
		rect.y += rect.height;
	}
}

struct HWTWidget *
hwt_grid_create(struct HWT *const a_hwt, size_t const a_row_num, size_t const
    a_col_num)
{
	struct Grid *grid;

	if (0 == a_row_num || 1000 < a_row_num ||
	    0 == a_col_num || 1000 < a_col_num) {
		fprintf(stderr, "Grid dimensions (rows=%"PRIz",cols=%"PRIz") "
		    "out of range.\n", a_row_num, a_col_num);
		abort();
	}
	CALLOC(grid, 1);
	hwt_widget_init(a_hwt, &grid->widget, g_type);
	grid->row_num = a_row_num;
	grid->col_num = a_col_num;
	CALLOC(grid->row_min, a_row_num);
	CALLOC(grid->col_min, a_col_num);
	CALLOC(grid->child, a_row_num * a_col_num);
	return &grid->widget;
}

struct HWTHolder *
hwt_grid_get_child(struct HWTWidget *const a_grid, size_t const a_row, size_t
    const a_col)
{
	struct Grid *grid;

	grid = hwt_cast_Grid(a_grid);
	if (grid->row_num <= a_row || grid->col_num <= a_col) {
		fprintf(stderr, "Grid indices (row=%"PRIz",col=%"PRIz") out "
		    "of range (rows=%"PRIz",cols=%"PRIz").\n", a_row, a_col,
		    grid->row_num, grid->col_num);
		abort();
	}
	return &grid->child[grid->col_num * a_row + a_col];
}

void
hwt_grid_setup_(struct HWT *const a_hwt)
{
	HWT_WIDGET_REGISTER(g_type, a_hwt, grid);
}
