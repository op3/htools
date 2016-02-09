/*
 * Copyright (c) 2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hconf.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <hutils/queue.h>

#define STRCTV_BEGIN strctv_(
#define STRCTV_END ,strctv_sentinel_)

enum DoMain {
	YES,
	NO,
	DONE
};
enum PrintType {
	ALL = 0,
	VERBOSE
};

TAILQ_HEAD(VariableList, Variable);
struct Variable {
	char	*expr;
	TAILQ_ENTRY(Variable)	next;
};

static void	argumentize(char ***, char const *);
static int	build(char const *, char const *, char const *, char const *,
    char const *, char const *, char const *);
static char	*get_option(char const *);
static int	has_main(char const *);
static void	my_exit(void);
static void	perfect(Options);
static void	print(enum PrintType, char const *, ...);
static void	resolve_variables(Options, struct VariableList *);
static char	*strctv_(char const *, ...);
static int	strecmp(char const *, char const *);
static char	*strndup_(char const *, size_t);
static void	try(char const *, struct VariableList *);
static void	unconfed(void);
static void	usage(int);
static void	write_main(void);
static int	write_hconf(Options);

static char const *strctv_sentinel_ = (char const *)&strctv_sentinel_;

static char const	c_no_option[] = "no_option";
static int	g_is_verbose;
static FILE	*g_log;
static char	*g_work_base;
static int	g_is_source;
static enum DoMain	g_do_main;
static char const	*g_base_mk;
static char const	*g_out_dir;
static char const	*g_filename;
static char	*g_filename_c;
static char	*g_filename_h;
static char	*g_filename_log;
static char	*g_filename_mk;
static char	*g_filename_o;
static char	*g_main_c;
static char	*g_main_o;
static char	*g_main_bin;
static char	*g_upper;
static Options	g_best_link;
static int	g_best_link_ret = 1e9;

void
argumentize(char ***const a_arr, char const *const a_str)
{
	struct VariableList list;
	char **arr;
	char const *p;
	void *mem;
	size_t i, num;

	TAILQ_INIT(&list);
	num = 0;
	for (p = a_str;;) {
		struct Variable *var;
		size_t len;

		for (; isspace(*p); ++p)
			;
		if ('\0' == *p) {
			break;
		}
		for (len = 0; '\0' != p[len] && !isspace(p[len]); ++len)
			;
		var = malloc(sizeof *var);
		var->expr = strndup_(p, len);
		TAILQ_INSERT_TAIL(&list, var, next);
		++num;
		p += len;
	}
	mem = calloc(num + 1, sizeof(char *));
	arr = (char **)mem;
	for (i = 0; !TAILQ_EMPTY(&list); ++i) {
		struct Variable *var;

		var = TAILQ_FIRST(&list);
		TAILQ_REMOVE(&list, var, next);
		arr[i] = var->expr;
		free(var);
	}
	assert(num == i);
	arr[i] = NULL;
	*a_arr = arr;
}

int
build(char const *const a_out, char const *const a_in, char const *const
    a_cppflags, char const *const a_cflags, char const *const a_ldflags, char
    const *const a_libs, char const *const a_extra)
{
	int pipefd[2];
	pid_t pid;
	char *cmd;
	int line_no, status;

	cmd = STRCTV_BEGIN "gcc ", a_cppflags, " ", a_cflags, " ", a_ldflags,
	    " -o ", a_out, " ", a_in, " ", a_libs, " ", a_extra STRCTV_END;
	print(VERBOSE, "%s:\n", cmd);
	if (0 != pipe(pipefd)) {
		err_(EXIT_FAILURE, "pipe");
	}
	pid = fork();
	if (-1 == pid) {
		err_(EXIT_FAILURE, "fork()");
	} else if (0 == pid) {
		char **arr;

		close(pipefd[0]);
		dup2(pipefd[1], 1);
		dup2(pipefd[1], 2);
		close(pipefd[1]);
		argumentize(&arr, cmd);
		execvp("gcc", arr);
		err_(EXIT_FAILURE, "exec(gcc)");
	}
	close(pipefd[1]);
	for (line_no = 0;;) {
		char line[1024];
		int ret;

		ret = read(pipefd[0], line, sizeof line - 1);
		if (-1 == ret) {
			err_(EXIT_FAILURE, "read(gcc)");
			break;
		}
		if (0 == ret) {
			break;
		}
		line[ret] = '\0';
		print(VERBOSE, "%s", line);
		if ('\n' == line[ret - 1]) {
			++line_no;
		}
	}
	close(pipefd[0]);
	if (-1 == wait(&status)) {
		err_(EXIT_FAILURE, "wait(gcc)");
	}
	free(cmd);
	return 0 == status ? 0 : line_no;
}

char *
get_option(char const *const a_str)
{
	char const *p;
	char const *start;

	/* [ \t]*#.*if[ \t]+defined(HCONF_ */
	for (p = a_str; isspace(*p); ++p)
		;
	if ('#' != *p) {
		return NULL;
	}
	for (++p; isspace(*p); ++p)
		;
	if (0 == strncmp(p, "if", 2)) {
		p += 2;
	} else if (0 == strncmp(p, "elif", 4)) {
		p += 4;
	} else {
		return NULL;
	}
	if (!isspace(*p)) {
		return NULL;
	}
	for (++p; isspace(*p); ++p)
		;
	if (0 != STRBCMP(p, "defined(HCONF_")) {
		return NULL;
	}
	start = p + 8;
	for (p += 14; '_' == *p || isalnum(*p); ++p) {
		if ('\0' == *p) {
			return NULL;
		}
	}
	return strndup_(start, p - start);
}

int
has_main(char const *const a_s)
{
	char const *p;

	for (p = a_s; isspace(*p); ++p)
		;
	if ('#' != *p) {
		return 0;
	}
	for (++p; isspace(*p); ++p)
		;
	if (0 != strncmp(p, "define", 6)) {
		return 0;
	}
	for (p += 6; isspace(*p); ++p)
		;
	if (0 != strncmp(p, "HCONF_MAIN", 10)) {
		return 0;
	}
	p += 10;
	return !('_' == *p || isalnum(*p));
}

void
my_exit()
{
	if (NULL != g_log) {
		fclose(g_log);
	}
	free(g_work_base);
	free(g_filename_c);
	free(g_filename_h);
	free(g_filename_log);
	free(g_filename_mk);
	free(g_filename_o);
	free(g_main_c);
	free(g_main_o);
	free(g_main_bin);
	free(g_upper);
}

void
perfect(Options a_options)
{
	print(ALL, "Perfect!\n");
	exit(EXIT_SUCCESS);
}

void
print(enum PrintType const a_print_type, char const *a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	vfprintf(g_log, a_fmt, args);
	va_end(args);

	if (ALL == a_print_type || (VERBOSE == a_print_type && g_is_verbose))
	{
		va_start(args, a_fmt);
		vprintf(a_fmt, args);
		va_end(args);
		fflush(stdout);
	}
}

void
resolve_variables(Options a_opts, struct VariableList *const a_list)
{
	FILE *file;
	FILE *pip;
	struct Variable *var;
	char *script_filename;
	int line_num;

	/* Generate executable script. */
	script_filename = STRCTV_BEGIN g_work_base, ".sh" STRCTV_END;
	file = fopen(script_filename, "wb");
	if (NULL == file) {
		err_(EXIT_FAILURE, "fopen(%s, wb)", script_filename);
	}
	TAILQ_FOREACH(var, a_list, next) {
		fprintf(file, "%s\n", var->expr);
	}
	fprintf(file, "echo $CPPFLAGS\n");
	fprintf(file, "echo $CFLAGS\n");
	fprintf(file, "echo $LDFLAGS\n");
	fprintf(file, "echo $LIBS\n");
	fprintf(file, "echo $EXTRA\n");
	fclose(file);
	if (0 != chmod(script_filename, 0700)) {
		err_(EXIT_FAILURE, "chmod(%s)", script_filename);
	}
	/* Run script and grab echo:ed variables. */
	pip = popen(script_filename, "r");
	if (NULL == pip) {
		err_(EXIT_FAILURE, "popen(%s)", script_filename);
	}
	for (line_num = 0;; ++line_num) {
		char result[STR_SIZ];
		char *p;

		if (NULL == fgets(result, sizeof result, pip)) {
			break;
		}
		if (OPT_NUM <= line_num) {
			break;
		}
		for (p = result; '\0' != *p && '\n' != *p; ++p)
			;
		*p = '\0';
		strcpy(a_opts[line_num + 1], result);
	}
	if (OPT_NUM - 1 != line_num) {
		errx_(EXIT_FAILURE, "%s: Didn't echo %d expected "
		    "variables.\n", script_filename, OPT_NUM);
	}
	pclose(pip);
	free(script_filename);
}

char *
strctv_(char const *a_s1, ...)
{
	va_list args;
	char const *from;
	char *dst, *to;
	size_t len;

	len = 0;
	va_start(args, a_s1);
	from = a_s1;
	do {
		if (NULL != from) {
			len += strlen(from);
		}
		from = va_arg(args, char const *);
	} while (strctv_sentinel_ != from);
	va_end(args);
	dst = malloc(len + 1);
	to = dst;
	va_start(args, a_s1);
	from = a_s1;
	do {
		if (NULL != from) {
			while ('\0' != *from) {
				*to++ = *from++;
			}
		}
		from = va_arg(args, char const *);
	} while (strctv_sentinel_ != from);
	va_end(args);
	*to = '\0';
	return dst;
}

int
strecmp(char const *const a_big, char const *const a_pattern)
{
	size_t big_len, pattern_len;

	big_len = strlen(a_big);
	pattern_len = strlen(a_pattern);
	if (big_len < pattern_len) {
		return -1;
	}
	return strcmp(a_big + big_len - pattern_len, a_pattern);
}

char *
strndup_(char const *const a_s, size_t a_maxlen)
{
	char *s;
	size_t len;

	len = strlen(a_s);
	if (len > a_maxlen) {
		len = a_maxlen;
	}
	s = malloc(len + 1);
	if (NULL == s) {
		err_(EXIT_FAILURE, s);
	}
	memmove(s, a_s, len);
	s[len] = '\0';
	return s;
}

void
try(char const *const a_option, struct VariableList *const a_var_list)
{
	Options options;
	char *cppflags, *cflags;
	int do_not_link, ret;

	write_main();

	print(ALL, "%s: Testing %s... ", g_filename, a_option);
	strcpy(options[0], a_option);
	resolve_variables(options, a_var_list);
	do_not_link = write_hconf(options);
	cppflags = STRCTV_BEGIN "-I. -I", g_out_dir, "/hconf_ ",
		 options[OPT_CPPFLAGS] STRCTV_END;
	cflags = STRCTV_BEGIN "-c ", options[OPT_CFLAGS] STRCTV_END;
	ret = build(g_main_o, g_main_c, cppflags, cflags, NULL, NULL, NULL);
	free(cppflags);
	free(cflags);
	if (0 == ret) {
		if (c_no_option == a_option) {
			unconfed();
		}
		if (do_not_link) {
			perfect(options);
		}
		cppflags = STRCTV_BEGIN "-I", g_out_dir, "/hconf_ ",
			 options[OPT_CPPFLAGS] STRCTV_END;
		ret = build(g_main_bin, g_main_o, cppflags, NULL,
		    options[OPT_LDFLAGS], options[OPT_LIBS],
		    options[OPT_EXTRA]);
		free(cppflags);
		if (0 == ret) {
			perfect(options);
		}
		if (ret < g_best_link_ret) {
			g_best_link_ret = ret;
			memmove(&g_best_link, options, sizeof g_best_link);
			strcpy(g_best_link[OPT_NAME], a_option);
			print(ALL, "Keeping (linking=%d).\n", ret);
		} else {
			print(ALL, "Skipping (linking=%d).\n", ret);
		}
	} else {
		if (c_no_option == a_option) {
			print(ALL, "Ok, hconfing.\n");
		} else {
			print(ALL, "Failed.\n");
		}
	}
	while (!TAILQ_EMPTY(a_var_list)) {
		struct Variable *var;

		var = TAILQ_FIRST(a_var_list);
		TAILQ_REMOVE(a_var_list, var, next);
		free(var->expr);
		free(var);
	}
}

void
unconfed()
{
	print(ALL, "Compilation must fail if not hconf:ed!\n");
	exit(EXIT_FAILURE);
}

void
usage(int const a_exit_code)
{
	fprintf(stderr, "Usage: $0 [-v] hconf dir file\n");
	fprintf(stderr, " -v     Verbose output.\n");
	fprintf(stderr, " hconf  Hconf cache file to append to.\n");
	fprintf(stderr, " dir    Generated files will be put here.\n");
	fprintf(stderr, " file   File to undergo hconf:ing.\n");
	exit(a_exit_code);
}

int
write_hconf(Options a_options)
{
	FILE *file;
	size_t i;
	int do_not_link;

	file = fopen(g_filename_h, "wb");
	if (NULL == file) {
		err_(EXIT_FAILURE, "fopen(%s, wb)", g_filename_h);
	}
	fprintf(file, "#ifndef %s\n", g_upper);
	fprintf(file, "#define %s\n", g_upper);
	fprintf(file, "#define %s\n", a_options[OPT_NAME]);
	fprintf(file, "#undef HCONF_TEST\n");
	fprintf(file, "#endif\n");
	fclose(file);

	file = fopen(g_filename_mk, "wb");
	if (NULL == file) {
		err_(EXIT_FAILURE, "fopen(%s, wb)", g_filename_mk);
	}
	fprintf(file, "\n");
	do_not_link = 0;
	for (i = 1; OPT_EXTRA > i; ++i) {
		if (OPT_LIBS == i && 0 == strcmp(a_options[i], "dont")) {
			fprintf(file, "\n");
			do_not_link = 1;
		} else {
			fprintf(file, "%s\n", a_options[i]);
		}
	}
	fclose(file);

	{
		char const *argv[2];

		argv[0] = g_base_mk;
		argv[1] = g_filename_mk;
		hconf_merge(a_options, 2, argv);
	}

	file = fopen(g_filename_mk, "wb");
	if (NULL == file) {
		err_(EXIT_FAILURE, "fopen(%s, wb)", g_filename_mk);
	}
	for (i = 0; OPT_EXTRA > i; ++i) {
		fprintf(file, "%s\n", a_options[i]);
	}
	fclose(file);

	return do_not_link;
}

void
write_main()
{
	FILE *file;

	if (DONE == g_do_main) {
		return;
	}
	file = fopen(g_main_c, "wb");
	if (NULL == file) {
		err_(EXIT_FAILURE, "fopen(%s, wb)", g_main_c);
	}
	fprintf(file, "#include <%s>\n", g_filename);
	if (YES == g_do_main) {
		fprintf(file, "int main(void) {\n");
	} else {
		fprintf(file, "extern int hconf_test_(void);\n");
		fprintf(file, "int hconf_test_() {\n");
	}
	fprintf(file, "#ifdef HCONF_TEST\n");
	fprintf(file, "\tHCONF_TEST;\n");
	fprintf(file, "#endif\n");
	fprintf(file, "\treturn 0;\n");
	fprintf(file, "}\n");
	fclose(file);
	g_do_main = DONE;
}

int
main(int argc, char const *const *argv)
{
	struct VariableList var_list;
	FILE *file;
	char *option;
	int line_no;

	atexit(my_exit);

	if (1 < argc && 0 == strcmp("-v", argv[1])) {
		g_is_verbose = 1;
		--argc;
		++argv;
	}
	if (4 != argc) {
		usage(EXIT_FAILURE);
	}

	g_base_mk = argv[1];
	g_out_dir = argv[2];
	g_filename = argv[3];

	/* Create useful filenames and the working directory. */
	{
		char const *base_start;
		char const *q;
		char *p;

		g_work_base = STRCTV_BEGIN g_out_dir, "/hconf_/hconf/",
			    g_filename STRCTV_END;
		for (p = g_work_base;; *p = '/') {
			struct stat st;

			p = strchr(p + 1, '/');
			if (NULL == p) {
				break;
			}
			if (2 <= p - g_work_base &&
			    0 == strncmp(p - 2, "../", 3)) {
				memmove(p - 2, "__/", 3);
			}
			*p = '\0';
			if (0 == stat(g_work_base, &st)) {
				if (!S_ISDIR(st.st_mode)) {
					errx_(EXIT_FAILURE, "%s: Not a "
					    "directory.", g_work_base);
				}
				continue;
			}
			if (0 != mkdir(g_work_base, 0700)) {
				err_(EXIT_FAILURE, "mkdir(%s)", g_work_base);
			}
		}
		g_filename_h = strdup(g_work_base);
		if (0 == strecmp(g_filename, ".h")) {
			g_is_source = 0;
		} else {
			size_t i;

			g_is_source = 1;
			g_filename_c = strdup(g_filename);
			for (i = strlen(g_filename_h) - 1; '.' !=
			    g_filename_h[i]; --i) {
				if (0 == i) {
					errx_(EXIT_FAILURE, "%s: Invalid "
					    "filename.", g_filename);
				}
			}
			strcpy(g_filename_h + i, ".h");
		}
		g_filename_log = STRCTV_BEGIN g_work_base, ".log" STRCTV_END;
		g_filename_mk = STRCTV_BEGIN g_work_base, ".mk" STRCTV_END;
		g_filename_o = STRCTV_BEGIN g_work_base, ".o" STRCTV_END;
		g_main_c = STRCTV_BEGIN g_work_base, "_main.c" STRCTV_END;
		g_main_o = STRCTV_BEGIN g_work_base, "_main.o" STRCTV_END;
		g_main_bin = STRCTV_BEGIN g_work_base, "_main.bin" STRCTV_END;
		g_log = fopen(g_filename_log, "wb");
		if (NULL == g_log) {
			err_(EXIT_FAILURE, "fopen(%s, wb)", g_filename_log);
		}
		for (q = g_filename; '\0' != *q; ++q) {
			if ('/' == *q) {
				base_start = q + 1;
			}
		}
		g_upper = malloc(q - base_start + 1);
		for (q = base_start, p = g_upper; '\0' != *q;) {
			*p = toupper(*q++);
			if ('.' == *p) {
				*p = '_';
			}
			++p;
		}
		*p = '\0';
	}

	TAILQ_INIT(&var_list);

	file = fopen(g_filename, "rb");
	if (NULL == file) {
		err_(EXIT_FAILURE, "fopen(%s, rb)", g_filename);
	}
	/* Try without any input, this must always fail. */
	g_do_main = YES;
	try(c_no_option, &var_list);
	g_do_main = YES;
	option = NULL;
	for (line_no = 1;; ++line_no) {
		char line[STR_SIZ];
		struct Variable *var;
		char *expr_start, *expr_end;
		char *option_new, *p;

		if (NULL == fgets(line, sizeof line, file)) {
			if (NULL == option) {
				errx_(EXIT_FAILURE, "%s: File not prepared "
				    "for hconf?", g_filename);
			}
			try(option, &var_list);
			break;
		}
		option_new = get_option(line);
		if (NULL != option_new) {
			/* New hconf option. */
			if (NULL != option) {
				/* We found one earlier, try it. */
				try(option, &var_list);
				free(option);
			}
			option = option_new;
			continue;
		}
		if (has_main(line)) {
			g_do_main = NO;
			continue;
		}
		/* Look for commented variable assignment. */
		p = strstr(line, "/*");
		if (NULL == p) {
			continue;
		}
		for (p += 2; isspace(*p); ++p)
			;
		if ('_' != *p && !isalpha(*p)) {
			continue;
		}
		expr_start = p;
		for (++p; '_' == *p || isalnum(*p); ++p)
			;
		if ('=' != *p) {
			continue;
		}
		for (; '\0' != *p; ++p) {
			if (0 == STRBCMP(p, "*/")) {
				expr_end = p;
				break;
			}
		}
		if ('\0' == *p) {
			print(ALL, "%s:%d: Is this line correct?", g_filename,
			    line_no);
			continue;
		}
		var = malloc(sizeof *var);
		var->expr = strndup_(expr_start, expr_end - expr_start);
		TAILQ_INSERT_TAIL(&var_list, var, next);
	}

	free(option);

	if ('\0' != g_best_link[OPT_NAME][0]) {
		write_hconf(g_best_link);
		exit(EXIT_SUCCESS);
	}

	/* TODO: Why is this not working? */
	fprintf(stderr, "%s: Failed, dumping log:\n", g_filename);
	fclose(g_log);
	g_log = fopen(g_filename_log, "rb");
	if (NULL == g_log) {
		err_(EXIT_FAILURE, "fopen(%s)", g_filename_log);
	}
	for (;;) {
		char line[STR_SIZ];

		if (NULL == fgets(line, sizeof line, file)) {
			if (ferror(file)) {
				err_(EXIT_FAILURE, "fgets(%s)",
				    g_filename_log);
			}
			break;
		}
		fprintf(stderr, "%s", line);
	}
	exit(EXIT_FAILURE);
}
