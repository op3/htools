/*
 * Copyright (c) 2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
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

#ifndef WIDGET_H
#define WIDGET_H

#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <hutils/macros.h>
#include <hwt/hwt.h>

#define HWT_CAST(typ, out, in) do {\
	if (typ != in->type) {\
		fprintf(stderr, "HWT cast mismatch (src=%s != dst=%s).\n",\
		    in->type->name, typ->name);\
		abort();\
	}\
	out = (void *)in;\
} HUTILS_COND(while, 0)

struct HWT;
struct HWTRect;

struct HWTWidgetType {
	char	const *name;
	struct	HWTWidgetCallback callback;
	TAILQ_ENTRY(HWTWidgetType)	next;
};
struct HWTWidget {
	struct	HWTWidgetType const *type;
	struct	HWTHolder *holder;
};

void widget_free(struct HWT *, struct HWTWidget **);
void widget_setup(struct HWTWidget *, struct HWTWidgetType const *);
void widget_propagate_min(struct HWT *, struct HWTWidget *, struct HWTRect *);
void widget_propagate_size(struct HWT *, struct HWTWidget *, struct HWTRect
    const *);

#endif
