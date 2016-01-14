#ifndef HUTILS_SNPRINTF_H
#define HUTILS_SNPRINTF_H

#include <hutils/macros.h>
#include <hconf/include/hutils/snprintf.h>

#if defined(HCONF_SNPRINTF_BSD_SOURCE)
/* CPPFLAGS=-D_BSD_SOURCE */
/* LIBS=dont */
# include <stdio.h>
#elif defined(HCONF_SNPRINTF_PROTOTYPE)
int snprintf(char *, size_t, char const *, ...) FUNC_PRINTF(2, 3);
#elif defined(HCONF_SNPRINTF_UNSAFE)
/* EXTRA="-Iinclude src/snprintf.c" */
# define snprintf snprintf_unsafe_
int snprintf_unsafe_(char *, size_t, char const *, ...) FUNC_PRINTF(2, 3);
#else
# error Not hconf:ed.
#endif

#define HCONF_TEST snprintf(NULL, 0, " ")

#endif
