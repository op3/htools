/*
 * Copyright (c) 2014-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
#if defined(_MSC_VER)
#	include <windows.h>
#	define HTEST_COLOR_ WORD
#else
#	include <sys/wait.h>
#	include <unistd.h>
#	define HTEST_COLOR_ char const *
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hutils/err.h>
#include <hutils/macros.h>

#if defined(HTEST_COV_FLUSH)
#	define COV_FLUSH __gcov_flush()
void __gcov_flush(void);
#else
#	define COV_FLUSH
#endif

struct HTestSuite {
	void	(*header)(HTEST_COLOR_, HTEST_COLOR_);
	void	(*suite)(HTEST_COLOR_, HTEST_COLOR_, HTEST_COLOR_, int, int *,
	    int *);
};

/* Test. */

#define HTEST(name)\
static void \
htest_test_header_##name##_(HTEST_COLOR_ a_color_header_, HTEST_COLOR_\
    a_color_reset_)\
{\
	htest_set_color_(a_color_header_);\
	printf(" Test("__FILE__":%d:"#name")", __LINE__);\
	htest_set_color_(a_color_reset_);\
	printf("\n");\
}\
static void \
htest_test_##name##_(HTEST_COLOR_ a_color_fail_, HTEST_COLOR_ a_color_reset_,\
    int *a_passed_)

/* Suite. */

#define HTEST_SUITE_PROTO(name)\
void htest_suite_header_##name##_(HTEST_COLOR_, HTEST_COLOR_);\
void htest_suite_##name##_(HTEST_COLOR_, HTEST_COLOR_, HTEST_COLOR_, int,\
    int *, int *)
#define HTEST_SUITE(name)\
HTEST_SUITE_PROTO(name);\
void \
htest_suite_header_##name##_(HTEST_COLOR_ a_color_header_, HTEST_COLOR_\
    a_color_reset_)\
{\
	htest_set_color_(a_color_header_);\
	printf("Suite("__FILE__":%d:"#name")", __LINE__);\
	htest_set_color_(a_color_reset_);\
	printf("\n");\
}\
void \
htest_suite_##name##_(HTEST_COLOR_ a_color_header_, HTEST_COLOR_ a_color_fail_,\
    HTEST_COLOR_ a_color_reset_, int a_test_index_, int *a_test_enumerator_,\
    int *a_passed_)
#define HTEST_ADD(name) do {\
	++*a_test_enumerator_;\
	if (*a_test_enumerator_ == a_test_index_) {\
		htest_test_header_##name##_(a_color_header_, a_color_reset_);\
		htest_output_suppress_();\
		htest_test_##name##_(a_color_fail_, a_color_reset_,\
		    a_passed_);\
		htest_output_restore_();\
	}\
} WHILE_0

/* Generated suite list. */

#define HTEST_SUITE_LIST_BEGIN \
struct HTestSuite g_htest_suite_list_[] = {
#define HTEST_SUITE_LIST_ADD(name) \
	{htest_suite_header_##name##_, htest_suite_##name##_},
#define HTEST_SUITE_LIST_END \
	{NULL, NULL}\
};

/* Tests. */

#define HTRY_FAIL_HEADER_ do {\
	htest_output_restore_();\
	htest_set_color_(a_color_fail_);\
	printf("  Fail:");\
	htest_set_color_(a_color_reset_);\
	printf(__FILE__":%d: ", __LINE__);\
} WHILE_0
#define HTRY_FAIL_FOOTER_ do {\
	htest_output_suppress_();\
	*a_passed_ = 0;\
} WHILE_0

#define HTRY(Type, fmt, a, op, b) do {\
	Type const aa_ = a;\
	Type const bb_ = b;\
	IF_CONST(!(aa_ op bb_)) {\
		HTRY_FAIL_HEADER_;\
		printf("'"#a"'=%"#fmt" "#op" '"#b\
		    "'=%"#fmt"\n", aa_, bb_);\
		HTRY_FAIL_FOOTER_;\
	}\
} WHILE_0

#define HTRY64(Type, fmt, a, op, b) do {\
	Type const aa_ = a;\
	Type const bb_ = b;\
	IF_CONST(!(aa_ op bb_)) {\
		HTRY_FAIL_HEADER_;\
		printf("'"#a"'=%" #fmt "%" #fmt " "#op" '"#b\
		    "'=%" #fmt "%" #fmt "\n", (uint32_t)(aa_ >> 32), \
		    (uint32_t)(aa_ & 0xffffffff), (uint32_t)(bb_ >> 32), \
		    (uint32_t)(bb_ & 0xffffffff));\
		HTRY_FAIL_FOOTER_;\
	}\
} WHILE_0

#define HTRY_DBL(a, op, b) HTRY(double, e, a, op, b)
#define HTRY_FLT(a, op, b) HTRY(float, e, a, op, b)
#define HTRY_I(a, op, b) HTRY(int, d, a, op, b)
#define HTRY_PTR(a, op, b) HTRY(void const *, p, a, op, b)
#define HTRY_C(a, op, b) HTRY(char, c, a, op, b)
#define HTRY_UC(a, op, b) HTRY(unsigned char, u, a, op, b)
#define HTRY_U(a, op, b) HTRY(unsigned int, u, a, op, b)
#define HTRY_UL(a, op, b) HTRY(unsigned long int, lu, a, op, b)
#define HTRY_ULL(a, op, b) HTRY64(uint64_t, u, a, op, b)

#define HTRY_BOOL(expr) do {\
	IF_CONST(!(expr)) {\
		HTRY_FAIL_HEADER_;\
		printf("'"#expr"'\n");\
		HTRY_FAIL_FOOTER_;\
	}\
} WHILE_0

#define HTRY_STRN(a, op, b, siz) do {\
	char const *aa_ = a;\
	char const *bb_ = b;\
	if (NULL == aa_) {\
		HTRY_FAIL_HEADER_;\
		printf("'%s'=\"%s\" "#op" '%s'=\"%s\".\n", #a, aa_, #b, bb_);\
		HTRY_FAIL_FOOTER_;\
	} else if (NULL == bb_) {\
		HTRY_FAIL_HEADER_;\
		printf("'%s'=\"%s\" "#op" '%s'=\"%s\".\n", #a, aa_, #b, bb_);\
		HTRY_FAIL_FOOTER_;\
	} else if ((size_t)-1 == siz) {\
		if (!(strcmp(aa_, bb_) op 0)) {\
			HTRY_FAIL_HEADER_;\
			printf("'%s'=\"%s\" "#op"(%d) '%s'=\"%s\".\n", #a,\
			    aa_, siz, #b, bb_); \
			HTRY_FAIL_FOOTER_; \
		}\
	} else {\
		if (!(strncmp(aa_, bb_, siz) op 0)) {\
			HTRY_FAIL_HEADER_;\
			printf("'%s'=\"%s\" "#op" '%s'=\"%s\".\n", #a, aa_,\
			    #b, bb_); \
			HTRY_FAIL_FOOTER_; \
		}\
	}\
} WHILE_0
#define HTRY_STR(a, op, b) HTRY_STRN(a, op, b, (size_t)-1)

#define HTRY_VOID(expr) do {\
	(void)a_color_fail_;\
	(void)a_color_reset_;\
	(void)a_passed_;\
	expr;\
} WHILE_0
#if defined(_MSC_VER)
#	include <setjmp.h>
#	define HTRY_SIGNAL(expr) do {\
	if (htest_do_recover()) {\
		htest_try_install_sighandler_();\
		if (0 == setjmp(g_htest_try_jmp_buf_)) {\
			expr;\
			HTRY_FAIL_HEADER_;\
			printf("Expected signal.\n");\
			HTRY_FAIL_FOOTER_;\
		}\
		htest_suite_install_sighandler_();\
	}\
} WHILE_0
extern jmp_buf g_htest_try_jmp_buf_;
#else
#	define HTRY_SIGNAL(expr) do {\
	if (htest_do_recover()) {\
		pid_t pid_;\
		int status_;\
		fflush(stdout);\
		fflush(stderr);\
		pid_ = fork();\
		if (0 > pid_) {\
			hutils_err(EXIT_FAILURE, "fork");\
		} else if (0 == pid_) {\
			htest_try_install_sighandler_();\
			expr;\
			COV_FLUSH;\
			_exit(EXIT_SUCCESS);\
		}\
		waitpid(pid_, &status_, 0);\
		if (EXIT_SUCCESS == status_) {\
			HTRY_FAIL_HEADER_;\
			printf("Expected signal.\n");\
			HTRY_FAIL_FOOTER_;\
		}\
	}\
} WHILE_0
#endif

CDECLS_BEGIN

int	htest_do_recover(void) FUNC_RETURNS;
void	htest_output_restore_(void);
void	htest_output_suppress_(void);
void	htest_set_color_(HTEST_COLOR_);
void	htest_suite_install_sighandler_(void);
void	htest_try_install_sighandler_(void);

CDECLS_END

#endif
