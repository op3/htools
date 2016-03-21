/*
 * Copyright (c) 2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hwt/button.h>
#include <hwt/hwt.h>
#include <hwt/widget.h>
#include <src/builtin.h>

struct Button {
	struct	HWTWidget *child;
	void	(*callback)(struct HWTWidget *);
};

static void			destroy(struct HWT *, struct HWTWidget *);
static void			draw(struct HWT *, struct HWTWidget *);
static void			pull_min(struct HWT *, struct HWTWidget *,
    struct HWTSize *);
static void			push_rect(struct HWT *, struct HWTWidget *);
static enum HWTEventFlow	respond(struct HWT *, struct HWTWidget *,
    struct HWTEvent const *);

static struct HWTWidgetType const *g_type;
HWT_CASTER(Button, g_type);

void
destroy(struct HWT *const a_hwt, struct HWTWidget *const a_button)
{
	struct Button *button;

	button = hwt_cast_Button(a_button);
	hwt_widget_free(a_hwt, &button->child);
}

void
draw(struct HWT *const a_hwt, struct HWTWidget *const a_button)
{
	(void)a_hwt;
	(void)a_button;
}

void
pull_min(struct HWT *const a_hwt, struct HWTWidget *const a_button, struct
    HWTSize *const a_min)
{
	struct Button *button;

	button = hwt_cast_Button(a_button);
	hwt_widget_pull_min(a_hwt, button->child, a_min);
}

void
push_rect(struct HWT *const a_hwt, struct HWTWidget *const a_button)
{
	struct Button *button;

	button = hwt_cast_Button(a_button);
	hwt_widget_push_rect(a_hwt, button->child,
	    hwt_widget_get_rect(a_button));
}

enum HWTEventFlow
respond(struct HWT *const a_hwt, struct HWTWidget *const a_button, struct
    HWTEvent const *const a_event)
{
	struct Button *button;
	struct HWTRect const *rect;
	float x1, y1, x2, y2;
	int do_callback;

	(void)a_hwt;
	button = hwt_cast_Button(a_button);
	HWT_EVENT_RESPOND(a_hwt, button->child, a_event);
	do_callback = 0;
	rect = hwt_widget_get_rect(a_button);
	x1 = rect->x;
	y1 = rect->y;
	x2 = rect->x + rect->width;
	y2 = rect->y + rect->height;
	if (HWT_EVENT_POINTER_UP == a_event->type &&
	    x1 <= a_event->data.pointer_up.x &&
	    x2 >  a_event->data.pointer_up.x &&
	    y1 <= a_event->data.pointer_up.y &&
	    y2 >  a_event->data.pointer_up.y &&
	    0 == a_event->data.pointer_up.code) {
		do_callback = 1;
	}
	if (do_callback && NULL != button->callback) {
		button->callback(a_button);
		return HWT_BREAK;
	}
	return HWT_CONTINUE;
}

struct HWTWidget *
hwt_button_create(struct HWT *const a_hwt)
{
	return hwt_widget_create(a_hwt, g_type);
}

void
hwt_button_set_callback(struct HWTWidget *const a_button, void
    (*a_callback)(struct HWTWidget *))
{
	struct Button *button;

	button = hwt_cast_Button(a_button);
	button->callback = a_callback;
}

void
hwt_button_set_child(struct HWTWidget *const a_button, struct HWTWidget
    **const a_child)
{
	hwt_widget_assign(&hwt_cast_Button(a_button)->child, a_child);
}

void
hwt_button_setup_(struct HWT *const a_hwt)
{
	HWT_WIDGET_REGISTER(a_hwt, Button, g_type);
}
