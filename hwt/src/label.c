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

#include <hwt/label.h>
#include <hutils/strdup.h>
#include <hwt/widget.h>
#include <src/builtin.h>

struct Label {
	char	*text;
};

static void			destroy(struct HWT *, struct HWTWidget *);
static void			draw(struct HWT *, struct HWTWidget *);
static void			pull_min(struct HWT *, struct HWTWidget *,
    struct HWTSize *);
static void			push_rect(struct HWT *, struct HWTWidget *);
static enum HWTEventFlow	respond(struct HWT *, struct HWTWidget *,
    struct HWTEvent const *);

static struct HWTWidgetType const *g_type;
HWT_CASTER(Label, g_type);

struct HWTWidget *
hwt_label_create(struct HWT *const a_hwt, char const *const a_text)
{
	struct Label *label;
	struct HWTWidget *widget;

	widget = hwt_widget_create(a_hwt, g_type);
	label = hwt_cast_Label(widget);
	label->text = strdup(a_text);
	return widget;
}

void
hwt_label_setup_(struct HWT *const a_hwt)
{
	HWT_WIDGET_REGISTER(a_hwt, Label, g_type);
}

void
destroy(struct HWT *const a_hwt, struct HWTWidget *const a_label)
{
	struct Label *label;

	(void)a_hwt;
	label = hwt_cast_Label(a_label);
	FREE(label->text);
}

void
draw(struct HWT *const a_hwt, struct HWTWidget *const a_label)
{
	(void)a_hwt;
	(void)a_label;
}

void
pull_min(struct HWT *const a_hwt, struct HWTWidget *const a_label, struct
    HWTSize *const a_min)
{
	(void)a_hwt;
	(void)a_label;
	(void)a_min;
}

void
push_rect(struct HWT *const a_hwt, struct HWTWidget *const a_label)
{
	(void)a_hwt;
	(void)a_label;
}

enum HWTEventFlow
respond(struct HWT *const a_hwt, struct HWTWidget *const a_label, struct
    HWTEvent const *const a_event)
{
	(void)a_hwt;
	(void)a_label;
	(void)a_event;
	return HWT_CONTINUE;
}
