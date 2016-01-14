#include <hutils/snprintf.h>

#if defined(HCONF_SNPRINTF_UNSAFE)

# include <stdarg.h>
# include <stdio.h>

int
snprintf_unsafe_(char *const a_dst, size_t const a_dst_size, char const *const
    a_fmt, ...)
{
	va_list args;
	size_t len;

	va_start(args, a_fmt);
	len = vsprintf(a_dst, a_fmt, args);
	if (a_dst_size < len) {
		fprintf(stderr, "Overrun in snprintf_unsafe_, abort!()\n");
		abort();
	}
	va_end(args);
	return len;
}

#endif
