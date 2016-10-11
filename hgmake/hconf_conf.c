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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <hutils/queue.h>
#include <common.h>

struct Variable {
	char	*expr;
	int	line_no;
	TAILQ_ENTRY(Variable)	next;
};
TAILQ_HEAD(VariableList, Variable);
struct Branch {
	int	is_ok;
	char	*name;
	struct	VariableList var_list;
	struct	Bucket bucket;
};
struct Module {
	char	*name;
	struct	Branch *branch;
	TAILQ_ENTRY(Module)	next;
};
TAILQ_HEAD(ModuleList, Module);

static void		branch_free(struct Branch **);
static int		build(char const *, char const *, char const *, char
    const *, char const *, char const *, char const *);
static void		get_branch(char const *, char **, char **);
static void		log_(char const *, ...);
static struct Module	*module_add(char *);
static void		my_exit(void);
static void		resolve_variables(struct Branch *);
static void		try(void);
static void		usage(int);
static void		write_files(int);

static char const	*g_arg0;
static int	g_is_verbose;
static char const	*g_out_dir;
static char const	*g_filename;
static char const	*g_hconf_base;
static FILE	*g_log;
static char	*g_filename_h;
static char	*g_filename_hconf;
static char	*g_filename_log;
static char	*g_filename_main_bin;
static char	*g_filename_main_c;
static char	*g_filename_o;
static char	*g_filename_sh;
static char	*g_filename_upper;
static int	g_line_no;
static struct	ModuleList g_module_list =
	TAILQ_HEAD_INITIALIZER(g_module_list);

void
branch_free(struct Branch **a_branch)
{
	struct Branch *branch;

	branch = *a_branch;
	if (NULL == branch) {
		return;
	}
	free(branch->name);
	while (!TAILQ_EMPTY(&branch->var_list)) {
		struct Variable *var;

		var = TAILQ_FIRST(&branch->var_list);
		TAILQ_REMOVE(&branch->var_list, var, next);
		free(var->expr);
		free(var);
	}
	free(branch);
	*a_branch = NULL;
}

int
build(char const *a_out, char const *a_in, char const *a_cc, char const
    *a_cppflags, char const *a_cflags, char const *a_ldflags, char const
    *a_libs)
{
	FILE *file;
	int pipefd[2];
	pid_t pid;
	char *cmd;
	int line_no, status;

	/*
	 * Run in script to make sure exec* "100%" runs, because some moldy
	 * platforms do nasty things when one end is closed while accessing
	 * the other end (i.e. bug majore).
	 */
	file = fopen(g_filename_sh, "wb");
	if (NULL == file) {
		err_(EXIT_FAILURE, "fopen(%s)", g_filename_sh);
	}
	cmd = STRCTV_BEGIN a_cc, " ", a_cppflags, " ", a_cflags, " -o ",
	    a_out, " ", a_in, " ", a_ldflags, " ", a_libs STRCTV_END;
	fprintf(file, "%s", cmd);
	fclose(file);
	log_("%s\n", cmd);
	free(cmd);
	if (0 != pipe(pipefd)) {
		err_(EXIT_FAILURE, "pipe");
	}
	pid = fork();
	if (-1 == pid) {
		err_(EXIT_FAILURE, "fork()");
	} else if (0 == pid) {
		close(pipefd[0]);
		dup2(pipefd[1], 1);
		dup2(pipefd[1], 2);
		close(pipefd[1]);
		execlp("sh", "sh", g_filename_sh, NULL);
		err_(EXIT_FAILURE, "exec(sh)");
	}
	close(pipefd[1]);
	for (line_no = 0;;) {
		char buf[1024];
		char *p;
		int ret;

		ret = read(pipefd[0], buf, sizeof buf - 1);
		if (-1 == ret) {
			err_(EXIT_FAILURE, "read(sh)");
			break;
		}
		if (0 == ret) {
			break;
		}
		buf[ret] = '\0';
		log_("%s", buf);
		for (p = buf; '\0' != *p; ++p) {
			if ('\n' == *p) {
				++line_no;
			}
		}
	}
	close(pipefd[0]);
	if (-1 == waitpid(pid, &status, 0)) {
		err_(EXIT_FAILURE, "wait(sh)");
	}
	return WIFEXITED(status) && 0 == WEXITSTATUS(status) ? 0 : line_no;
}

void
get_branch(char const *a_str, char **a_module, char **a_branch)
{
	char const *p;
	char const *module_start, *module_end;
	char const *branch_start, *branch_end;

	/* # (el)if defined ( HCONF_mMODULE_bBRANCH ) */
	*a_module = NULL;
	*a_branch = NULL;
	for (p = a_str; isspace(*p); ++p)
		;
	if ('#' != *p) {
		return;
	}
	for (++p; isspace(*p); ++p)
		;
	if (0 == STRBCMP(p, "el")) {
		p += 2;
	}
	if (0 != STRBCMP(p, "if")) {
		return;
	}
	p += 2;
	if (!isspace(*p)) {
		return;
	}
	for (++p; isspace(*p); ++p)
		;
	if (0 != STRBCMP(p, "defined")) {
		return;
	}
	for (p += 7; isspace(*p); ++p)
		;
	if ('(' != *p) {
		return;
	}
	for (++p; isspace(*p); ++p)
		;
	if (0 != STRBCMP(p, "HCONF_m")) {
		return;
	}
	p += 7;
	module_start = p;
	p = strstr(p, "_b");
	if (NULL == p) {
		fprintf(stderr, "%s:%d: Hm, is this line really correct?\n",
		    g_filename, g_line_no);
		return;
	}
	module_end = p;
	p += 2;
	branch_start = p;
	for (; '_' == *p || isalnum(*p); ++p)
		;
	branch_end = p;
	*a_module = strndup_(module_start, module_end - module_start);
	*a_branch = strndup_(branch_start, branch_end - branch_start);
}

void
log_(char const *a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	vfprintf(g_log, a_fmt, args);
	va_end(args);
	if (g_is_verbose) {
		va_start(args, a_fmt);
		vprintf(a_fmt, args);
		va_end(args);
	}
}

struct Module *
module_add(char *a_name)
{
	struct Module *module;

	module = TAILQ_LAST(&g_module_list, ModuleList);
	if (TAILQ_END(&g_module_list) != module) {
		if (NULL == module->branch) {
			errx_(EXIT_FAILURE, "%s: %s failed, check log %s.",
			    g_filename, module->name, g_filename_log);
		}
	}
	if (NULL != a_name) {
		module = calloc(1, sizeof *module);
		module->name = a_name;
		TAILQ_INSERT_TAIL(&g_module_list, module, next);
	}
	return module;
}

void
my_exit()
{
	if (NULL != g_log) {
		fclose(g_log);
	}
	free(g_filename_h);
	free(g_filename_hconf);
	free(g_filename_log);
	free(g_filename_main_bin);
	free(g_filename_main_c);
	free(g_filename_o);
	free(g_filename_sh);
}

void
resolve_variables(struct Branch *a_branch)
{
	FILE *file, *pip;
	struct Bucket *bucket;
	struct Variable *var;
	size_t line_i;

	/* Generate variable extractor script. */
	file = fopen(g_filename_sh, "wb");
	if (NULL == file) {
		err_(EXIT_FAILURE, "fopen(%s, wb)", g_filename_sh);
	}
	bucket = &a_branch->bucket;
	bucket->do_link = 1;
	TAILQ_FOREACH(var, &a_branch->var_list, next) {
		if (0 == STRBCMP(var->expr, "HCONF_SRC")) {
			char const *p;

			p = strchr(var->expr, '=');
			assert(NULL != p);
			++p;
			*bucket->var[VAR_SRC] = '\0';
			cat_str(bucket->var[VAR_SRC], p, sizeof
			    bucket->var[VAR_SRC]);
		} else if (0 == STRBCMP(var->expr, "HCONF_OPT")) {
			char const *p;
			char const *nolink;

			p = strchr(var->expr, '=');
			assert(NULL != p);
			++p;
			nolink = strstr(p, "nolink");
			if (NULL != nolink) {
				bucket->do_link = 0;
			}
		} else {
			fprintf(file, "%s # %d\n", var->expr, var->line_no);
		}
	}
	fprintf(file, "echo $HCONF_CC\n");
	fprintf(file, "echo $HCONF_CPPFLAGS\n");
	fprintf(file, "echo $HCONF_CFLAGS\n");
	fprintf(file, "echo $HCONF_LDFLAGS\n");
	fprintf(file, "echo $HCONF_LIBS\n");
	fclose(file);
	if (0 != chmod(g_filename_sh, 0700)) {
		err_(EXIT_FAILURE, "chmod(%s, 0700)", g_filename_sh);
	}
	/* Run it! */
	pip = popen(g_filename_sh, "r");
	if (NULL == pip) {
		err_(EXIT_FAILURE, "popen(%s)", g_filename_sh);
	}
	for (line_i = 0;; ++line_i) {
		char line[STR_SIZ];
		char *p;

		if (NULL == fgets(line, sizeof line, pip)) {
			break;
		}
		if (VAR_OUTPUT_NUM == line_i) {
			++line_i;
			break;
		}
		for (p = line; '\0' != *p && '\n' != *p; ++p)
			;
		*p = '\0';
		memmove(bucket->var[line_i], line, p - line + 1);
	}
	pclose(pip);
	if (VAR_OUTPUT_NUM != line_i) {
		errx_(EXIT_FAILURE, "%s: Didn't echo %d expected variables.",
		    g_filename_sh, VAR_OUTPUT_NUM);
	}
}

void
try()
{
	struct Module *module;
	struct Bucket *bucket;
	char *cppflags, *cflags, *src;
	int ret;

	if (TAILQ_EMPTY(&g_module_list)) {
		return;
	}
	module = TAILQ_LAST(&g_module_list, ModuleList);
	if (module->branch->is_ok) {
		return;
	}
	log_("Module=%s, branch=%s.\n", module->name, module->branch->name);
	resolve_variables(module->branch);
	write_files(0);
	bucket = &module->branch->bucket;
	cppflags = STRCTV_BEGIN "-DHCONFING_m", module->name, " -DHCONFING_m",
		 module->name, "_b", module->branch->name, " -I. -I",
		 g_out_dir, "/_hconf ", bucket->var[VAR_CPPFLAGS] STRCTV_END;
	cflags = STRCTV_BEGIN "-c ", bucket->var[VAR_CFLAGS] STRCTV_END;
	ret = build(g_filename_o, g_filename_main_c, bucket->var[VAR_CC],
	    cppflags, cflags, "", "");
	free(cppflags);
	free(cflags);
	if (0 != ret) {
		goto try_failed;
	}
	if (!bucket->do_link) {
		goto try_passed;
	}
	cppflags = STRCTV_BEGIN "-I", g_out_dir, "/_hconf ",
		 bucket->var[VAR_CPPFLAGS] STRCTV_END;
	src = STRCTV_BEGIN g_filename_o, " ", bucket->var[VAR_SRC] STRCTV_END;
	ret = build(g_filename_main_bin, src, bucket->var[VAR_CC], cppflags,
	    bucket->var[VAR_CFLAGS], bucket->var[VAR_LDFLAGS],
	    bucket->var[VAR_LIBS]);
	free(cppflags);
	free(src);
	if (0 == ret) {
		goto try_passed;
	}
try_failed:
	log_("Failed.\n");
	branch_free(&module->branch);
	return;
try_passed:
	printf("%s: %s:%s\n", g_filename, module->name, module->branch->name);
	if (!g_is_verbose) {
		log_("Passed.\n");
	}
	module->branch->is_ok = 1;
}

void
usage(int a_exit_code)
{
	FILE *str;

	str = 0 == a_exit_code ? stdout : stderr;
	fprintf(str, "Usage: %s [-v] -d dir [-h hconf] -i header\n", g_arg0);
	fprintf(str, " -v         Optional verbose output.\n");
	fprintf(str, " -d dir     Generated files will be put here.\n");
	fprintf(str, " -h hconf   Optional hconf file to start with.\n");
	fprintf(str, " -i header  File to undergo hconf:ing.\n");
	exit(a_exit_code);
}

void
write_files(int a_write_final)
{
	FILE *f;
	struct Module *module;
	struct Bucket *bucket;
	size_t i;

	f = fopen(g_filename_h, "wb");
	if (NULL == f) {
		err_(EXIT_FAILURE, "fopen(%s, wb)", g_filename_h);
	}
	fprintf(f, "#ifndef HCONF_%s\n", g_filename_upper);
	fprintf(f, "#define HCONF_%s\n", g_filename_upper);
	if (!a_write_final) {
		fprintf(f, "#define HCONF_TEST(ret, args) extern ret "
		    "hconf_test_ args; ret hconf_test_ args\n");
	}
	TAILQ_FOREACH(module, &g_module_list, next) {
		fprintf(f, "#define HCONF_m%s_b%s\n", module->name,
		    module->branch->name);
	}
	fprintf(f, "#endif \n");
	fclose(f);

	/* Write local bucket. */
	f = fopen(g_filename_hconf, "wb");
	if (NULL == f) {
		err_(EXIT_FAILURE, "fopen(%s, wb)", g_filename_hconf);
	}
	module = TAILQ_LAST(&g_module_list, ModuleList);
	for (i = 0; VAR_OUTPUT_NUM > i; ++i) {
		int is_first;

		is_first = 1;
		TAILQ_FOREACH(module, &g_module_list, next) {
			bucket = &module->branch->bucket;
			if (!is_first) {
				fprintf(f, " ");
			}
			is_first = 0;
			fprintf(f, "%s", bucket->var[i]);
		}
		fprintf(f, "\n");
	}
	fclose(f);
	/* Merge with given base hconf into current branch bucket. */
	{
		char const *argv[2];
		size_t argc;

		argc = 0;
		if (NULL != g_hconf_base) {
			argv[argc++] = g_hconf_base;
		}
		argv[argc++] = g_filename_hconf;
		merge(bucket, argc, argv);
	}
	/* Write it. */
	f = fopen(g_filename_hconf, "wb");
	if (NULL == f) {
		err_(EXIT_FAILURE, "fopen(%s, wb)", g_filename_hconf);
	}
	for (i = 0; VAR_OUTPUT_NUM > i; ++i) {
		fprintf(f, "%s\n", bucket->var[i]);
	}
	fclose(f);
}

int
main(int argc, char const *const *argv)
{
	FILE *file;
	int argi, do_usage;

	atexit(my_exit);
	g_arg0 = argv[0];

	for (argi = 1; argc > argi; ++argi) {
#define OPT_VALUE(dst, opt) \
	if (0 == strncmp(argv[argi], opt, sizeof opt)) {\
		if ('\0' != argv[argi][2]) {\
			dst = &argv[argi][2];\
		} else if (++argi < argc) {\
			dst = argv[argi];\
		} else {\
			fprintf(stderr, "Missing value for "opt".\n");\
			usage(EXIT_FAILURE);\
		}\
	}
		if (0 == strcmp(argv[argi], "-v")) {
			g_is_verbose = 1;
		} else OPT_VALUE(g_out_dir, "-d")
		else OPT_VALUE(g_filename, "-i")
		else OPT_VALUE(g_hconf_base, "-h")
		else {
			fprintf(stderr, "Unknown argument %s.\n", argv[argi]);
			usage(EXIT_FAILURE);
		}
	}
	do_usage = 0;
	if (NULL == g_out_dir) {
		fprintf(stderr, "Missing mandatory -d argument.\n");
		do_usage = 1;
	}
	if (NULL == g_filename) {
		fprintf(stderr, "Missing mandatory -i argument.\n");
		do_usage = 1;
	}
	if (do_usage) {
		usage(EXIT_FAILURE);
	}

	/* Create useful filenames and the output directory. */
	{
		char *p;

		g_filename_h = STRCTV_BEGIN g_out_dir, "/_hconf/hconf/",
			   g_filename STRCTV_END;
		for (p = g_filename_h;; *p = '/') {
			struct stat st;

			p = strchr(p + 1, '/');
			if (NULL == p) {
				break;
			}
			if (p - 2 >= g_filename_h &&
			    0 == STRBCMP(p - 2, "../")) {
				memmove(p - 2, "__/", 3);
			}
			*p = '\0';
			if (0 == stat(g_filename_h, &st)) {
				if (!S_ISDIR(st.st_mode)) {
					errx_(EXIT_FAILURE, "%s: Not a "
					    "directory.", g_filename_h);
				}
				continue;
			}
			if (0 != mkdir(g_filename_h, 0700)) {
				err_(EXIT_FAILURE, "mkdir(%s, 0700)",
				    g_filename_h);
			}
		}
		g_filename_hconf = STRCTV_BEGIN g_filename_h, ".hconf"
		    STRCTV_END;
		g_filename_log = STRCTV_BEGIN g_filename_h, ".log" STRCTV_END;
		g_filename_main_bin = STRCTV_BEGIN g_out_dir,
				    "/_hconf/main.bin" STRCTV_END;
		g_filename_main_c = STRCTV_BEGIN g_filename_h, ".main.c"
		    STRCTV_END;
		g_filename_o = STRCTV_BEGIN g_filename_h, ".o" STRCTV_END;
		g_filename_sh = STRCTV_BEGIN g_filename_h, ".sh" STRCTV_END;
		g_filename_upper = strdup(g_filename);
		for (p = g_filename_upper; '\0' != *p; ++p) {
			if (isalpha(*p)) {
				*p = toupper(*p);
			} else if (!isdigit(*p)) {
				*p = '_';
			}
		}
		g_log = fopen(g_filename_log, "wb");
		if (NULL == g_log) {
			err_(EXIT_FAILURE, "fopen(%s, wb)", g_filename_log);
		}
		file = fopen(g_filename_main_c, "wb");
		if (NULL == file) {
			err_(EXIT_FAILURE, "fopen(%s, wb)",
			    g_filename_main_c);
		}
		fprintf(file, "#include <%s>\n", g_filename);
		fprintf(file, "int main(void) { return 0; }\n");
		fclose(file);
	}

	file = fopen(g_filename, "rb");
	if (NULL == file) {
		err_(EXIT_FAILURE, "fopen(%s, rb)", g_filename);
	}
	for (g_line_no = 1;; ++g_line_no) {
		char line[STR_SIZ];
		struct Module *module;
		struct Variable *var;
		char *module_new, *branch_new, *p;
		char *expr_start, *expr_end;

		if (NULL == fgets(line, sizeof line, file)) {
			try();
			module_add(NULL);
			break;
		}
		get_branch(line, &module_new, &branch_new);
		if (NULL != branch_new) {
			struct Branch *branch;

			try();
			module = TAILQ_LAST(&g_module_list, ModuleList);
			if (TAILQ_END(&g_module_list) != module &&
			    0 == strcmp(module->name, module_new)) {
				free(module_new);
				if (NULL != module->branch &&
				    module->branch->is_ok) {
					free(branch_new);
					continue;
				}
			} else {
				module = module_add(module_new);
			}
			branch = calloc(1, sizeof *branch);
			branch->name = branch_new;
			TAILQ_INIT(&branch->var_list);
			assert(NULL == module->branch);
			module->branch = branch;
			continue;
		}
		if (TAILQ_EMPTY(&g_module_list)) {
			continue;
		}
		/* Look for variables. */
		for (p = line; isspace(*p); ++p)
			;
		if (0 != STRBCMP(p, "/*")) {
			continue;
		}
		for (p += 2; isspace(*p); ++p)
			;
		if (0 != STRBCMP(p, "HCONF_")) {
			continue;
		}
		expr_start = p;
		for (p += 6; '_' == *p || isalnum(*p); ++p)
			;
		for (; isspace(*p); ++p)
			;
		if ('=' != *p) {
			errx_(EXIT_FAILURE, "%s:%d: Variable missing value.",
			    g_filename, g_line_no);
		}
		p = strstr(p + 1, "*/");
		if (NULL == p) {
			errx_(EXIT_FAILURE, "%s:%d: Variable missing end of "
			    "comment.", g_filename, g_line_no);
		}
		for (; isspace(p[-1]); --p)
			;
		expr_end = p;
		var = malloc(sizeof *var);
		var->expr = strndup_(expr_start, expr_end - expr_start);
		var->line_no = g_line_no;
		module = TAILQ_LAST(&g_module_list, ModuleList);
		TAILQ_INSERT_TAIL(&module->branch->var_list, var, next);
	}
	if (TAILQ_EMPTY(&g_module_list)) {
		errx_(EXIT_FAILURE, "%s: Is this file ready for hconf?",
		    g_filename);
	}
	write_files(1);
	exit(EXIT_SUCCESS);
}
