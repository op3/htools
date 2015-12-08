#ifndef HUTILS_SNPRINTF_H
#define HUTILS_SNPRINTF_H

#include <hconf/include/hutils/snprintf.h>

#if defined(HCONF_SNPRINTF_BSD_SOURCE)
/* CPPFLAGS=-D_BSD_SOURCE */
#else
# error Not hconf:ed.
#endif

#include <stdio.h>

#define HCONF_TEST snprintf(NULL, 0, " ")

#endif
