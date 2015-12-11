#ifndef HUTILS_STRSIGNAL_H
#define HUTILS_STRSIGNAL_H

#include <hconf/include/hutils/strsignal.h>

#if defined(HCONF_STRSIGNAL_POSIX_200809)
/* CPPFLAGS=-D_POSIX_C_SOURCE=200809 */
/* LIBS=dont */
#elif defined(HCONF_STRSIGNAL_PROTOTYPE)
char *strsignal(int);
#elif defined(HCONF_STRSIGNAL_CUSTOM)
# include <signal.h>
# define strsignal(signum) (\
	SIGABRT == signum ? "SIGABRT" :\
	SIGFPE == signum ? "SIGFPE" :\
	SIGILL == signum ? "SIGILL" :\
	SIGINT == signum ? "SIGINT" :\
	SIGSEGV == signum ? "SIGSEGV" :\
	SIGTERM == signum ? "SIGTERM" : "SIGUnknown")
#else
# error Not hconf:ed.
#endif

#include <string.h>

#define HCONF_TEST (void)strsignal(0)

#endif
