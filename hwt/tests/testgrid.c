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
#include <hwt/grid.h>
#include <hwt/hwt.h>
#include <tests/mockwidget.h>

struct Child {
	struct	HWTSize min;
	struct	HWTRect rect;
};

static void	pull_min(struct HWTSize *, void *);
static void	push_rect(struct HWTRect const *, void *);
static void	dtor(void);

static struct HWT *g_hwt;
static struct HWTWidget *g_widget;

void
dtor()
{
	hwt_widget_free(g_hwt, &g_widget);
	hwt_free(&g_hwt);
}

void
pull_min(struct HWTSize *const a_min, void *const a_data)
{
	struct Child *child;

	child = a_data;
	COPY(*a_min, child->min);
}

void
push_rect(struct HWTRect const *const a_rect, void *const a_data)
{
	struct Child *child;

	child = a_data;
	COPY(child->rect, *a_rect);
}

HTEST(PositiveDimensions)
{
	struct HWTWidget *grid;
	int i;

	for (i = -1; 1 >= i; ++i) {
		int j;

		for (j = -1; 1 >= j; ++j) {
			if (1 == i && 1 == j) {
				grid = hwt_grid_create(g_hwt, i, j);
				hwt_set_root(g_hwt, grid);
			} else {
				HTRY_SIGNAL_DTOR(grid = hwt_grid_create(g_hwt,
				    i, j),
				    dtor);
			}
		}
	}
}

HTEST(Children)
{
	struct HWTWidget *grid;
	int row;

	grid = hwt_grid_create(g_hwt, 2, 2);
	hwt_set_root(g_hwt, grid);
	for (row = -1; 2 >= row; ++row) {
		int col;

		for (col = -1; 2 >= col; ++col) {
			g_widget = mockwidget_create(g_hwt, NULL);
			if (0 <= row && 2 > row &&
			    0 <= col && 2 > col) {
				HTRY_VOID(hwt_grid_set_child(grid, row, col,
				    &g_widget));
			} else {
				HTRY_SIGNAL_DTOR(hwt_grid_set_child(grid, row,
				    col, &g_widget), dtor);
			}
		}
	}
}

HTEST(Update)
{
	struct Child child0 = {{ 0.0f, 10.0f}, {0.0f, 0.0f, 0.0f, 0.0f}};
	struct Child child1 = {{20.0f, 30.0f}, {0.0f, 0.0f, 0.0f, 0.0f}};
	struct Child child2 = {{40.0f, 50.0f}, {0.0f, 0.0f, 0.0f, 0.0f}};
	struct Child child3 = {{60.0f, 70.0f}, {0.0f, 0.0f, 0.0f, 0.0f}};
	struct MockWidgetCallback cb;
	struct HWTEvent event_resize;
	struct HWTWidget *grid;
	struct HWTWidget *mock;

	grid = hwt_grid_create(g_hwt, 2, 2);
	hwt_set_root(g_hwt, grid);

	cb.destroy = NULL;
	cb.pull_min = pull_min;
	cb.push_rect = push_rect;

	cb.data = &child0;
	mock = mockwidget_create(g_hwt, &cb);
	hwt_grid_set_child(grid, 0, 0, &mock);

	cb.data = &child1;
	mock = mockwidget_create(g_hwt, &cb);
	hwt_grid_set_child(grid, 0, 1, &mock);

	cb.data = &child2;
	mock = mockwidget_create(g_hwt, &cb);
	hwt_grid_set_child(grid, 1, 0, &mock);

	cb.data = &child3;
	mock = mockwidget_create(g_hwt, &cb);
	hwt_grid_set_child(grid, 1, 1, &mock);

	event_resize.type = HWT_EVENT_RESIZE;
	event_resize.data.resize.size.width = 120.0f;
	event_resize.data.resize.size.height = 200.0f;

	hwt_send_event(g_hwt, &event_resize);

	HTRY_FLT(  0.0f, ==, child0.rect.x);
	HTRY_FLT(  0.0f, ==, child0.rect.y);
	HTRY_FLT( 50.0f, ==, child0.rect.width);
	HTRY_FLT( 80.0f, ==, child0.rect.height);

	HTRY_FLT( 50.0f, ==, child1.rect.x);
	HTRY_FLT(  0.0f, ==, child1.rect.y);
	HTRY_FLT( 70.0f, ==, child1.rect.width);
	HTRY_FLT( 80.0f, ==, child1.rect.height);

	HTRY_FLT(  0.0f, ==, child2.rect.x);
	HTRY_FLT( 80.0f, ==, child2.rect.y);
	HTRY_FLT( 50.0f, ==, child2.rect.width);
	HTRY_FLT(120.0f, ==, child2.rect.height);

	HTRY_FLT( 50.0f, ==, child3.rect.x);
	HTRY_FLT( 80.0f, ==, child3.rect.y);
	HTRY_FLT( 70.0f, ==, child3.rect.width);
	HTRY_FLT(120.0f, ==, child3.rect.height);
}

HTEST_SUITE(Grid)
{
	g_hwt = hwt_create(NULL);
	mockwidget_setup(g_hwt);

	HTEST_ADD(PositiveDimensions);
	HTEST_ADD(Children);
	HTEST_ADD(Update);

	hwt_free(&g_hwt);
}
