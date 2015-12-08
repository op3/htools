#ifndef HUTILS_STRSIGNAL_H
#define HUTILS_STRSIGNAL_H

#include <hconf/include/hutils/strsignal.h>

#if defined(HCONF_STRSIGNAL_POSIX_200809)
/* CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
#elif defined(HCONF_STRSIGNAL_GNU_SOURCE)
/* CPPFLAGS=-D_GNU_SOURCE */
#else
# error Not hconf:ed.
#endif

#include <string.h>

#define HCONF_TEST strsignal(0)

#endif
