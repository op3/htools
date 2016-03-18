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
#include <assert.h>
#include <hutils/memory.h>
#include <hwt/hwt.h>
#include <src/holder.h>
#include <src/panel.h>
#include <src/widget.h>

struct Panel {
	struct	HWTWidget widget;
	struct	HWTHolder child;
};

static void	panel_destroy(struct HWT *, struct HWTWidget *);
static void	panel_draw(struct HWT *, struct HWTWidget *);
static void	panel_pull_min(struct HWT *, struct HWTWidget *, struct
    HWTSize *);
static void	panel_push_rect(struct HWT *, struct HWTWidget *, struct
    HWTRect const *);

static struct HWTWidgetType const *g_type;
HWT_CASTER(Panel, g_type);

struct HWTWidget *
hwt_panel_create()
{
	struct Panel *panel;

	CALLOC(panel, 1);
	hwt_widget_init(&panel->widget, g_type);
	return &panel->widget;
}

void
hwt_panel_setup_(struct HWT *const a_hwt)
{
	HWT_WIDGET_REGISTER(g_type, a_hwt, panel);
}

struct HWTHolder *
hwt_panel_get_child(struct HWTWidget *const a_panel)
{
	return &hwt_cast_Panel(a_panel)->child;
}

void
panel_destroy(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	struct Panel *panel;

	panel = hwt_cast_Panel(a_widget);
	hwt_widget_free(a_hwt, &panel->child.widget);
}

void
panel_draw(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	(void)a_hwt;
	(void)a_widget;
}

void
panel_pull_min(struct HWT *const a_hwt, struct HWTWidget *const a_widget,
    struct HWTSize *const a_min)
{
	struct Panel *panel;

	panel = hwt_cast_Panel(a_widget);
	hwt_widget_pull_min(a_hwt, panel->child.widget, a_min);
}

void
panel_push_rect(struct HWT *const a_hwt, struct HWTWidget *const a_widget,
    struct HWTRect const *const a_size)
{
	struct Panel *panel;

	panel = hwt_cast_Panel(a_widget);
	hwt_widget_push_rect(a_hwt, panel->child.widget, a_size);
}
