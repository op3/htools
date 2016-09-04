/*
 * Copyright (c) 2015-2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/getopt.h>

#if defined(HCONF_mGETOPT_bCUSTOM) || defined(_MSC_VER)
#	include <stdlib.h>
#	include <string.h>
#	include <hutils/assert.h>

char *optarg = NULL;
int optopt = -1;
int optind = 1;
static int optsubind = 1;

int
getopt(int a_argc, char *const *a_argv, char const *a_optstring)
{
	optarg = NULL;
	optopt = -1;
	for (;;) {
		size_t i;

		if (-1 == optsubind) {
			++optind;
			optsubind = 1;
		}
		ASSERT(int, "d", optind, <=, a_argc);
		if (optind == a_argc || 0 == strcmp(a_argv[optind], "--")) {
			optsubind = 1;
			return -1;
		}
		if ('-' != a_argv[optind][0] || '\0' == a_argv[optind][optsubind]) {
			++optind;
			optsubind = 1;
			continue;
		}
		optopt = a_argv[optind][optsubind];
		for (i = 0; a_optstring[i] != optopt; ++i) {
			if ('\0' == a_optstring[i]) {
				optsubind = 1;
				return '?';
			}
		}
		if (':' == a_optstring[i + 1]) {
			if ('\0' != a_argv[optind][optsubind + 1]) {
				optarg = a_argv[optind] + optsubind + 1;
				optsubind = -1;
			} else {
				++optind;
				if (optind == a_argc) {
					optsubind = 1;
					return '?';
				}
				optarg = a_argv[optind];
				optsubind = -1;
			}
		} else {
			++optsubind;
			if ('\0' == a_argv[optind][optsubind]) {
				optsubind = -1;
			}
		}
		return optopt;
	}
}

#endif
