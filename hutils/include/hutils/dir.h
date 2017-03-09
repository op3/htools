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
#	if defined(HCONFING_mDIR)
#		define READDIR_R readdir_r(dir, &entry, &p)
#	endif
#elif defined(HCONF_mDIR_bDIRENT_ANCIENT)
#	include <sys/types.h>
#	include <dirent.h>
#	if defined(HCONFING_mDIR)
#		define READDIR_R (void)p; readdir_r(dir, &entry)
#	endif
#endif
#if defined(HCONFING_mDIR)
HCONF_TEST
{
	DIR *dir;
	struct dirent entry, *p;
	(void)i;
	dir = opendir(".");
	READDIR_R;
	closedir(dir);
	return 1;
}
#endif

CDECLS_BEGIN

struct Dir;
struct DirEntry {
	char	const *name;
};

struct Dir	*dir_open(char const *) FUNC_RETURNS;
void		dir_close(struct Dir **);
int		dir_get(struct Dir *, struct DirEntry *) FUNC_RETURNS;

CDECLS_END

#endif
