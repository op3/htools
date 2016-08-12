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

#if defined(HCONF_mGETOPT_bMSC)
#	include <assert.h>
#	include <stdlib.h>
#	include <string.h>

char *optarg = NULL;
int optind = 1;
int optopt = 0;

int
getopt(int const a_argc, char *const *a_argv, char const *const a_optstring)
{
	optarg = NULL;
	optopt = 0;
	for (;; ++optind) {
		int i;

		assert(optind <= a_argc);
		if (optind == a_argc) {
			return -1;
		}
		if (0 == strcmp(a_argv[optind], "--")) {
			return -1;
		}
		if ('-' != a_argv[optind][0] || '\0' == a_argv[optind][1]) {
			continue;
		}
		optopt = a_argv[optind][1];
		for (i = 0; a_optstring[i] != optopt; ++i) {
			if ('\0' == a_optstring[i]) {
				return '?';
			}
		}
		if (':' == a_optstring[i + 1]) {
			if ('\0' != a_argv[optind][2]) {
				optarg = a_argv[optind] + 2;
			} else {
				++optind;
				if (optind == a_argc) {
					return '?';
				}
				optarg = a_argv[optind];
			}
		}
		++optind;
		return a_optstring[i];
	}
}

#endif
