/*
 * Copyright (c) 2014 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef HTEST_HTEST_H
#define HTEST_HTEST_H

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct HTestSuite {
	void	(*header)(char const *, char const *);
	void	(*suite)(char const *, char const *, char const *, int, int
	    *const, int *const);
};

/* Test. */

#define HTEST(name) \
static void \
htest_test_header_##name##_(char const *a_color, char const *a_reset)\
{\
	printf(" %sTest("__FILE__":%d:"#name")%s\n", a_color, __LINE__,\
	    a_reset);\
}\
static void \
htest_test_##name##_(char const *a_color, char const *a_reset, int *const \
    a_result)

/* Suite. */

#define HTEST_SUITE_PROTO(name) \
void htest_suite_header_##name##_(char const *, char const *);\
void htest_suite_##name##_(char const *, char const *, char const *, int, \
    int *const, int *const)
#define HTEST_SUITE(name) \
HTEST_SUITE_PROTO(name);\
void \
htest_suite_header_##name##_(char const *a_color, char const *a_reset) \
{\
	printf("%sSuite("__FILE__":%d:"#name")%s\n", a_color, __LINE__,\
	    a_reset);\
}\
void \
htest_suite_##name##_(char const *a_color_header, char const *a_color_fail, \
    char const *a_reset, int a_test_index, int *const a_test_enumerator, int \
    *const a_result)
#define HTEST_ADD(name) \
do {\
	++*a_test_enumerator;\
	if (*a_test_enumerator == a_test_index) {\
		htest_test_header_##name##_(a_color_header, a_reset);\
		htest_test_##name##_(a_color_fail, a_reset, a_result);\
	}\
} while (0)

/* Generated suite list. */

#define HTEST_SUITE_LIST_BEGIN \
struct HTestSuite htest_suite_list_[] = {
#define HTEST_SUITE_LIST_ADD(name) \
	{htest_suite_header_##name##_, htest_suite_##name##_},
#define HTEST_SUITE_LIST_END \
	{NULL, NULL}\
};

/* Tests. */

#define HTRY_FAIL_ *a_result = 1
#define HTRY_FAIL_FMT_ "  %sFail:%s%s:%d: "
#define HTRY_FAIL_ARG_ a_color, a_reset, __FILE__, __LINE__

#define HTRY(Type, fmt, a, op, b) \
do {\
	Type aa = a;\
	Type bb = b;\
	if (!(aa op bb)) {\
		fprintf(stderr, HTRY_FAIL_FMT_"'"#a"'=%"#fmt" "#op" '"#b\
		    "'=%"#fmt"\n", HTRY_FAIL_ARG_, aa, bb);\
		HTRY_FAIL_;\
	}\
} while (0)
#define HTRY_I(a, op, b) HTRY(int, d, a, op, b)

#define HTRY_STR(a, op, b) \
do {\
	char const *aa = a;\
	char const *bb = b;\
	if (!(strcmp(aa, bb) op 0)) {\
		fprintf(stderr, HTRY_FAIL_FMT_"'"#a"'=\"%s\" "#op" '"#b\
		    "'=\"%s\".\n", HTRY_FAIL_ARG_, aa, bb);\
		HTRY_FAIL_;\
	}\
} while (0)

#define HTRY_FORK_(expr, dtor) \
pid_t pid;\
int status;\
pid = fork();\
if (0 > pid) {\
	perror(NULL);\
	abort();\
}\
if (0 == pid) {\
	htest_dtor_ = dtor;\
	signal(SIGABRT, htest_sighandler_);\
	signal(SIGFPE, htest_sighandler_);\
	signal(SIGILL, htest_sighandler_);\
	signal(SIGINT, htest_sighandler_);\
	signal(SIGSEGV, htest_sighandler_);\
	signal(SIGTERM, htest_sighandler_);\
	expr;\
	htest_dtor_();\
	_exit(0);\
}\
waitpid(pid, &status, 0)

#define HTRY_VOID_DTOR(expr, dtor) \
do {\
	HTRY_FORK_(expr, dtor);\
	if (0 != status) {\
		fprintf(stderr, HTRY_FAIL_FMT_"Caught signal.\n",\
		    HTRY_FAIL_ARG_);\
		HTRY_FAIL_;\
	}\
} while (0)
#define HTRY_VOID(expr) HTRY_VOID_DTOR(expr, htest_dtor_nop_)

#define HTRY_SIGNAL_DTOR(expr, dtor) \
do {\
	HTRY_FORK_(expr, dtor);\
	if (0 == status) {\
		fprintf(stderr, HTRY_FAIL_FMT_"Missed signal.\n",\
		    HTRY_FAIL_ARG_);\
		HTRY_FAIL_;\
	}\
} while (0)
#define HTRY_SIGNAL(expr) HTRY_SIGNAL_DTOR(expr, htest_dtor_nop_)

void htest_dtor_nop_(void);
void htest_sighandler_(int);

extern void (*htest_dtor_)(void);

#endif
