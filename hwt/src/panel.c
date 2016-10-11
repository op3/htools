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

#include <hwt/panel.h>
#include <hwt/hwt.h>
#include <hwt/widget.h>
#include <src/builtin.h>

struct Panel {
	struct	HWTWidget *child;
};

static void			destroy(struct HWT *, struct HWTWidget *);
static void			draw(struct HWT *, struct HWTWidget *);
static void			pull_min(struct HWT *, struct HWTWidget *,
    struct HWTSize *);
static void			push_rect(struct HWT *, struct HWTWidget *);
static enum HWTEventFlow	respond(struct HWT *, struct HWTWidget *,
    struct HWTEvent const *);

static struct HWTWidgetType const *g_type;
HWT_CASTER(Panel, g_type);

struct HWTWidget *
hwt_panel_create(struct HWT *a_hwt)
{
	return hwt_widget_create(a_hwt, g_type);
}

void
hwt_panel_set_child(struct HWTWidget *a_panel, struct HWTWidget **a_child)
{
	hwt_widget_assign(&hwt_cast_Panel(a_panel)->child, a_child);
}

void
hwt_panel_setup_(struct HWT *a_hwt)
{
	HWT_WIDGET_REGISTER(a_hwt, Panel, g_type);
}

void
destroy(struct HWT *a_hwt, struct HWTWidget *a_label)
{
	struct Panel *panel;

	panel = hwt_cast_Panel(a_label);
	hwt_widget_free(a_hwt, &panel->child);
}

void
draw(struct HWT *a_hwt, struct HWTWidget *a_label)
{
	(void)a_hwt;
	(void)a_label;
}

void
pull_min(struct HWT *a_hwt, struct HWTWidget *a_label, struct HWTSize *a_min)
{
	struct Panel *panel;

	panel = hwt_cast_Panel(a_label);
	hwt_widget_pull_min(a_hwt, panel->child, a_min);
}

void
push_rect(struct HWT *a_hwt, struct HWTWidget *a_label)
{
	struct Panel *panel;

	panel = hwt_cast_Panel(a_label);
	hwt_widget_push_rect(a_hwt, panel->child,
	    hwt_widget_get_rect(a_label));
}

enum HWTEventFlow
respond(struct HWT *a_hwt, struct HWTWidget *a_label, struct HWTEvent const
    *a_event)
{
	struct Panel *panel;

	panel = hwt_cast_Panel(a_label);
	HWT_EVENT_RESPOND(a_hwt, panel->child, a_event);
	return HWT_CONTINUE;
}
