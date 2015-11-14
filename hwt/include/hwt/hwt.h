/*
 * Copyright (c) 2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#ifndef HTW_HWT_H
#define HTW_HWT_H

#include <hutils/macros.h>

#define HWT_CALLBACK_SETUP(callback, prefix) do {\
	callback.draw = prefix##_draw;\
	callback.destroy = prefix##_destroy;\
	callback.propagate_min = prefix##_propagate_min;\
	callback.propagate_size = prefix##_propagate_size;\
} HUTILS_COND(while, 0)

struct HWT;
struct HWTRenderer;
struct HWTWidget;
struct HWTWidgetType;

struct HWTWidgetCallback {
	void	(*destroy)(struct HWT *, struct HWTWidget *);
	void	(*draw)(struct HWT *, struct HWTWidget *);
	void	(*propagate_size)(struct HWT *, struct HWTWidget *, struct
	    HWTSize const *);
	void	(*query_min)(struct HWT *, struct HWTWidget *, struct HWTSize
	    *);
};

struct HWT			*hwt_create(struct HWTRenderer const *)
	FUNC_RETURNS;
void				hwt_free(struct HWT **);
void				hwt_send_event(struct HWT *, struct HWTEvent
    const *);
void				hwt_set_root(struct HWT *, struct HWTWidget
    *);
void				hwt_update(struct HWT *);
void				hwt_widget_free(struct HWT *, struct HWTWidget
    **);
struct HWTWidgetType const	*hwt_widget_register(struct HWT *, char const
    *, struct HWTWidgetCallback const *) FUNC_RETURNS;

#endif
