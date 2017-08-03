/*
 * Copyright (c) 2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/file.h>
#include <sys/stat.h>
#include <hutils/err.h>
#include <hutils/fmtmod.h>
#include <hutils/memory.h>

void *
file_load(char const *a_path, size_t a_max_size)
{
	struct stat st;
	FILE *file;
	char *buf;

	if (0 != stat(a_path, &st)) {
		hutils_warn("stat(%s)", a_path);
		return NULL;
	}
	if ((size_t)st.st_size > a_max_size) {
		hutils_warnx("file_load(%s).size=%"PRIz" > max_size=%"PRIz".",
		    a_path, (size_t)st.st_size, a_max_size);
		return NULL;
	}
	file = fopen(a_path, "rb");
	if (NULL == file) {
		hutils_warn("fopen(%s)", a_path);
		return NULL;
	}
	buf = malloc(st.st_size + 1);
	if (NULL == buf) {
		hutils_warn("malloc(%lu)", (unsigned long)st.st_size);
		return NULL;
	}
	if (fread(buf, 1, st.st_size, file) != (size_t)st.st_size) {
		hutils_warn("fread(%s)", a_path);
		fclose(file);
		return NULL;
	}
	fclose(file);
	buf[st.st_size] = '\0';
	return buf;
}
