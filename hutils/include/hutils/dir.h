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

#ifndef HUTILS_DIR_H
#define HUTILS_DIR_H

#include <hconf/include/hutils/dir.h>
#include <hutils/funcattr.h>

#if defined(HCONF_mDIR_bDIRENT)
/* HCONF_CPPFLAGS=-D_POSIX_C_SOURCE=199506 */
#	include <dirent.h>
#	if defined(HCONFING_mDIR_bDIRENT)
HCONF_TEST(int, (void))
{
	void *p = 0;
	return readdir_r(p, p, p);
}
#	endif
#elif defined(HCONF_mDIR_bDIRENT_POSIX_DRAFT9)
#	include <dirent.h>
#	if defined(HCONFING_mDIR_bDIRENT_POSIX_DRAFT9)
HCONF_TEST(struct dirent *, (void))
{
	return readdir_r(NULL, NULL);
}
#	endif
#elif defined(HCONF_mDIR_bWINDOWS)
#	include <windows.h>
#	if defined(HCONFING_mDIR_bWINDOWS)
HCONF_TEST(HANDLE, (char const *const a_path))
{
	return FindFirstFile(a_path, NULL);
}
#	endif
#endif

struct Dir;
struct DirEntry {
	char	const *name;
};

struct Dir	*dir_open(char const *) FUNC_RETURNS;
void		dir_close(struct Dir **);
int		dir_get(struct Dir *, struct DirEntry *) FUNC_RETURNS;

#endif
