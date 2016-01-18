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

#include <hutils/string.h>
#include <hutils/memory.h>

/*
 * Concatenates an array of strings into a malloc:d string.
 * a_arr: Array of pointers to strings to be concatenated.
 * a_arr_len: Length of array.
 * Returns: Pointed to strings concatenated.
 */
char *
strcxt(char const *const *const a_arr, size_t const a_arr_len)
{
	char *dst, *to;
	size_t i, len;

	len = 0;
	for (i = 0; a_arr_len > i; ++i) {
		len += strlen(a_arr[i]);
	}
	MALLOC(dst, len + 1);
	to = dst;
	for (i = 0; a_arr_len > i; ++i) {
		char const *from;

		for (from = a_arr[i]; '\0' != *from;) {
			*to++ = *from++;
		}
	}
	*to = '\0';
	return dst;
}

/*
 * Compares the end of the first string with the second string.
 * a_s1: First string.
 * a_s2: Second string.
 * Returns: Similar to strcmp.
 */
int
strecmp(char const *const a_s1, char const *const a_s2)
{
	size_t len1, len2;

	len1 = strlen(a_s1);
	len2 = strlen(a_s2);
	if (len1 < len2) {
		return -1;
	}
	return strcmp(a_s1 + len1 - len2, a_s2);
}
