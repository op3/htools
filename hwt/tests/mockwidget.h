#ifndef MOCKWIDGET_H
#define MOCKWIDGET_H

#include <hutils/macros.h>

struct HWT;
struct HWTRect;
struct HWTWidget;

struct MockWidgetCallback {
	void	(*func)(struct HWTRect const *, void *);
	void	*data;
};

struct HWTWidget *mockwidget_create(struct MockWidgetCallback *)
	FUNC_RETURNS;
void mockwidget_setup(struct HWT *);

#endif
