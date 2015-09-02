/*
 * Copyright (c) 2014, 2015
 * Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
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
#if defined(__linux__)
# include <sys/wait.h>
# include <unistd.h>
#elif defined(_WIN32)
#else
# include <sys/wait.h>
# include <signal.h>
# include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hutils/macros.h>

#if DO_GCOV_FLUSH
void __gcov_flush(void);
# define GCOV_FLUSH __gcov_flush()
#else
# define GCOV_FLUSH
#endif

struct HTestSuite {
	void	(*header)(char const *, char const *);
        void	(*suite)(char const *, char const *, char const *, int, int *,
            int *);
};

/* Test. */

#define HTEST(name)\
static void \
htest_test_header_##name##_(char const *const a_color_header_, char const \
    *const a_color_reset_)\
{\
	printf(" %sTest("__FILE__":%d:"#name")%s\n", a_color_header_,\
	    __LINE__, a_color_reset_);\
}\
static void \
htest_test_##name##_(char const *const a_color_fail_, char const *const \
    a_color_reset_, int *const a_result_)

/* Suite. */

#define HTEST_SUITE_PROTO(name)\
void htest_suite_header_##name##_(char const *, char const *);\
void htest_suite_##name##_(char const *, char const *, char const *, int, \
    int *, int *)
#define HTEST_SUITE(name)\
HTEST_SUITE_PROTO(name);\
void \
htest_suite_header_##name##_(char const *const a_color_header_, char const \
    *const a_color_reset_)\
{\
	printf("%sSuite("__FILE__":%d:"#name")%s\n", a_color_header_,\
	    __LINE__, a_color_reset_);\
}\
void \
htest_suite_##name##_(char const *const a_color_header_, char const *const \
    a_color_fail_, char const *const a_color_reset_, int const a_test_index_,\
    int *const a_test_enumerator_, int *const a_result_)
#define HTEST_ADD(name) do {\
	++*a_test_enumerator_;\
	if (*a_test_enumerator_ == a_test_index_) {\
		htest_test_header_##name##_(a_color_header_, a_color_reset_);\
		htest_output_suppress_();\
		htest_test_##name##_(a_color_fail_, a_color_reset_,\
		    a_result_);\
		htest_output_restore_();\
	}\
} HUTILS_COND(while, 0)

/* Generated suite list. */

#define HTEST_SUITE_LIST_BEGIN \
struct HTestSuite g_htest_suite_list_[] = {
#define HTEST_SUITE_LIST_ADD(name) \
	{htest_suite_header_##name##_, htest_suite_##name##_},
#define HTEST_SUITE_LIST_END \
	{NULL, NULL}\
};

/* Tests. */

#define HTRY_FAIL_ *a_result_ = 0
#define HTRY_FAIL_FMT_ "  %sFail:%s%s:%d: "
#define HTRY_FAIL_ARG_ a_color_fail_, a_color_reset_, __FILE__, __LINE__

#define HTRY(Type, fmt, a, op, b) do {\
	Type const aa_ = a;\
	Type const bb_ = b;\
	HUTILS_COND(if, !(aa_ op bb_)) {\
		htest_print_(HTRY_FAIL_FMT_"'"#a"'=%"#fmt" "#op" '"#b\
		    "'=%"#fmt"\n", HTRY_FAIL_ARG_, aa_, bb_);\
		HTRY_FAIL_;\
	}\
} HUTILS_COND(while, 0)
#define HTRY_DBL(a, op, b) HTRY(double, e, a, op, b)
#define HTRY_FLT(a, op, b) HTRY(float, e, a, op, b)
#define HTRY_I(a, op, b) HTRY(int, d, a, op, b)
#define HTRY_PTR(a, op, b) HTRY(void const *, p, a, op, b)
#define HTRY_U(a, op, b) HTRY(unsigned int, u, a, op, b)

#define HTRY_BOOL(expr) do {\
	HUTILS_COND(if, !(expr)) {\
		htest_print_(HTRY_FAIL_FMT_"'"#expr"'\n", HTRY_FAIL_ARG_);\
		HTRY_FAIL_;\
	}\
} HUTILS_COND(while, 0)

#define HTRY_STR(a, op, b) do {\
	char const *aa_ = a;\
	char const *bb_ = b;\
	if (NULL == aa_) {\
		htest_print_(HTRY_FAIL_FMT_"'"#a"'=\"%s\" "#op" '"#b\
		    "'=\"%s\".\n", HTRY_FAIL_ARG_, aa_, bb_);\
		HTRY_FAIL_;\
	} else if (NULL == bb_) {\
		htest_print_(HTRY_FAIL_FMT_"'"#a"'=\"%s\" "#op" '"#b\
		    "'=\"%s\".\n", HTRY_FAIL_ARG_, aa_, bb_);\
		HTRY_FAIL_;\
	} else if (!(strcmp(aa_, bb_) op 0)) {\
		htest_print_(HTRY_FAIL_FMT_"'"#a"'=\"%s\" "#op" '"#b\
		    "'=\"%s\".\n", HTRY_FAIL_ARG_, aa_, bb_);\
		HTRY_FAIL_;\
	}\
} HUTILS_COND(while, 0)

#define HTRY_VOID(expr) do {\
	(void)a_color_fail_;\
	(void)a_color_reset_;\
	(void)a_result_;\
	expr;\
} HUTILS_COND(while, 0)

#if defined(_MSC_VER)
# define HTRY_SIGNAL_DTOR(expr, dtor) do {\
	(void)a_color_fail_;\
	(void)a_color_reset_;\
	(void)a_result_;\
	expr;\
} HUTILS_COND(while, 0)
#else
# define HTRY_SIGNAL_DTOR(expr, dtor) do {\
	pid_t pid_;\
	int status_;\
	pid_ = fork();\
	if (0 > pid_) {\
		perror(NULL);\
		exit(EXIT_FAILURE);\
	} else if (0 == pid_) {\
		g_htest_dtor_ = dtor;\
		signal(SIGABRT, htest_sighandler_);\
		signal(SIGFPE, htest_sighandler_);\
		signal(SIGILL, htest_sighandler_);\
		signal(SIGINT, htest_sighandler_);\
		signal(SIGSEGV, htest_sighandler_);\
		signal(SIGTERM, htest_sighandler_);\
		expr;\
		htest_output_restore_();\
		g_htest_dtor_();\
		GCOV_FLUSH;\
		_exit(EXIT_SUCCESS);\
	}\
	waitpid(pid_, &status_, 0);\
	if (0 == status_) {\
		htest_print_(HTRY_FAIL_FMT_"Expected signal.\n",\
		    HTRY_FAIL_ARG_);\
		HTRY_FAIL_;\
	}\
} HUTILS_COND(while, 0)
#endif
#define HTRY_SIGNAL(expr) HTRY_SIGNAL_DTOR(expr, htest_dtor_noop_)

void htest_dtor_noop_(void);
void htest_output_restore_(void);
void htest_output_suppress_(void);
void htest_print_(char const *, ...);
void htest_sighandler_(int);

extern void (*g_htest_dtor_)(void);

#endif
