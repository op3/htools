#ifndef HUTILS_SNPRINTF_H
#define HUTILS_SNPRINTF_H

#include <stdio.h>
#include <hconf/include/hutils/snprintf.h>

#if defined(HCONF_SNPRINTF_BSD_SOURCE)
/* CPPFLAGS=-D_BSD_SOURCE */
/* LIBS=dont */
#elif defined(HCONF_SNPRINTF_PROTOTYPE)
/* LIBS=dont */
int snprintf(char *, size_t, char const *, ...);
#else
# error Not hconf:ed.
#endif

#define HCONF_TEST snprintf(NULL, 0, " ")

#endif
