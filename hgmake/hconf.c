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
#include <ctype.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <hutils/queue.h>
#include <hutils/tailqsort.h>

#define NONTERMED(type) errx_(EXIT_FAILURE, "Non-terminated "type".")

TAILQ_HEAD(FlagList, Flag);
struct Flag {
	char	*str;
	TAILQ_ENTRY(Flag)	next;
};

TAILQ_SORT_PROTOTYPE(FlagList, flag_sort);
static void	add_flag(struct FlagList *, char const *, size_t);
static int	cmp(struct Flag const *, struct Flag const *);
static int	cmp_I(struct Flag const *, struct Flag const *);
static void	free_flag(struct Flag **);

TAILQ_SORT_DEFINE(FlagList, flag_sort, Flag, next, cmp);
TAILQ_SORT_DEFINE(FlagList, flag_sort_I, Flag, next, cmp_I);

void
add_flag(struct FlagList *const a_list, char const *const a_src, size_t const
    a_src_size)
{
	struct Flag *flag;

	flag = malloc(sizeof *flag);
	flag->str = malloc(a_src_size + 1);
	memmove(flag->str, a_src, a_src_size);
	flag->str[a_src_size] = '\0';
	TAILQ_INSERT_TAIL(a_list, flag, next);
}

int
cmp(struct Flag const *const a_l, struct Flag const *const a_r)
{
	return strcmp(a_l->str, a_r->str);
}

int
cmp_I(struct Flag const *const a_l, struct Flag const *const a_r)
{
	int l_I, r_I;

	/* Do not rearrange -I options, but put them at the start. */
	l_I = 0 == strncmp(a_l->str, "-I", 2);
	r_I = 0 == strncmp(a_r->str, "-I", 2);
	if (l_I && r_I) {
		return 0;
	}
	if (l_I) {
		return -1;
	}
	if (r_I) {
		return 1;
	}
	return strcmp(a_l->str, a_r->str);
}

/* Ugh, reimplemented for silly platforms. */
void
err_(int const a_code, char const *const a_fmt, ...)
{
	va_list args;

	fprintf(stderr, "^");
	va_start(args, a_fmt);
	vfprintf(stderr, a_fmt, args);
	va_end(args);
	fprintf(stderr, "#");
	fprintf(stderr, ": %s\n", strerror(errno));
	fprintf(stderr, "$");
	exit(EXIT_FAILURE);
}

void
errx_(int const a_code, char const *const a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	vfprintf(stderr, a_fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(EXIT_FAILURE);
}

void
free_flag(struct Flag **const a_flag)
{
	struct Flag *flag;

	flag = *a_flag;
	free(flag->str);
	free(flag);
	*a_flag = NULL;
}

/*
 * Takes the 'a_argc' # of hconf files 'a_argv', merges them with some clever
 * rules, and dumps the result into a_options.
 *  a_argc - Number of filenames/entries in a_argv.
 *  a_argv - Array of filenames.
 */
void
hconf_merge(Options a_options, int const a_argc, char const **const a_argv)
{
	struct FlagList flag_list[OPT_EXTRA];
	int flag_num[OPT_EXTRA];
	int ofs[OPT_EXTRA];
	FILE *file;
	size_t i, idx;

	for (i = 0; OPT_EXTRA > i; ++i) {
		a_options[i][0] = '\0';
		TAILQ_INIT(&flag_list[i]);
	}
	memset(flag_num, 0, sizeof flag_num);
	memset(ofs, 0, sizeof ofs);
	/* Pop flags from files into flag lists. */
	for (idx = 0; a_argc > idx; ++idx) {
		file = fopen(a_argv[idx], "rb");
		if (NULL == file) {
			errx_(EXIT_FAILURE, "fopen(%s, rb)", a_argv[idx]);
		}
		for (i = 0;; ++i) {
			char line[STR_SIZ];
			char *p;

			if (NULL == fgets(line, sizeof line, file)) {
				break;
			}
			if (0 == i && 0 != idx) {
				continue;
			}

			for (p = line;; ++p) {
				char const *start;

				for (; isspace(*p); ++p)
					;
				if ('\0' == *p) {
					break;
				}
				start = p;
				/* Some options can be pairs, pair them. */
				if (OPT_CPPFLAGS == i &&
				    0 == STRBCMP(p, "-I")) {
					for (p += LENGTH("-I") - 1;
					    isspace(*p); ++p)
						;
				} else if (OPT_LIBS == i &&
				    0 == STRBCMP(p, "-l")) {
					for (p += LENGTH("-l") - 1;
					    isspace(*p); ++p)
						;
				} else if (OPT_LIBS == i &&
				    0 == STRBCMP(p, "-framework ")) {
					for (p += LENGTH("-framework ") - 1;
					    isspace(*p); ++p)
						;
				}
				for (; '\0' != *p && !isspace(*p); ++p) {
					if ('\\' == *p) {
						++p;
						if ('\0' == *p) {
							NONTERMED(
							    "backslash");
						}
					} else if ('\'' == *p) {
						for (++p; '\'' != *p; ++p) {
							if ('\0' == *p) {
								NONTERMED(
								    "single-"
								    "quote");
							}
						}
					} else if ('"' == *p) {
						for (++p; '"' != *p; ++p) {
							if ('\0' == *p) {
								NONTERMED(
								    "double-"
								    "quote");
							}
						}
					}
				}
				add_flag(&flag_list[i], start, p - start);
				++flag_num[i];
			}
		}
		fclose(file);
		if (5 != i) {
			errx_(EXIT_FAILURE, "%s: Corrupt hconf.",
			    a_argv[idx]);
		}
	}

	/*
	 * What's supposed to happen:
	 *  CC       - Take the value from the first file.
	 *  CPPFLAGS - Remove dups and take the max -D values.
	 *  CFLAGS   - Remove dups.
	 *  LDFLAGS  - Remove dups.
	 *  LIBS     - Group "-framework value" and remove dups.
	 * Some code requires multiple references while linking, but that's
	 * rubbish code and whoever writes stuff like that can add explicit
	 * extra links themselves.
	 */

	/* CC. */
	while (!TAILQ_EMPTY(&flag_list[OPT_CC])) {
		struct Flag *flag;

		flag = TAILQ_FIRST(&flag_list[OPT_CC]);
		TAILQ_REMOVE(&flag_list[OPT_CC], flag, next);
		strcat(a_options[OPT_CC], flag->str);
		strcat(a_options[OPT_CC], " ");
		free_flag(&flag);
	}

	/* CPPFLAGS. */
	i = OPT_CPPFLAGS;
	flag_sort_I(&flag_list[i], flag_num[i]);
	{
		/* First do O(n^2) -I dup removal. */
		while (!TAILQ_EMPTY(&flag_list[i])) {
			struct Flag *flag, *rover, *next;

			flag = TAILQ_FIRST(&flag_list[i]);
			if (0 != strncmp(flag->str, "-I", 2)) {
				break;
			}
			TAILQ_REMOVE(&flag_list[i], flag, next);
			TAILQ_FOREACH_SAFE(rover, &flag_list[i], next, next) {
				if (0 == strcmp(flag->str, rover->str)) {
					TAILQ_REMOVE(&flag_list[i], rover,
					    next);
					free_flag(&rover);
				}
			}
			strcat(a_options[i], flag->str);
			strcat(a_options[i], " ");
			free_flag(&flag);
		}
	}
	{
		/* Then treat the rest. */
		struct Flag *prev;
		size_t prev_name_len;

		prev = NULL;
		prev_name_len = 0;
		while (!TAILQ_EMPTY(&flag_list[i])) {
			struct Flag *flag;
			size_t name_len;
			int do_write_prev;

			flag = TAILQ_FIRST(&flag_list[i]);
			TAILQ_REMOVE(&flag_list[i], flag, next);
			do_write_prev = 0;
			if (0 != strncmp(flag->str, "-D", 2)) {
				do_write_prev = NULL == prev ? 0 :
				    0 != strcmp(flag->str, prev->str);
			} else {
				for (name_len = 2;
				    '_' == flag->str[name_len] ||
				    isalnum(flag->str[name_len]); ++name_len)
					;
				do_write_prev = NULL == prev ? 0 :
				    name_len != prev_name_len || 0 !=
				    strncmp(flag->str, prev->str, name_len);
			}
			if (do_write_prev) {
				strcat(a_options[i], prev->str);
				strcat(a_options[i], " ");
			}
			if (NULL != prev) {
				free_flag(&prev);
			}
			prev = flag;
			prev_name_len = name_len;
		}
		if (NULL != prev) {
			strcat(a_options[i], prev->str);
			strcat(a_options[i], " ");
			free_flag(&prev);
		}
	}

	/* CFLAGS, LDFLAGS, and LIBS. */
	for (i = OPT_CFLAGS; OPT_LIBS >= i; ++i) {
		struct Flag *prev;

		flag_sort(&flag_list[i], flag_num[i]);
		prev = NULL;
		while (!TAILQ_EMPTY(&flag_list[i])) {
			struct Flag *flag;

			flag = TAILQ_FIRST(&flag_list[i]);
			TAILQ_REMOVE(&flag_list[i], flag, next);
			if (NULL == prev ||
			    0 != strcmp(flag->str, prev->str)) {
				if (NULL != prev) {
					free_flag(&prev);
				}
				prev = flag;
				strcat(a_options[i], flag->str);
				strcat(a_options[i], " ");
			} else {
				free_flag(&flag);
			}
		}
		if (NULL != prev) {
			free_flag(&prev);
		}
	}
}
