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

#ifndef HUTILS_DIR_H
#define HUTILS_DIR_H

#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hconf/include/hutils/dir.h>

#if defined(HCONF_mDIR_bDIRENT)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=199506 */
#	include <dirent.h>
#	define HUTILS_READDIR(dir, entry, result) do {\
		if (0 != readdir_r(dir, &entry, &result)) {\
			result = NULL;\
		}\
	} WHILE_0
#elif defined(HCONF_mDIR_bDIRENT_ANCIENT)
#	include <sys/types.h>
#	include <dirent.h>
#	define HUTILS_READDIR(dir, entry, result)\
    result = readdir_r(dir, &entry)
#elif defined(HCONF_mDIR_bDIRENT_READDIR)
/* readdir_r deprecated and readdir made safe, on modern platforms. */
#	include <dirent.h>
#	define HUTILS_READDIR(dir, entry, result) do {\
		(void)entry;\
		result = readdir(dir);\
	} WHILE_0
#endif
#if defined(HCONFING_mDIR)
#	include <stdlib.h>
#	include <hutils/macros.h>
HCONF_TEST
{
	DIR *dir;
	struct dirent entry, *result;
	(void)i;
	dir = opendir(".");
	HUTILS_READDIR(dir, entry, result);
	closedir(dir);
	return NULL != result;
}
#endif

struct Dir;
struct DirEntry {
	char	const *name;
};

CDECLS_BEGIN

struct Dir	*dir_open(char const *) FUNC_NONNULL(()) FUNC_RETURNS;
void		dir_close(struct Dir **) FUNC_NONNULL(());
int		dir_get(struct Dir *, struct DirEntry *) FUNC_NONNULL(())
	FUNC_RETURNS;

CDECLS_END

#endif
