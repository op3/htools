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
#include <sys/queue.h>
#include <stdio.h>
#include <hwt/widget.h>
#include <src/builtin.h>

struct HWTWidgetType {
	size_t	size;
	struct	HWTWidgetCallback callback;
	TAILQ_ENTRY(HWTWidgetType)	next;
};
TAILQ_HEAD(TypeList, HWTWidgetType);
struct HWTWidget {
	struct	HWTWidgetType const *type;
	struct	HWTWidget **parent;
	struct	HWTRect rect;
};
struct HWT {
	struct	HWTRenderer const *renderer;
	struct	TypeList type_list;
	struct	HWTWidget *root;
};

struct HWT *
hwt_create(struct HWTRenderer const *a_renderer)
{
	struct HWT *hwt;

	CALLOC(hwt, 1);
	hwt->renderer = a_renderer;
	TAILQ_INIT(&hwt->type_list);
	hwt->root = NULL;
	hwt_button_setup_(hwt);
	hwt_grid_setup_(hwt);
	hwt_label_setup_(hwt);
	hwt_panel_setup_(hwt);
	return hwt;
}

void
hwt_free(struct HWT **a_hwt)
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
hwt_send_event(struct HWT *a_hwt, struct HWTEvent const *a_event)
{
	if (HWT_EVENT_RESIZE == a_event->type) {
		struct HWTRect rect;
		struct HWTSize min;

		hwt_widget_pull_min(a_hwt, a_hwt->root, &min);
		rect.x = 0.0f;
		rect.y = 0.0f;
		rect.width = a_event->data.resize.size.width;
		rect.height = a_event->data.resize.size.height;
		hwt_widget_push_rect(a_hwt, a_hwt->root, &rect);
	} else {
		enum HWTEventFlow flow;

		(void)flow;
		flow = hwt_widget_respond(a_hwt, a_hwt->root, a_event);
	}
}

void
hwt_set_root(struct HWT *a_hwt, struct HWTWidget *a_root)
{
	hwt_widget_assign(&a_hwt->root, &a_root);
}

void
hwt_widget_assign(struct HWTWidget **a_parent, struct HWTWidget **a_child)
{
	if (NULL != *a_parent) {
		fprintf(stderr, "Parent already has a widget.\n");
		abort();
	}
	if (NULL == a_child || NULL == *a_child) {
		return;
	}
	if (NULL != (*a_child)->parent) {
		fprintf(stderr, "Child already assigned elsewhere.\n");
		abort();
	}
	*a_parent = *a_child;
	(*a_child)->parent = a_parent;
	*a_child = NULL;
}

void *
hwt_widget_cast_(struct HWTWidget *a_widget)
{
	return a_widget + 1;
}

struct HWTWidget *
hwt_widget_create(struct HWT *a_hwt, struct HWTWidgetType const *a_type)
{
	struct HWTWidgetType *type;

	TAILQ_FOREACH(type, &a_hwt->type_list, next) {
		if (a_type == type) {
			struct HWTWidget *widget;
			size_t total;

			total = sizeof *widget + type->size;
			MALLOC(widget, total);
			memset(widget, 0, total);
			widget->type = type;
			return widget;
		}
	}
	fprintf(stderr, "Widget type not registered.\n");
	abort();
}

void
hwt_widget_free(struct HWT *a_hwt, struct HWTWidget **a_widget)
{
	struct HWTWidget *widget;

	widget = *a_widget;
	if (NULL == widget) {
		return;
	}
	if (NULL != widget->parent) {
		*widget->parent = NULL;
		widget->parent = NULL;
	}
	widget->type->callback.destroy(a_hwt, widget);
	FREE(*a_widget);
}

struct HWTRect const *
hwt_widget_get_rect(struct HWTWidget const *a_widget)
{
	return &a_widget->rect;
}

struct HWTWidgetType const *
hwt_widget_get_type(struct HWTWidget const *a_widget)
{
	return a_widget->type;
}

void
hwt_widget_pull_min(struct HWT *a_hwt, struct HWTWidget *a_widget, struct
    HWTSize *a_min)
{
	if (NULL == a_widget) {
		ZERO(*a_min);
	} else {
		a_widget->type->callback.pull_min(a_hwt, a_widget, a_min);
	}
}

void
hwt_widget_push_rect(struct HWT *a_hwt, struct HWTWidget *a_widget, struct
    HWTRect const *a_rect)
{
	if (NULL != a_widget) {
		COPY(a_widget->rect, *a_rect);
		a_widget->type->callback.push_rect(a_hwt, a_widget);
	}
}

struct HWTWidgetType const *
hwt_widget_register_(struct HWT *a_hwt, size_t a_size, struct
    HWTWidgetCallback const *a_callback)
{
	struct HWTWidgetType *type;

	CALLOC(type, 1);
	type->size = a_size;
	COPY(type->callback, *a_callback);
	TAILQ_INSERT_TAIL(&a_hwt->type_list, type, next);
	return type;
}

enum HWTEventFlow
hwt_widget_respond(struct HWT *a_hwt, struct HWTWidget *a_widget, struct
    HWTEvent const *a_event)
{
	if (NULL == a_widget) {
		return HWT_CONTINUE;
	}
	return a_widget->type->callback.respond(a_hwt, a_widget, a_event);
}
