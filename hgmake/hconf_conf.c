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
	char	*name;
	struct	VariableList var_list;
	struct	Bucket bucket;
};
struct Module {
	char	*name;
	struct	Branch *branch;
	struct	Branch *best_branch;
	int	best_score;
	TAILQ_ENTRY(Module)	next;
};
TAILQ_HEAD(ModuleList, Module);

static void		argumentize(char ***, char const *);
static void		branch_free(struct Branch **);
static int		build(char const *, char const *, char const *, char
    const *, char const *, char const *, char const *);
static void		get_branch(char const *, char **, char **);
static void		log_(char const *, ...);
static struct Module	*module_add(char *);
static void		my_exit(void);
static void		perfect(void);
static void		resolve_variables(struct Branch *);
static void		set_best(struct Module *, int);
static void		try(void);
static void		usage(int);
static void		write_files(int);

static char const	*g_arg0;
static int	g_is_verbose;
static char const	*g_out_dir;
static char const	*g_filename;
static char const	*g_hconf_base;
static int	g_is_header;
static FILE	*g_log;
static char	*g_out_base;
static char	*g_filename_h;
static char	*g_filename_hconf;
static char	*g_filename_log;
static char	*g_filename_main_bin;
static char	*g_filename_main_c;
static char	*g_filename_main_o;
static char	*g_filename_o;
static char	*g_filename_sh;
static char	*g_filename_upper;
static int	g_line_no;
static struct	ModuleList g_module_list =
	TAILQ_HEAD_INITIALIZER(g_module_list);

void
argumentize(char ***const a_arr, char const *const a_str)
{
	char **arr;
	char const *p;
	size_t num;

	num = 0;
	for (p = a_str;;) {
		for (; isspace(*p); ++p)
			;
		if ('\0' == *p) {
			break;
		}
		for (; '\0' != *p && !isspace(*p); ++p)
			;
		++num;
	}
	arr = calloc(num + 1, sizeof(char *));
	num = 0;
	for (p = a_str;;) {
		size_t len;

		for (; isspace(*p); ++p)
			;
		if ('\0' == *p) {
			break;
		}
		for (len = 0; '\0' != p[len] && !isspace(p[len]); ++len)
			;
		arr[num++] = strndup_(p, len);
		p += len;
	}
	arr[num] = NULL;
	*a_arr = arr;
}

void
branch_free(struct Branch **const a_branch)
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
build(char const *const a_out, char const *const a_in, char const *const a_cc,
    char const *const a_cppflags, char const *const a_cflags, char const
    *const a_ldflags, char const *const a_libs)
{
	int pipefd[2];
	pid_t pid;
	char *cmd, *prog, *p;
	int line_no, status;

	cmd = STRCTV_BEGIN a_cc, " ", a_cppflags, " ", a_cflags, " -o ",
	    a_out, " ", a_in, " ", a_ldflags, " ", a_libs STRCTV_END;
	log_("%s\n", cmd);
	if (0 != pipe(pipefd)) {
		err_(EXIT_FAILURE, "pipe");
	}
	p = strchr(cmd, ' ');
	if (NULL == p) {
		errx_(EXIT_FAILURE, "Invalid build command.");
	}
	prog = strndup_(cmd, p - cmd);
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
		execvp(prog, arr);
		err_(EXIT_FAILURE, "exec(%s)", prog);
	}
	close(pipefd[1]);
	for (line_no = 0;;) {
		char line[1024];
		int ret;

		ret = read(pipefd[0], line, sizeof line - 1);
		if (-1 == ret) {
			err_(EXIT_FAILURE, "read(%s)", prog);
			break;
		}
		if (0 == ret) {
			break;
		}
		line[ret] = '\0';
		log_("%s", line);
		for (p = line;; ++p) {
			p = strchr(p, '\n');
			if (NULL == p) {
				break;
			}
			++line_no;
		}
	}
	close(pipefd[0]);
	if (-1 == wait(&status)) {
		err_(EXIT_FAILURE, "wait(%s)", prog);
	}
	free(cmd);
	free(prog);
	return 0 == status ? 0 : line_no;
}

void
get_branch(char const *const a_str, char **const a_module, char **const
    a_branch)
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
	*a_module = strndup(module_start, module_end - module_start);
	*a_branch = strndup(branch_start, branch_end - branch_start);
}

void
log_(char const *const a_fmt, ...)
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
module_add(char *const a_name)
{
	struct Module *module;

	module = TAILQ_LAST(&g_module_list, ModuleList);
	if (TAILQ_END(&g_module_list) != module) {
		if (NULL == module->best_branch) {
			errx_(EXIT_FAILURE, "%s: Module %s failed, check log "
			    "%s.", g_filename, module->name, g_filename_log);
		}
	}
	if (NULL != a_name) {
		module = calloc(1, sizeof *module);
		module->name = a_name;
		module->best_score = 1e9;
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
	free(g_out_base);
	free(g_filename_h);
	free(g_filename_hconf);
	free(g_filename_log);
	free(g_filename_o);
	free(g_filename_main_bin);
	free(g_filename_main_c);
	free(g_filename_main_o);
	free(g_filename_sh);
}

void
perfect(void)
{
	struct Module *module;

	log_("Perfect!\n");
	module = TAILQ_LAST(&g_module_list, ModuleList);
	set_best(module, 0);
}

void
resolve_variables(struct Branch *const a_branch)
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
	bucket->penalty = 0;
	TAILQ_FOREACH(var, &a_branch->var_list, next) {
		if (0 == STRBCMP(var->expr, "HCONF_SRC")) {
		} else if (0 == STRBCMP(var->expr, "HCONF_OPT")) {
			char const *p;
			char const *nolink;
			char const *penalty;

			p = strchr(var->expr, '=');
			assert(NULL != p);
			++p;
			nolink = strstr(p, "nolink");
			if (NULL != nolink) {
				bucket->do_link = 0;
			}
			penalty = strstr(p, "penalty");
			if (NULL != penalty) {
				char *end;

				for (p = penalty + 7; isspace(*p); ++p)
					;
				if ('=' != *p) {
					errx_(EXIT_FAILURE, "%s:%d: Penalty "
					    "option missing '='.", g_filename,
					    var->line_no);
				}
				++p;
				bucket->penalty = strtol(p, &end, 10);
				if (p == end) {
					errx_(EXIT_FAILURE, "%s:%d: Penalty "
					    "option missing number.",
					    g_filename, var->line_no);
				}
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
set_best(struct Module *const a_module, int const a_score)
{
	branch_free(&a_module->best_branch);
	a_module->best_branch = a_module->branch;
	a_module->branch = NULL;
	a_module->best_score = a_score;
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
	if (0 == module->best_score) {
		return;
	}
	log_("Module=%s, branch=%s.\n", module->name, module->branch->name);
	resolve_variables(module->branch);
	write_files(0);
	bucket = &module->branch->bucket;
	cppflags = STRCTV_BEGIN "-I. -I", g_out_dir, "/_hconf -DHCONFING_m",
		 module->name, " ", bucket->var[VAR_CPPFLAGS] STRCTV_END;
	cflags = STRCTV_BEGIN "-c ", bucket->var[VAR_CFLAGS] STRCTV_END;
	ret = g_is_header ?
	    build(g_filename_main_o, g_filename_main_c, bucket->var[VAR_CC],
		cppflags, cflags, NULL, NULL) :
	    build(g_filename_o, g_filename, bucket->var[VAR_CC], cppflags,
		cflags, NULL, NULL);
	free(cppflags);
	free(cflags);
	if (0 != ret) {
		log_("Failed.\n");
		return;
	}
	if (!bucket->do_link) {
		perfect();
		return;
	}
	src = g_is_header ?
	    STRCTV_BEGIN bucket->var[VAR_SRC], " ", g_filename_main_o
	    STRCTV_END :
	    STRCTV_BEGIN bucket->var[VAR_SRC], " ", g_filename_o, " ",
			 g_filename_main_c STRCTV_END;
	cppflags = STRCTV_BEGIN "-I. -I", g_out_dir, "/_hconf",
		 bucket->var[VAR_CPPFLAGS] STRCTV_END;
	ret = build(g_filename_main_bin, src, bucket->var[VAR_CC], cppflags,
	    NULL, bucket->var[VAR_LDFLAGS], bucket->var[VAR_LIBS]) +
	    bucket->penalty;
	free(src);
	free(cppflags);
	if (0 == ret) {
		perfect();
		return;
	}
	if (module->best_score > ret) {
		set_best(module, ret);
		log_("Keeping (score=%d).\n", ret);
	} else {
		log_("Skipping (score=%d).\n", ret);
	}
}

void
usage(int const a_exit_code)
{
	FILE *str;

	str = 0 == a_exit_code ? stdout : stderr;
	fprintf(str, "Usage: %s [-v] -d dir [-h hconf] -i file\n", g_arg0);
	fprintf(str, " -v        Optional verbose output.\n");
	fprintf(str, " -d dir    Generated files will be put here.\n");
	fprintf(str, " -h hconf  Optional hconf file to start with.\n");
	fprintf(str, " -i file   File to undergo hconf:ing.\n");
	exit(a_exit_code);
}

void
write_files(int const a_write_final)
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
		    a_write_final ? module->best_branch->name :
		    NULL != module->branch ? module->branch->name :
		    module->best_branch->name);
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
			bucket = a_write_final ? &module->best_branch->bucket :
			    NULL != module->branch ? &module->branch->bucket :
			    &module->best_branch->bucket;
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

		argv[0] = g_hconf_base;
		argv[1] = g_filename_hconf;
		merge(bucket, 2, argv);
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
	struct Module *module;
	int argi, do_usage;

	atexit(my_exit);
	g_arg0 = argv[0];

	do_usage = 0;
	for (argi = 1; argc > argi; ++argi) {
#define OPT_VALUE(dst, opt) \
	if (0 == strncmp(argv[argi], opt, sizeof opt)) {\
		if ('\0' != argv[argi][2]) {\
			dst = &argv[argi][2];\
		} else if (++argi < argc) {\
			dst = argv[argi];\
		} else {\
			fprintf(stderr, "Missing value for "opt".\n");\
			do_usage = 1;\
			break;\
		}\
	}
		if (0 == strcmp(argv[argi], "-v")) {
			g_is_verbose = 1;
		} else OPT_VALUE(g_out_dir, "-d")
		else OPT_VALUE(g_filename, "-i")
		else OPT_VALUE(g_hconf_base, "-h")
		else {
			fprintf(stderr, "Unknown argument %s.\n", argv[argi]);
			do_usage = 1;
			break;
		}
	}
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

		g_out_base = STRCTV_BEGIN g_out_dir, "/_hconf/hconf/",
			   g_filename STRCTV_END;
		for (p = g_out_base;; *p = '/') {
			struct stat st;

			p = strchr(p + 1, '/');
			if (NULL == p) {
				break;
			}
			if (p - 2 >= g_out_base &&
			    0 == STRBCMP(p - 2, "../")) {
				memmove(p - 2, "__/", 3);
			}
			*p = '\0';
			if (0 == stat(g_out_base, &st)) {
				if (!S_ISDIR(st.st_mode)) {
					errx_(EXIT_FAILURE, "%s: Not a "
					    "directory.", g_out_base);
				}
				continue;
			}
			if (0 != mkdir(g_out_base, 0700)) {
				err_(EXIT_FAILURE, "mkdir(%s, 0700)",
				    g_out_base);
			}
		}
		g_filename_h = strdup(g_out_base);
		g_is_header = 0 == STRECMP(g_filename, ".h");
		if (!g_is_header) {
			p = strrchr(g_filename_h, '.');
			if (NULL == p || 4 < strlen(p)) {
				errx_(EXIT_FAILURE, "%s: Strange input "
				    "filename.", g_filename);
			}
			memmove(p, ".h", 3);
		}
		g_filename_hconf = STRCTV_BEGIN g_out_base, ".hconf"
		    STRCTV_END;
		g_filename_log = STRCTV_BEGIN g_out_base, ".log" STRCTV_END;
		g_filename_main_bin = STRCTV_BEGIN g_out_dir,
				    "/_hconf/main.bin" STRCTV_END;
		g_filename_main_c = STRCTV_BEGIN g_out_base, ".main.c"
		    STRCTV_END;
		g_filename_main_o = STRCTV_BEGIN g_out_dir, "/_hconf/main.o"
		    STRCTV_END;
		g_filename_o = STRCTV_BEGIN g_out_base, ".o" STRCTV_END;
		g_filename_sh = STRCTV_BEGIN g_out_base, ".sh" STRCTV_END;
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
		if (g_is_header) {
			fprintf(file, "#include <%s>\n", g_filename);
		}
		fprintf(file, "#ifndef HCONF_HAS_MAIN\n");
		fprintf(file, "int main(void) { return 0; }\n");
		fprintf(file, "#else\n");
		fprintf(file, "int main_not_really_(void) { return 0; }\n");
		fprintf(file, "#endif\n");
		fclose(file);
	}

	file = fopen(g_filename, "rb");
	if (NULL == file) {
		err_(EXIT_FAILURE, "fopen(%s, rb)", g_filename);
	}
	for (g_line_no = 1;; ++g_line_no) {
		char line[STR_SIZ];
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
			} else {
				module = module_add(module_new);
			}
			branch_free(&module->branch);
			branch = calloc(1, sizeof *branch);
			branch->name = branch_new;
			TAILQ_INIT(&branch->var_list);
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
	fclose(g_log);
	write_files(1);
	exit(EXIT_SUCCESS);
}
