#ifndef HUTILS_STRDUP_H
#define HUTILS_STRDUP_H

#include <hconf/include/hutils/strdup.h>

#if defined(HCONF_STRDUP_NOTHING)
#elif defined(HCONF_STRDUP_BSD_SOURCE)
/* CPPFLAGS=-D_BSD_SOURCE */
#else
# error Not hconf:ed.
#endif

#include <string.h>

#define HCONF_TEST strdup("")

#endif
