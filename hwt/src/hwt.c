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

#include <hwt/hwt.h>
#include <assert.h>
#include <stdio.h>
#include <hutils/memory.h>
#include <hwt/common.h>
#include <src/grid.h>
#include <src/holder.h>
#include <src/panel.h>
#include <src/widget.h>

TAILQ_HEAD(TypeList, HWTWidgetType);
struct HWT {
	struct	HWTRenderer const *renderer;
	struct	TypeList type_list;
	struct	HWTWidget *root;
};

struct HWT *
hwt_create(struct HWTRenderer const *const a_renderer)
{
	struct HWT *hwt;

	CALLOC(hwt, 1);
	hwt->renderer = a_renderer;
	TAILQ_INIT(&hwt->type_list);
	hwt->root = NULL;
	hwt_grid_setup_(hwt);
	hwt_panel_setup_(hwt);
	return hwt;
}

void
hwt_free(struct HWT **const a_hwt)
{
	struct HWT *hwt;

	hwt = *a_hwt;
	if (NULL == hwt) {
		return;
	}
	hwt_widget_free(hwt, &hwt->root);

	while (!TAILQ_EMPTY(&hwt->type_list)) {
		struct HWTWidgetType *type;

		type = TAILQ_FIRST(&hwt->type_list);
		TAILQ_REMOVE(&hwt->type_list, type, next);
		FREE(type);
	}

	FREE(*a_hwt);
}

void
hwt_holder_set_widget(struct HWTHolder *const a_holder, struct HWTWidget
    *const a_widget)
{
	if (NULL != a_holder->widget) {
		a_holder->widget->owner = NULL;
	}
	if (NULL == a_widget) {
		a_holder->widget = NULL;
		return;
	}
	if (NULL != a_widget->owner) {
		fprintf(stderr, "Assigning an already owned child.\n");
		abort();
	}
	a_holder->widget = a_widget;
	a_widget->owner = a_holder;
}

void
hwt_set_root(struct HWT *const a_hwt, struct HWTWidget *const a_root)
{
	a_hwt->root = a_root;
}

void
hwt_update(struct HWT *const a_hwt, struct HWTRect const *const a_rect)
{
	struct HWTSize min;

	hwt_widget_pull_min(a_hwt, a_hwt->root, &min);
	hwt_widget_push_rect(a_hwt, a_hwt->root, a_rect);
}

void
hwt_widget_free(struct HWT *const a_hwt, struct HWTWidget **const a_widget)
{
	struct HWTWidget *widget;

	widget = *a_widget;
	if (NULL == widget) {
		return;
	}
	widget->type->callback.destroy(a_hwt, widget);
	FREE(*a_widget);
}

void
hwt_widget_init(struct HWT *const a_hwt, struct HWTWidget *const a_widget,
    struct HWTWidgetType const *const a_type)
{
	struct HWTWidgetType *type;

	TAILQ_FOREACH(type, &a_hwt->type_list, next) {
		if (a_type == type) {
			a_widget->type = a_type;
			a_widget->owner = NULL;
			return;
		}
	}
	fprintf(stderr, "Widget type not registered.\n");
	abort();
}

struct HWTWidgetType const *
hwt_widget_register_(struct HWT *const a_hwt, struct HWTWidgetCallback const
    *const a_callback)
{
	struct HWTWidgetType *type;

	CALLOC(type, 1);
	COPY(type->callback, *a_callback);
	TAILQ_INSERT_TAIL(&a_hwt->type_list, type, next);
	return type;
}
