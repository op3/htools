/*
 * Copyright (c) 2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/dir.h>
#include <hutils/memory.h>

#if defined(_MSC_VER)

struct Dir {
	HANDLE	handle;
	char	*first;
};

struct Dir *
dir_open(char const *const a_path)
{
	WIN32_FIND_DATA find_data;
	HANDLE handle;
	struct Dir *dir;
	char *path;
	size_t len;

	len = strlen(a_path);
	CALLOC(path, len + 3);
	strcpy(path, a_path);
	strcpy(path + len, "\\*");
	handle = FindFirstFile(path, &find_data);
	FREE(path);
	if (INVALID_HANDLE_VALUE == handle) {
		return NULL;
	}

	CALLOC(dir, 1);
	dir->handle = handle;
	STRDUP(dir->first, find_data.cFileName);
	return dir;
}

void
dir_close(struct Dir **const a_dir)
{
	struct Dir *dir;

	dir = *a_dir;
	if (NULL == dir) {
		return;
	}
	if (NULL != dir->first) {
		FREE(dir->first);
	}
	FindClose(dir->handle);
	FREE(*a_dir);
}

int
dir_get(struct Dir *const a_dir, struct DirEntry *const a_entry)
{
	WIN32_FIND_DATA find_data;

	if (NULL != a_dir->first) {
		strcpy(a_entry->name, a_dir->first);
		FREE(a_dir->first);
		return 1;
	}
	if (!FindNextFile(a_dir->handle, &find_data)) {
		if (ERROR_NO_MORE_FILES != GetLastError()) {
			return -1;
		}
		return 0;
	}
	strcpy(a_entry->name, find_data.cFileName);
	return 1;
}

#endif
