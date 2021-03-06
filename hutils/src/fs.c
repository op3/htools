/*
 * Copyright (c) 2016-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/fs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <hutils/memory.h>
#include <hutils/string.h>
#if defined(_MSC_VER)
#	include <direct.h>
#endif

int
fs_mkdirs(char const *a_path)
{
	char *path, *p;

	if ('\0' == *a_path) {
		return 0;
	}
	path = strdup(a_path);
	for (p = path;; *p = '/') {
		struct stat st;

		p = strchr(p + 1, '/');
		if (NULL == p) {
			FREE(path);
			return 0;
		}
		*p = '\0';
		if (0 == stat(path, &st)) {
#if !defined(_MSC_VER)
			if (!S_ISDIR(st.st_mode))
#else
			if (S_IFDIR != (S_IFMT & st.st_mode))
#endif
			{
				FREE(path);
				return -1;
			}
			continue;
		}
#if !defined(_MSC_VER)
		if (0 != mkdir(path, 0700))
#else
		if (0 != _mkdir(path))
#endif
		{
			hutils_err(EXIT_FAILURE, "mkdir(%s)", path);
		}
	}
}
