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

#ifndef HWT_EVENT_H
#define HWT_EVENT_H

#include <hwt/common.h>

#define HWT_EVENT_RESPOND(hwt, widget, event) do {\
	if (HWT_BREAK == hwt_widget_respond(hwt, widget, event)) {\
		return HWT_BREAK;\
	}\
} HUTILS_COND(while, 0)

enum HWTEventFlow {
	HWT_BREAK,
	HWT_CONTINUE
};
enum HWTEventType {
	HWT_EVENT_EXPOSE,
	HWT_EVENT_POINTER_DOWN,
	HWT_EVENT_POINTER_MOVE,
	HWT_EVENT_POINTER_UP,
	HWT_EVENT_RESIZE
};

struct HWTEvent {
	enum	HWTEventType type;
	union {
		struct {
			struct	HWTRect rect;
		} expose;
		struct {
			float	x;
			float	y;
			int	code;
		} pointer_down;
		struct {
			float	x;
			float	y;
			int	code;
		} pointer_move;
		struct {
			float	x;
			float	y;
			int	code;
		} pointer_up;
		struct {
			struct	HWTSize size;
		} resize;
	} data;
};

void	hwt_event_expose_init(struct HWTEvent *, enum HWTEventType, float,
    float, float, float);
void	hwt_event_pointer_down_init(struct HWTEvent *, enum HWTEventType,
    float, float, int);
void	hwt_event_pointer_move_init(struct HWTEvent *, enum HWTEventType,
    float, float);
void	hwt_event_pointer_up_init(struct HWTEvent *, enum HWTEventType, float,
    float, int);
void	hwt_event_resize_init(struct HWTEvent *, enum HWTEventType, float,
    float);

#endif
