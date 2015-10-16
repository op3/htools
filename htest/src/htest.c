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

#include <sys/types.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <htest/htest.h>

#ifdef _MSC_VER
# include <io.h>
# include <signal.h>
# include <hutils/getopt.h>
# define STDIN_FILENO 0
# define STDOUT_FILENO 1
# define STDERR_FILENO 2
char const *const nul_path = "NUL";
#else
# include <unistd.h>
char const *const nul_path = "/dev/null";
#endif

#define BLUE "\033[1;34m"
#define GREEN "\033[1;32m"
#define RED "\033[1;31m"
#define RESET "\033[0m"

static void handler(int);
static void usage(FILE *, char const *, int);

static FILE *g_nul, *g_new_stderr;
static int g_old_stderr, g_old_stdout;
static int g_do_verbose;
static char const *g_color_suite;
static char const *g_color_test;
static char const *g_color_fail;
static char const *g_color_reset;

extern struct HTestSuite g_htest_suite_list_[];
void (*g_htest_dtor_)(void);

void
handler(int const a_signum)
{
#if defined(__linux__) || defined(__OpenBSD__)
	htest_print_("  %sFail:%sCaught signal \"%s\", next suite...\n",
	    g_color_fail, g_color_reset, strsignal(a_signum));
#else
	htest_print_("  %sFail:%sCaught signal %d, next suite...\n",
	    g_color_fail, g_color_reset, a_signum);
#endif
	GCOV_FLUSH;
	_exit(EXIT_FAILURE);
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

	assert(NULL != g_nul);
	assert(NULL != g_new_stderr);

	fflush(stdout);
	dup2(g_old_stdout, STDOUT_FILENO);
	close(g_old_stdout);

	fflush(stderr);
	dup2(g_old_stderr, STDERR_FILENO);
	close(g_old_stderr);

	fclose(g_nul);
	g_nul = NULL;
	fclose(g_new_stderr);
	g_new_stderr = NULL;
}

void
htest_output_suppress_()
{
	if (g_do_verbose) {
		return;
	}

	assert(NULL == g_nul);
	assert(NULL == g_new_stderr);

	g_nul = fopen(nul_path, "w");

	g_old_stdout = dup(STDOUT_FILENO);
	fflush(stdout);
	dup2(fileno(g_nul), STDOUT_FILENO);

	g_old_stderr = dup(STDERR_FILENO);
	fflush(stderr);
	dup2(fileno(g_nul), STDERR_FILENO);

	g_new_stderr = fdopen(g_old_stderr, "w");
}

void
htest_print_(char const *const a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	vfprintf(NULL != g_new_stderr ? g_new_stderr : stderr, a_fmt, args);
	va_end(args);
}

void
htest_sighandler_(int const a_signum)
{
	(void)a_signum;
	htest_output_restore_();
	g_htest_dtor_();
	GCOV_FLUSH;
	_exit(EXIT_FAILURE);
}

void
usage(FILE *const a_out, char const *const a_argv0, int const a_exit_code)
{
	fprintf(a_out, "Usage: %s [-c] [-f] [-h] [-v]\n", a_argv0);
	fprintf(a_out, " -c  Enable colored output.\n");
	fprintf(a_out, " -f  Disable forking of tests, nice with gdb.\n");
	fprintf(a_out, " -h  Show this.\n");
	fprintf(a_out, " -v  Show what tested code prints to "
	    "stdout/stderr.\n");
	exit(a_exit_code);
}

int
main(int const argc, char **const argv)
{
	struct HTestSuite *suite;
	int test_num, test_pass_num;
	int do_colors, do_fork;
	int opt;

	do_colors = 0;
	do_fork = 1;
	g_do_verbose = 0;
	while (-1 != (opt = getopt(argc, argv, "cfhv"))) {
		switch (opt) {
			case 'c':
				do_colors = 1;
				break;
			case 'f':
				do_fork = 0;
				break;
			case 'v':
				g_do_verbose = 1;
				break;
			case 'h':
				usage(stdout, argv[0], EXIT_SUCCESS);
			default:
				usage(stderr, argv[0], EXIT_FAILURE);
		}
	}

	if (do_colors) {
		g_color_suite = GREEN;
		g_color_test = BLUE;
		g_color_fail = RED;
		g_color_reset = RESET;
	} else {
		g_color_suite = "";
		g_color_test = "";
		g_color_fail = "";
		g_color_reset = "";
	}

	test_num = 0;
	test_pass_num = 0;
	for (suite = g_htest_suite_list_; NULL != suite->header; ++suite) {
		int test_enumerator;
		int test_index;

		suite->header(g_color_suite, g_color_reset);

		test_enumerator = 0;
		suite->suite("", "", "", 0, &test_enumerator, NULL);
		if (0 == test_enumerator) {
			continue;
		}
		test_num += test_enumerator;

		for (test_index = 1; test_enumerator >= test_index;
		    ++test_index) {
#if !defined(_MSC_VER)
			if (do_fork) {
				pid_t pid;

				pid = fork();
				if (0 > pid) {
					perror(NULL);
					exit(EXIT_FAILURE);
				}
				if (0 == pid) {
					int result;

					signal(SIGABRT, handler);
					signal(SIGFPE, handler);
					signal(SIGILL, handler);
					signal(SIGSEGV, handler);
					test_enumerator = 0;
					result = 1;
					suite->suite(g_color_test,
					    g_color_fail, g_color_reset,
					    test_index, &test_enumerator,
					    &result);
					GCOV_FLUSH;
					_exit(result ? EXIT_SUCCESS :
					    EXIT_FAILURE);
				} else {
					int status;

					waitpid(pid, &status, 0);
					if (EXIT_SUCCESS == status) {
						++test_pass_num;
					}
				}
			} else
#endif
      {
				int result;

				test_enumerator = 0;
				result = 1;
				suite->suite(g_color_test, g_color_fail,
				    g_color_reset, test_index,
				    &test_enumerator, &result);
				if (result) {
					++test_pass_num;
				}
			}
		}
	}

	printf("Passed %d/%d (%.1f%%) tests.\n", test_pass_num, test_num,
	    100.0f * test_pass_num / test_num);
	return 0;
}
