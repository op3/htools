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
#include <hutils/fmtmod.h>
#include <hwt/hwt.h>
#include <hwt/widget.h>
#include <src/builtin.h>

struct Grid {
	size_t	row_num;
	size_t	col_num;
	float	*row_min;
	float	*col_min;
	struct	HWTSize min;
	struct	HWTWidget **child;
};

static void			destroy(struct HWT *, struct HWTWidget *);
static void			draw(struct HWT *, struct HWTWidget *);
static void			pull_min(struct HWT *, struct HWTWidget *,
    struct HWTSize *);
static void			push_rect(struct HWT *, struct HWTWidget *);
static enum HWTEventFlow	respond(struct HWT *, struct HWTWidget *,
    struct HWTEvent const *);

static struct HWTWidgetType const *g_type;
HWT_CASTER(Grid, g_type);

void
destroy(struct HWT *a_hwt, struct HWTWidget *a_widget)
{
	struct Grid *grid;
	size_t i;

	grid = hwt_cast_Grid(a_widget);
	for (i = 0; grid->row_num * grid->col_num > i; ++i) {
		hwt_widget_free(a_hwt, &grid->child[i]);
	}
	FREE(grid->row_min);
	FREE(grid->col_min);
	FREE(grid->child);
}

void
draw(struct HWT *a_hwt, struct HWTWidget *a_widget)
{
	(void)a_hwt;
	(void)a_widget;
}

void
pull_min(struct HWT *a_hwt, struct HWTWidget *a_widget, struct HWTSize *a_min)
{
	struct Grid *grid;
	struct HWTWidget **p;
	size_t col, row;

	grid = hwt_cast_Grid(a_widget);
	memset(grid->row_min, 0, sizeof *grid->row_min * grid->row_num);
	memset(grid->col_min, 0, sizeof *grid->col_min * grid->col_num);
	p = grid->child;
	for (row = 0; grid->row_num > row; ++row) {
		for (col = 0; grid->col_num > col; ++col) {
			struct HWTSize min;

			if (NULL != *p) {
				hwt_widget_pull_min(a_hwt, *p, &min);
				grid->row_min[row] = MAX(grid->row_min[row],
				    min.height);
				grid->col_min[col] = MAX(grid->col_min[col],
				    min.width);
			}
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
push_rect(struct HWT *a_hwt, struct HWTWidget *a_widget)
{
	struct HWTRect r;
	struct HWTSize fill;
	struct Grid *grid;
	struct HWTRect const *rect;
	struct HWTWidget **p;
	size_t row;

	grid = hwt_cast_Grid(a_widget);
	rect = hwt_widget_get_rect(a_widget);
	fill.width = (rect->width - grid->min.width) / grid->col_num;
	fill.height = (rect->height - grid->min.height) /
	    grid->row_num;
	p = grid->child;
	r.y = rect->y;
	for (row = 0; grid->row_num > row; ++row) {
		size_t col;

		r.height = grid->row_min[row] + fill.height;
		r.x = rect->x;
		for (col = 0; grid->col_num > col; ++col) {
			r.width = grid->col_min[col] + fill.width;
			hwt_widget_push_rect(a_hwt, *p, &r);
			r.x += r.width;
			++p;
		}
		r.y += r.height;
	}
}

enum HWTEventFlow
respond(struct HWT *a_hwt, struct HWTWidget *a_widget, struct HWTEvent const
    *a_event)
{
	struct Grid *grid;
	size_t i;

	grid = hwt_cast_Grid(a_widget);
	for (i = 0; grid->row_num * grid->col_num > i; ++i) {
		HWT_EVENT_RESPOND(a_hwt, grid->child[i], a_event);
	}
	return HWT_CONTINUE;
}

struct HWTWidget *
hwt_grid_create(struct HWT *a_hwt, size_t a_row_num, size_t a_col_num)
{
	struct Grid *grid;
	struct HWTWidget *widget;

	if (0 == a_row_num || 1000 < a_row_num ||
	    0 == a_col_num || 1000 < a_col_num) {
		fprintf(stderr, "Grid dimensions (rows=%"PRIz",cols=%"PRIz") "
		    "out of range.\n", a_row_num, a_col_num);
		abort();
	}
	widget = hwt_widget_create(a_hwt, g_type);
	grid = hwt_cast_Grid(widget);
	grid->row_num = a_row_num;
	grid->col_num = a_col_num;
	CALLOC(grid->row_min, a_row_num);
	CALLOC(grid->col_min, a_col_num);
	CALLOC(grid->child, a_row_num * a_col_num);
	return widget;
}

void
hwt_grid_set_child(struct HWTWidget *a_grid, size_t a_row, size_t a_col,
    struct HWTWidget **a_child)
{
	struct Grid *grid;

	grid = hwt_cast_Grid(a_grid);
	if (grid->row_num <= a_row || grid->col_num <= a_col) {
		fprintf(stderr, "Grid indices (row=%"PRIz",col=%"PRIz") out "
		    "of range (rows=%"PRIz",cols=%"PRIz").\n", a_row, a_col,
		    grid->row_num, grid->col_num);
		abort();
	}
	hwt_widget_assign(&grid->child[grid->col_num * a_row + a_col],
	    a_child);
}

void
hwt_grid_setup_(struct HWT *a_hwt)
{
	HWT_WIDGET_REGISTER(a_hwt, Grid, g_type);
}
