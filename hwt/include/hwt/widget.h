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

/* This header declares things needed only for creating new widgets. */

#ifndef HTW_WIDGET_H
#define HTW_WIDGET_H

#include <hutils/macros.h>

#define HWT_CASTER(Type, typ) \
static struct Type *\
hwt_cast_##Type(struct HWTWidget *const a_widget)\
{\
	assert(typ == a_widget->type);\
	return (struct Type *)a_widget;\
}\
struct Type *hwt_cast_##Type(struct HWTWidget *)
#define HWT_WIDGET_REGISTER(o_type, hwt, prefix) do {\
	struct HWTWidgetCallback callback_;\
	callback_.destroy = prefix##_destroy;\
	callback_.draw = prefix##_draw;\
	callback_.pull_min = prefix##_pull_min;\
	callback_.push_rect = prefix##_push_rect;\
	o_type = hwt_widget_register_(hwt, &callback_);\
} HUTILS_COND(while, 0)

struct HWT;
struct HWTRect;
struct HWTSize;
struct HWTWidget;
struct HWTWidgetType;

struct HWTWidgetCallback {
	void	(*destroy)(struct HWT *, struct HWTWidget *);
	void	(*draw)(struct HWT *, struct HWTWidget *);
	void	(*pull_min)(struct HWT *, struct HWTWidget *, struct HWTSize
	    *);
	void	(*push_rect)(struct HWT *, struct HWTWidget *, struct HWTRect
	    const *);
};

void				hwt_widget_init(struct HWT *, struct HWTWidget
    *, struct HWTWidgetType const *);
void				hwt_widget_pull_min(struct HWT *, struct
    HWTWidget *, struct HWTSize *);
void				hwt_widget_push_rect(struct HWT *, struct
    HWTWidget *, struct HWTRect const *);
struct HWTWidgetType const	*hwt_widget_register_(struct HWT *, struct
    HWTWidgetCallback const *) FUNC_RETURNS;

#endif
