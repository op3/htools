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

#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <htest/htest.h>

#ifdef _WIN32
# include <io.h>
# define DUP(fd) _dup(fd)
# define DUP2(fd, newfd) _dup2(fd, newfd)
char const *const nul_path = "NUL";
#else
# include <unistd.h>
# define DUP(fd) dup(fd)
# define DUP2(fd, newfd) dup2(fd, newfd)
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
extern struct HTestSuite htest_suite_list_[];
void (*htest_dtor_)(void);

void
handler(int a_signum)
{
	fprintf(stderr, "  Fail: Caught signal = %s, next suite...\n",
	    strsignal(a_signum));
	_exit(EXIT_FAILURE);
}

void
htest_dtor_nop_(void)
{
}

void
htest_output_restore_()
{
	if (0 == g_do_verbose) {
		return;
	}

	assert(NULL != g_nul);
	assert(NULL != g_new_stderr);

	fflush(stdout);
	DUP2(g_old_stdout, STDOUT_FILENO);
	close(g_old_stdout);

	fflush(stderr);
	DUP2(g_old_stderr, STDERR_FILENO);
	close(g_old_stderr);

	fclose(g_nul);
	g_nul = NULL;
	fclose(g_new_stderr);
	g_new_stderr = NULL;
}

void
htest_output_suppress_()
{
	if (0 == g_do_verbose) {
		return;
	}

	assert(NULL == g_nul);
	assert(NULL == g_new_stderr);

	g_nul = fopen(nul_path, "w");

	g_old_stdout = DUP(STDOUT_FILENO);
	fflush(stdout);
	DUP2(fileno(g_nul), STDOUT_FILENO);

	g_old_stderr = DUP(STDERR_FILENO);
	fflush(stderr);
	DUP2(fileno(g_nul), STDERR_FILENO);

	g_new_stderr = fdopen(g_old_stderr, "w");
}

void
htest_print_(char const *a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	vfprintf(NULL != g_new_stderr ? g_new_stderr : stderr, a_fmt, args);
	va_end(args);
}

void
htest_sighandler_(int a_signum)
{
	(void)a_signum;
	htest_dtor_();
	_exit(EXIT_FAILURE);
}

void
usage(FILE *a_out, char const *a_argv0, int a_exit_code)
{
	fprintf(a_out, "Usage: %s [-c] [-f] [-h] [-v]\n", a_argv0);
	exit(a_exit_code);
}

int
main(int argc, char **argv)
{
	struct HTestSuite *suite;
	char const *suite_color;
	char const *test_color;
	char const *fail_color;
	char const *reset_color;
	int test_num, test_pass_num;
	int do_colors, do_fork;
	int opt;

	do_colors = 1;
	do_fork = 0;
	g_do_verbose = 1;
	while (-1 != (opt = getopt(argc, argv, "cfhv"))) {
		switch (opt) {
			case 'c':
				do_colors = 0;
				break;
			case 'f':
				do_fork = 1;
				break;
			case 'v':
				g_do_verbose = 0;
				break;
			case 'h':
				usage(stdout, argv[0], EXIT_SUCCESS);
			default:
				usage(stderr, argv[0], EXIT_FAILURE);
		}
	}

	if (0 == do_colors) {
		suite_color = GREEN;
		test_color = BLUE;
		fail_color = RED;
		reset_color = RESET;
	} else {
		suite_color = "";
		test_color = "";
		fail_color = "";
		reset_color = "";
	}

	test_num = 0;
	test_pass_num = 0;
	for (suite = htest_suite_list_; NULL != suite->header; ++suite) {
		int test_enumerator;
		int test_index;

		suite->header(suite_color, reset_color);

		test_enumerator = 0;
		suite->suite("", "", "", 0, &test_enumerator, NULL);
		if (0 == test_enumerator) {
			continue;
		}
		test_num += test_enumerator;

		for (test_index = 1; test_enumerator >= test_index;
		    ++test_index) {
			if (0 == do_fork) {
				pid_t pid;

				pid = fork();
				if (0 > pid) {
					perror(NULL);
					abort();
				}
				if (0 == pid) {
					int result;

					signal(SIGABRT, handler);
					signal(SIGFPE, handler);
					signal(SIGILL, handler);
					signal(SIGINT, handler);
					signal(SIGSEGV, handler);
					signal(SIGTERM, handler);
					test_enumerator = 0;
					result = EXIT_SUCCESS;
					suite->suite(test_color, fail_color,
					    reset_color, test_index,
					    &test_enumerator, &result);
					_exit(0 == result ? EXIT_SUCCESS :
					    EXIT_FAILURE);
				} else {
					int status;

					waitpid(pid, &status, 0);
					if (0 == status) {
						++test_pass_num;
					}
				}
			} else {
				int result;

				test_enumerator = 0;
				result = 0;
				suite->suite(test_color, fail_color,
				    reset_color, test_index, &test_enumerator,
				    &result);
				if (0 == result) {
					++test_pass_num;
				}
			}
		}
	}

	printf("Passed %d/%d (%.1f%%) tests.\n", test_pass_num, test_num,
	    100.0f * test_pass_num / test_num);
	return 0;
}
