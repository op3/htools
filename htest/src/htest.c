/*
 * Copyright (c) 2014-2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <htest/htest.h>
#include <assert.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <hutils/getopt.h>
#include <hutils/string.h>
#include <src/htest.h>

#if defined(HCONF_mHTEST_bPOSIX)
#	define SUPPORT_FORK
#	define BLUE "\033[1;34m"
#	define GREEN "\033[1;32m"
#	define RED "\033[1;31m"
#	define RESET "\033[0m"
char const c_nul_path[] = "/dev/null";
#elif defined(_MSC_VER)
#	include <io.h>
#	define SUPPORT_JMP
#	define close _close
#	define dup _dup
#	define dup2 _dup2
#	define fdopen _fdopen
#	define open _open
#	define STDERR_FILENO 1
#	define STDOUT_FILENO 2
#	define BLUE (FOREGROUND_BLUE | FOREGROUND_GREEN)
#	define GREEN FOREGROUND_GREEN
#	define RED (FOREGROUND_RED | FOREGROUND_INTENSITY)
char const c_nul_path[] = "NUL";
static jmp_buf g_suite_jmp_buf;
jmp_buf g_htest_try_jmp_buf_;
static HANDLE g_htest_console_;
static WORD RESET;
#endif

static void suite_sighandler(int);
static void try_sighandler(int);
static void usage(FILE *, char const *, int);

static int g_nul, g_old_stderr, g_old_stdout;
static int g_do_verbose;
static HTEST_COLOR_ g_color_suite;
static HTEST_COLOR_ g_color_test;
static HTEST_COLOR_ g_color_fail;

extern struct HTestSuite g_htest_suite_list_[];
void (*g_htest_dtor_)(void);

void
suite_sighandler(int a_signum)
{
	htest_output_restore_();
	htest_set_color_(g_color_fail);
	printf("  Fail:");
	htest_set_color_(RESET);
	printf("Caught signal \"%s\".\n", strsignal(a_signum));
	GCOV_FLUSH;
#if defined(SUPPORT_JMP)
	longjmp(g_suite_jmp_buf, 1);
#else
	_exit(EXIT_FAILURE);
#endif
}

void
try_sighandler(int a_signum)
{
	(void)a_signum;
	g_htest_dtor_();
	GCOV_FLUSH;
#if defined(SUPPORT_JMP)
	longjmp(g_htest_try_jmp_buf_, 1);
#else
	_exit(EXIT_FAILURE);
#endif
}

void
htest_dtor_noop_()
{
}

void
htest_output_restore_()
{
	if (g_do_verbose) {
		return;
	}
	dup2(g_old_stdout, STDOUT_FILENO);
	dup2(g_old_stderr, STDERR_FILENO);
}

void
htest_output_suppress_()
{
	if (g_do_verbose) {
		return;
	}
	fflush(stdout);
	dup2(g_nul, STDOUT_FILENO);
	fflush(stderr);
	dup2(g_nul, STDERR_FILENO);
}

void
htest_set_color_(HTEST_COLOR_ a_color)
{
#if defined(_MSC_VER)
	SetConsoleTextAttribute(g_htest_console_, a_color);
#else
	printf("%s", a_color);
#endif
}

void
htest_suite_install_sighandler_()
{
	signal(SIGABRT, suite_sighandler);
#if defined(SIGBUS)
	signal(SIGBUS, suite_sighandler);
#endif
	signal(SIGABRT, suite_sighandler);
	signal(SIGFPE, suite_sighandler);
	signal(SIGILL, suite_sighandler);
	signal(SIGSEGV, suite_sighandler);
}

void
htest_try_install_sighandler_()
{
	signal(SIGABRT, try_sighandler);
#if defined(SIGBUS)
	signal(SIGBUS, try_sighandler);
#endif
	signal(SIGFPE, try_sighandler);
	signal(SIGILL, try_sighandler);
	signal(SIGSEGV, try_sighandler);
}

void
usage(FILE *a_out, char const *a_argv0, int a_exit_code)
{
	fprintf(a_out, "Usage: %s [-c] [-h] [-r] [-v]\n", a_argv0);
	fprintf(a_out, " -c  Enable colored output.\n");
	fprintf(a_out, " -h  Show this.\n");
	fprintf(a_out, " -r  Disable failure recovery (fork/longjmp).\n");
	fprintf(a_out, " -v  Show what tested code prints to stdout/stderr.\n");
	exit(a_exit_code);
}

int
main(int const argc, char **const argv)
{
	struct HTestSuite *suite;
	int test_num, test_pass_num;
	int do_colors, do_recover;
	int opt;

	do_colors = 0;
	do_recover = 1;
	g_do_verbose = 0;
	while (-1 != (opt = getopt(argc, argv, "chrv"))) {
		switch (opt) {
		case 'c':
			do_colors = 1;
			break;
		case 'h':
			usage(stdout, argv[0], EXIT_SUCCESS);
		case 'r':
			do_recover = 0;
			break;
		case 'v':
			g_do_verbose = 1;
			break;
		default:
			usage(stderr, argv[0], EXIT_FAILURE);
		}
	}

		g_nul = open(c_nul_path, O_WRONLY);
		g_old_stdout = dup(STDOUT_FILENO);
		g_old_stderr = dup(STDERR_FILENO);
#if defined(_MSC_VER)
	_set_abort_behavior(0, _WRITE_ABORT_MSG);
	{
		CONSOLE_SCREEN_BUFFER_INFO info;

		g_htest_console_ = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(g_htest_console_, &info);
		RESET = info.wAttributes;
	}
#endif
	if (do_colors) {
		g_color_suite = GREEN;
		g_color_test = BLUE;
		g_color_fail = RED;
	}
	else {
		g_color_suite = RESET;
		g_color_test = RESET;
		g_color_fail = RESET;
	}
	htest_suite_install_sighandler_();
	test_num = 0;
	test_pass_num = 0;
	for (suite = g_htest_suite_list_; NULL != suite->header; ++suite) {
		int test_enumerator;
		int test_index;

		suite->header(g_color_suite, RESET);

		test_enumerator = 0;
		suite->suite(RESET, RESET, RESET, 0, &test_enumerator, NULL);
		if (0 == test_enumerator) {
			continue;
		}
		test_num += test_enumerator;

		for (test_index = 1; test_enumerator >= test_index;
			++test_index) {
#if defined(SUPPORT_FORK)
			if (do_recover) {
				pid_t pid;

				pid = fork();
				if (0 > pid) {
					perror(NULL);
					exit(EXIT_FAILURE);
				}
				if (0 == pid) {
					int result;

					test_enumerator = 0;
					result = 1;
					suite->suite(g_color_test,
						g_color_fail, RESET,
						test_index, &test_enumerator,
						&result);
					GCOV_FLUSH;
					_exit(result ? EXIT_SUCCESS :
						EXIT_FAILURE);
				}
				else {
					int status;

					waitpid(pid, &status, 0);
					if (EXIT_SUCCESS !=
						WEXITSTATUS(status)) {
						htest_set_color_(g_color_fail);
						printf("  Fail:");
						htest_set_color_(RESET);
						printf("Exited.\n");
					}
					else {
						++test_pass_num;
					}
				}
			}
#elif defined(SUPPORT_JMP)
			/* This is rubbish, but some OS:s cannot do more... */
			if (do_recover) {
				int result;

				if (0 == setjmp(g_suite_jmp_buf)) {
					test_enumerator = 0;
					result = 1;
					suite->suite(g_color_test,
						g_color_fail, RESET,
						test_index, &test_enumerator,
						&result);
					GCOV_FLUSH;
				}
				else {
					htest_set_color_(g_color_fail);
					printf("  Fail:");
					htest_set_color_(RESET);
					printf("Jumped.\n");
					result = 0;
				}
				if (result) {
					++test_pass_num;
				}
			}
#endif
			else {
				int result;

				test_enumerator = 0;
				result = 1;
				suite->suite(g_color_test, g_color_fail,
					RESET, test_index,
					&test_enumerator, &result);
				if (result) {
					++test_pass_num;
				}
			}
		}
	}
		close(g_nul);
		close(g_old_stdout);
		close(g_old_stderr);

	printf("Passed %d/%d (%.1f%%) tests.\n", test_pass_num, test_num,
	    100.0f * test_pass_num / test_num);
	exit(test_pass_num == test_num ? EXIT_SUCCESS : EXIT_FAILURE);
}
