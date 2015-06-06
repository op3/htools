#ifndef MOCKWIDGET_H
#define MOCKWIDGET_H

#include <hutils/macros.h>

struct HWT;
struct HWTWidget;

struct MockWidgetCallback {
	void	(*func)(void *);
	void	*data;
};

struct HWTWidget *mockwidget_create(struct MockWidgetCallback *)
	RETURN_UNUSED;
void mockwidget_setup(struct HWT *);

#endif
