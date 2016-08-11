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

/* This header declares things needed only for creating widgets. */

#ifndef HTW_WIDGET_H
#define HTW_WIDGET_H

#include <assert.h>
#include <hutils/macros.h>
#include <hutils/memory.h>
#include <hwt/event.h>

#define HWT_CASTER(Type, typ) \
static struct Type *\
hwt_cast_##Type(struct HWTWidget *const a_widget)\
{\
	return hwt_widget_cast_(a_widget);\
}\
struct Type *hwt_cast_##Type(struct HWTWidget *)
#define HWT_WIDGET_REGISTER(hwt, Type, o_type) do {\
	struct HWTWidgetCallback callback_;\
	callback_.destroy = destroy;\
	callback_.draw = draw;\
	callback_.pull_min = pull_min;\
	callback_.push_rect = push_rect;\
	callback_.respond = respond;\
	o_type = hwt_widget_register_(hwt, sizeof(struct Type), &callback_);\
} WHILE_0

struct HWT;
struct HWTEvent;
struct HWTRect;
struct HWTSize;
struct HWTWidget;
struct HWTWidgetType;

struct HWTWidgetCallback {
	void	(*destroy)(struct HWT *, struct HWTWidget *);
	void	(*draw)(struct HWT *, struct HWTWidget *);
	void	(*pull_min)(struct HWT *, struct HWTWidget *, struct HWTSize
	    *);
	void	(*push_rect)(struct HWT *, struct HWTWidget *);
	enum	HWTEventFlow (*respond)(struct HWT *, struct HWTWidget *,
	    struct HWTEvent const *);
};

void				hwt_widget_assign(struct HWTWidget **, struct
    HWTWidget **);
void				*hwt_widget_cast_(struct HWTWidget *)
	FUNC_RETURNS;
struct HWTWidget		*hwt_widget_create(struct HWT *, struct
    HWTWidgetType const *) FUNC_RETURNS;
struct HWTRect const		*hwt_widget_get_rect(struct HWTWidget const *)
	FUNC_RETURNS;
struct HWTWidgetType const	*hwt_widget_get_type(struct HWTWidget const *)
	FUNC_RETURNS;
void				hwt_widget_pull_min(struct HWT *, struct
    HWTWidget *, struct HWTSize *);
void				hwt_widget_push_rect(struct HWT *, struct
    HWTWidget *, struct HWTRect const *);
enum HWTEventFlow		hwt_widget_respond(struct HWT *, struct
    HWTWidget *, struct HWTEvent const *) FUNC_RETURNS;
struct HWTWidgetType const	*hwt_widget_register_(struct HWT *, size_t,
    struct HWTWidgetCallback const *) FUNC_RETURNS;

#endif
