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
#include <hutils/memory.h>
#include <grid.h>
#include <holder.h>
#include <hwt.h>
#include <panel.h>
#include <widget.h>

TAILQ_HEAD(TypeList, HWTWidgetType);
struct HWT {
	struct	HWTRenderer *renderer;
	struct	HWTWidget *root;
	struct	TypeList type_list;
};

struct HWT *
hwt_create(struct HWTRenderer *const a_renderer)
{
	struct HWT *hwt;

	CALLOC(hwt, 1);
	hwt->renderer = a_renderer;
	hwt->root = NULL;
	TAILQ_INIT(&hwt->type_list);
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
	widget_free(hwt, &hwt->root);

	while (!TAILQ_EMPTY(&hwt->type_list)) {
		struct HWTWidgetType *type;

		type = TAILQ_FIRST(&hwt->type_list);
		TAILQ_REMOVE(&hwt->type_list, type, next);
		FREE(type);
	}

	FREE(*a_hwt);
}

struct HWTWidgetCallback const *
hwt_get_callback(struct HWTWidget const *const a_widget)
{
	return &a_widget->type->callback;
}

void
hwt_holder_set_child(struct HWTHolder *const a_holder, struct HWTWidget *const
    a_child)
{
	if (NULL == a_child) {
		a_holder->child = NULL;
		return;
	}

	if (NULL != a_child->holder) {
		fprintf(stderr, "Assigning an owned child.\n");
		abort();
	}
	if (NULL != a_holder->child) {
		a_holder->child->holder = NULL;
	}
	a_holder->child = a_child;
	a_child->holder = a_holder;
}

void
hwt_set_root(struct HWT *const a_hwt, struct HWTWidget *const a_root)
{
	a_hwt->root = a_root;
}

void
hwt_update(struct HWT *const a_hwt)
{
	widget_update(a_hwt, a_hwt->root);
}

struct HWTWidgetType const *
hwt_widget_register(struct HWT *const a_hwt, char const *const a_name, struct
    HWTWidgetCallback const *const a_callback)
{
	struct HWTWidgetType *type;

	CALLOC(type, 1);
	type->name = a_name;
	memmove(&type->callback, a_callback, sizeof type->callback);
	TAILQ_INSERT_TAIL(&a_hwt->type_list, type, next);
	return type;
}

void
hwt_widget_free(struct HWT *const a_hwt, struct HWTWidget **const a_widget)
{
	widget_free(a_hwt, a_widget);
}
