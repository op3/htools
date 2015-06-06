#include <mockwidget.h>
#include <hutils/memory.h>
#include <widget.h>

struct MockWidget {
	struct	HWTWidget widget;
	struct	MockWidgetCallback user_callback;
};

static void	mock_destroy(struct HWT *, struct HWTWidget *);
static void	mock_draw(struct HWT *, struct HWTWidget *);
static void	mock_update(struct HWT *, struct HWTWidget *);

static struct HWTWidgetType const *g_type;

void
mock_destroy(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	(void)a_hwt;
	(void)a_widget;
}

void
mock_draw(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	(void)a_hwt;
	(void)a_widget;
}

void
mock_update(struct HWT *const a_hwt, struct HWTWidget *const a_widget)
{
	struct MockWidget *mock;

	(void)a_hwt;
	HWT_CAST(g_type, mock, a_widget);
	mock->user_callback.func(mock->user_callback.data);
}

struct HWTWidget *
mockwidget_create(struct MockWidgetCallback *const a_user_callback)
{
	struct MockWidget *mock;

	CALLOC(mock, 1);
	widget_setup(&mock->widget, g_type);
	memmove(&mock->user_callback, a_user_callback, sizeof
	    mock->user_callback);
	return &mock->widget;
}

void
mockwidget_setup(struct HWT *const a_hwt)
{
	struct HWTWidgetCallback callback;

	HWT_CALLBACK_SETUP(callback, mock);
	g_type = hwt_widget_register(a_hwt, "Mock", &callback);
}
