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

#ifndef HWT_RENDERER_H
#define HWT_RENDERER_H

#define HWT_RENDERER_BIND(dst) do {\
		(dst).button_draw = button_draw;\
		(dst).panel_draw = panel_draw;\
		(dst).text_create = text_create;\
		(dst).text_draw = text_draw;\
		(dst).text_free = text_free;\
	} HUTILS_COND(while, 0)

struct HWTRendererText;
struct HWTRenderer {
	void	(*button_draw)(struct Vector3f const *, struct Vector3f const
	    *);
	void	(*panel_draw)(struct Vector3f const *, struct Vector3f const
	    *);
	struct	HWTRendererText *(*text_create)(char const *) FUNC_RETURNS;
	void	(*text_draw)(struct HWTRendererText const *, struct Vector3f
	    const *);
	void	(*text_free)(struct HWTRendererText **);
};

#endif
