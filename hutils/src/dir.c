/*
 * Copyright (c) 2015-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/dir.h>
#include <hutils/macros.h>
#include <hutils/memory.h>
#include <hutils/string.h>

static void	getnames(char const *, char **, char **, char const *);

#if defined(_MSC_VER)
#	include <windows.h>
#	include <hutils/string.h>

struct Dir {
	HANDLE	handle;
	int	is_first;
	char	*name;
};

struct Dir *
dir_open(char const *a_path)
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
	dir->is_first = 1;
	dir->name = strdup(find_data.cFileName);
	return dir;
}

void
dir_close(struct Dir **a_dir)
{
	struct Dir *dir;

	dir = *a_dir;
	if (NULL == dir) {
		return;
	}
	if (NULL != dir->name) {
		FREE(dir->name);
	}
	FindClose(dir->handle);
	FREE(*a_dir);
}

int
dir_get(struct Dir *a_dir, struct DirEntry *a_entry)
{
	WIN32_FIND_DATA find_data;

	if (a_dir->is_first) {
		a_dir->is_first = 0;
		a_entry->name = a_dir->name;
		return 1;
	}
	FREE(a_dir->name);
	if (!FindNextFile(a_dir->handle, &find_data)) {
		if (ERROR_NO_MORE_FILES != GetLastError()) {
			return -1;
		}
		return 0;
	}
	a_dir->name = strdup(find_data.cFileName);
	a_entry->name = a_dir->name;
	return 1;
}

void
dir_getnames(char const *a_path, char **a_dir, char **a_base)
{
	getnames(a_path, a_dir, a_base, "/\\");
}

#else

#	include <dirent.h>
#	include <stddef.h>
#	include <unistd.h>

struct Dir {
	DIR	*dir;
	struct	dirent *entry;
	/*
	 * Some platforms automatically rewind after all entries have been
	 * touched, let's not do that.
	 */
	int	is_done;
};

struct Dir *
dir_open(char const *a_path)
{
	DIR *d;
	struct Dir *dir;
	long name_max;

	d = opendir(a_path);
	if (NULL == d) {
		return NULL;
	}
	CALLOC(dir, 1);
	dir->dir = d;
	name_max = pathconf(a_path, _PC_NAME_MAX);
	if (-1 == name_max) {
		name_max = 255;
	}
	dir->entry = malloc(offsetof(struct dirent, d_name) + name_max + 1);
	return dir;
}

void
dir_close(struct Dir **a_dir)
{
	struct Dir *dir;

	dir = *a_dir;
	if (NULL == dir) {
		return;
	}
	FREE(dir->entry);
	closedir(dir->dir);
	FREE(*a_dir);
}

int
dir_get(struct Dir *a_dir, struct DirEntry *a_entry)
{
	struct dirent *result;

	if (a_dir->is_done) {
		return 0;
	}
	HUTILS_READDIR(a_dir->dir, *a_dir->entry, result);
	if (NULL == result) {
		a_dir->is_done = 1;
		a_entry->name = NULL;
		return 0;
	}
	a_entry->name = a_dir->entry->d_name;
	return 1;
}

void
dir_getnames(char const *a_path, char **a_dir, char **a_base)
{
	getnames(a_path, a_dir, a_base, "/");
}

#endif

void
getnames(char const *a_path, char **a_dir, char **a_base, char const *a_delim)
{
	char const *p, *slash;

	slash = NULL;
	for (p = a_path; '\0' != *p; ++p) {
		char const *d;

		for (d = a_delim; '\0' != *d; ++d) {
			if (*d == *p) {
				slash = p;
			}
		}
	}
	if (NULL != a_dir) {
		if (NULL == slash) {
			*a_dir = strdup(".");
		} else {
			*a_dir = strndup(a_path, slash - a_path);
		}
	}
	if (NULL != a_base) {
		*a_base = strdup(NULL == slash ? a_path : slash + 1);
	}
}

char *
dir_substext(char const *a_path, char const *a_ext)
{
	char const *p, *dot, *slash;
	char *s;
	size_t siz1, siz2;

	dot = NULL;
	slash = NULL;
	for (p = a_path; '\0' != *p; ++p) {
		if ('/' == *p) {
			slash = p;
		} else if ('.' == *p) {
			dot = p;
		}
	}
	if (NULL == dot || slash > dot) {
		return strdup(a_path);
	}
	siz1 = dot - a_path;
	siz2 = strlen(a_ext);
	s = malloc(siz1 + 1 + siz2 + 1);
	memcpy(s, a_path, siz1);
	s[siz1] = '.';
	strcpy(s + siz1 + 1, a_ext);
	return s;
}
