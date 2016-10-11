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

#include <common.h>
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
	TAILQ_ENTRY(Flag)	original_next;
};

TAILQ_SORT_PROTOTYPE(FlagList, flag_sort);
static struct Flag	*add_flag(struct FlagList *, char const *, size_t,
    char const *, size_t) FUNC_RETURNS;
static int		cmp(struct Flag const *, struct Flag const *)
	FUNC_RETURNS;
static void		free_flag(struct Flag **);

TAILQ_SORT_DEFINE(FlagList, flag_sort, Flag, next, cmp);
char const *strctv_sentinel_ = (char const *)&strctv_sentinel_;

struct Flag *
add_flag(struct FlagList *a_list, char const *a_name, size_t a_name_size, char
    const *a_value, size_t a_value_size)
{
	struct Flag *flag;

	flag = malloc(sizeof *flag);
	flag->str = malloc(a_name_size + a_value_size + 1);
	memmove(flag->str, a_name, a_name_size);
	memmove(flag->str + a_name_size, a_value, a_value_size);
	flag->str[a_name_size + a_value_size] = '\0';
	TAILQ_INSERT_TAIL(a_list, flag, next);
	return flag;
}

void
cat_str(char *a_dst, char const *a_src, size_t a_dst_len)
{
	char const *from;
	char *to;

	for (to = a_dst; '\0' != *to; ++to)
		;
	for (from = a_src; '\0' != *from && a_dst_len - 1 > to - a_dst;) {
		*to++ = *from++;
	}
	*to = '\0';
}

int
cmp(struct Flag const *a_l, struct Flag const *a_r)
{
	return strcmp(a_l->str, a_r->str);
}

/* Ugh, reimplemented for silly platforms. */
void
err_(int a_code, char const *a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	vfprintf(stderr, a_fmt, args);
	va_end(args);
	fprintf(stderr, ": %s\n", strerror(errno));
	exit(a_code);
}

void
errx_(int a_code, char const *a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	vfprintf(stderr, a_fmt, args);
	va_end(args);
	fprintf(stderr, "\n");
	exit(a_code);
}

void
free_flag(struct Flag **a_flag)
{
	struct Flag *flag;

	flag = *a_flag;
	free(flag->str);
	free(flag);
	*a_flag = NULL;
}

/*
 * Takes an array of hconf files, merges them with some clever rules, and
 * dumps the result in a_bucket.
 *  a_argc - Number of filenames/entries in a_argv.
 *  a_argv - Array of filenames.
 */
void
merge(struct Bucket *a_bucket, int a_argc, char const *const *a_argv)
{
	struct FlagList flag_list[VAR_OUTPUT_NUM];
	struct FlagList original_list;
	int flag_num[VAR_OUTPUT_NUM];
	size_t i, idx;

	for (i = 0; VAR_OUTPUT_NUM > i; ++i) {
		a_bucket->var[i][0] = '\0';
		TAILQ_INIT(&flag_list[i]);
	}
	TAILQ_INIT(&original_list);
	memset(flag_num, 0, sizeof flag_num);
	/* Pop flags from files into flag lists. */
	for (idx = 0; a_argc > idx; ++idx) {
		FILE *file;
		char const *filename;

		filename = a_argv[idx];
		file = fopen(filename, "rb");
		if (NULL == file) {
			err_(EXIT_FAILURE, "fopen(%s, rb)", filename);
		}
		for (i = 0;; ++i) {
			char line[STR_SIZ];
			char *p;

			if (NULL == fgets(line, sizeof line, file)) {
				break;
			}
			if (VAR_OUTPUT_NUM == i) {
				++i;
				break;
			}
			for (p = line;;) {
				struct Flag *flag;
				char const *name_start, *name_end;
				char const *value_start, *value_end;
				int do_no_arg;

				for (; isspace(*p); ++p)
					;
				if ('\0' == *p) {
					break;
				}
				name_start = p;
				/*
				 * Flag = -X[ \t]*[^ \t]*, except ancient -W
				 * and Apple's weird -framework stuff.
				 */
				do_no_arg = 0;
				if (0 == STRBCMP(p, "-framework")) {
					p += 10;
					if (!isspace(*p)) {
						errx_(EXIT_FAILURE, "%s:%d: "
						    "Funny -framework flag.",
						    filename, i + 1);
					}
					/*
					 * Add a space to the name, because:
					 * "-framework yadayada".
					 */
					++p;
				} else if ('-' == p[0]) {
					do_no_arg = 0 == STRBCMP(p, "-W") &&
					    ('\0' == p[2] || isspace(p[2]));
					p += 2;
				}
				name_end = p;
				if (do_no_arg) {
					value_end = value_start = p;
				} else {
					for (; isspace(*p); ++p)
						;
					value_start = p;
					for (; '\0' != *p && !isspace(*p);
					    ++p) {
						if ('\\' == p[0] &&
						    isspace(p[1])) { ++p;
						}
					}
					value_end = p;
				}
				flag = add_flag(&flag_list[i], name_start,
				    name_end - name_start, value_start,
				    value_end - value_start);
				if (VAR_CPPFLAGS == i) {
					TAILQ_INSERT_TAIL(&original_list,
					    flag, original_next);
				}
				++flag_num[i];
			}
		}
		fclose(file);
		if (VAR_OUTPUT_NUM != i) {
			errx_(EXIT_FAILURE, "%s: Corrupt hconf.",
			    filename);
		}
	}

#define EMPTY TAILQ_EMPTY(&flag_list[i])
#define END TAILQ_END(&flag_list[i])
#define FIRST TAILQ_FIRST(&flag_list[i])
#define FREE(flag) free_flag(&flag)
#define LAST TAILQ_LAST(&flag_list[i], FlagList)
#define NEXT(flag) TAILQ_NEXT(flag, next)
#define PREV(flag) TAILQ_PREV(flag, FlagList, next)
#define REMOVE(flag) TAILQ_REMOVE(&flag_list[i], flag, next)

	/*
	 * CC: Remove dups to the right.
	 */
	i = VAR_CC;
	while (!EMPTY) {
		struct Flag *flag, *next, *nnext;

		flag = FIRST;
		REMOVE(flag);
		for (next = FIRST; END != next; next = nnext) {
			nnext = NEXT(next);
			if (0 == strcmp(flag->str, next->str)) {
				REMOVE(next);
				FREE(next);
			}
		}
		cat_str(a_bucket->var[i], flag->str, sizeof a_bucket->var[i]);
		cat_str(a_bucket->var[i], " ", sizeof a_bucket->var[i]);
		FREE(flag);
	}

	/*
	 * CPPFLAGS: Sort, remove dups + keep the largest -D values, and emit
	 * in original list order.
	 * "-I. -Da=1", "-Da=0", "-I." ->
	 * "-Da=0 -Da=1 -I. -I." ->
	 * "-I. -Da=1"
	 */
	i = VAR_CPPFLAGS;
	flag_sort(&flag_list[i], flag_num[i]);
	{
		struct Flag *prev;
		size_t prev_dname_len;

		prev = END;
		prev_dname_len = 0;
		do {
			struct Flag *flag;
			int do_write_prev;

			flag = FIRST;
			if (END != flag) {
				REMOVE(flag);
			}
			if (END == flag) {
				do_write_prev = 1;
			} else if (0 == STRBCMP(flag->str, "-D")) {
				size_t dname_len;

				for (dname_len = 2;
				    '_' == flag->str[dname_len] ||
				    isalnum(flag->str[dname_len]);
				    ++dname_len)
					;
				do_write_prev = END == prev ? 0 :
				    dname_len != prev_dname_len ||
				    0 != strncmp(flag->str, prev->str,
					dname_len);
				prev_dname_len = dname_len;
			} else {
				do_write_prev = END == prev ? 0 :
				    0 != strcmp(flag->str, prev->str);
			}
			if (END != prev && !do_write_prev) {
				TAILQ_REMOVE(&original_list, prev,
				    original_next);
				FREE(prev);
			}
			prev = flag;
		} while (END != prev);
		while (!TAILQ_EMPTY(&original_list)) {
			struct Flag *flag;

			flag = TAILQ_FIRST(&original_list);
			TAILQ_REMOVE(&original_list, flag, original_next);
			cat_str(a_bucket->var[i], flag->str, sizeof
			    a_bucket->var[i]);
			cat_str(a_bucket->var[i], " ", sizeof
			    a_bucket->var[i]);
			FREE(flag);
		}
	}

	/*
	 * CFLAGS, LDFLAGS, and LIBS: Remove dups, keeping the last.
	 * "-I1", "-I2 -I1" -> "-I2 -I1"
	 * "-la -lc", "-la -lb", "-la" -> "-lc -lb -la"
	 */
	for (i = VAR_CFLAGS; VAR_LIBS >= i; ++i) {
		struct Flag *flag;

		for (flag = LAST; END != flag; flag = PREV(flag)) {
			struct Flag *prev, *pprev;

			for (prev = PREV(flag); END != prev; prev = pprev) {
				pprev = PREV(prev);
				if (0 == strcmp(flag->str, prev->str)) {
					REMOVE(prev);
					FREE(prev);
				}
			}
		}
		while (!EMPTY) {
			flag = FIRST;
			cat_str(a_bucket->var[i], flag->str, sizeof
			    a_bucket->var[i]);
			cat_str(a_bucket->var[i], " ", sizeof
			    a_bucket->var[i]);
			REMOVE(flag);
			FREE(flag);
		}
	}
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

char *
strndup_(char const *a_s, size_t a_maxlen)
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
