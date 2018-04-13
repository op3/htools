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

#include <hutils/hashtable.h>
#include <htest/htest.h>
#include <hutils/fmtmod.h>
#include <hutils/hash.h>
#include <hutils/string.h>

#define N 1000

struct KeyInt {
	int	i;
};
struct Int {
	struct	KeyInt key;
};
HASHTABLE_HEAD(HTInt, Int);

struct KeyString {
	char	str[32];
};
struct String {
	struct	KeyString key;
};
HASHTABLE_HEAD(HTString, String);

static int	equal_int(struct KeyInt const *, struct KeyInt const *)
	FUNC_RETURNS;
static int	equal_string(struct KeyString const *, struct KeyString const
    *) FUNC_RETURNS;
static uint32_t	hash_int(struct KeyInt const *) FUNC_RETURNS;
static uint32_t	hash_string(struct KeyString const *) FUNC_RETURNS;

HASHTABLE_IMPLEMENT(HTInt, Int, KeyInt, key, 10, hash_int, equal_int,
    free);
HASHTABLE_IMPLEMENT(HTString, String, KeyString, key, 10, hash_string,
    equal_string, free);

int
equal_int(struct KeyInt const *a_i0, struct KeyInt const *a_i1)
{
	return a_i0->i == a_i1->i;
}

int
equal_string(struct KeyString const *a_s0, struct KeyString const *a_s1)
{
	return 0 == strcmp(a_s0->str, a_s1->str);
}

uint32_t
hash_int(struct KeyInt const *a_key)
{
	return a_key->i;
}

uint32_t
hash_string(struct KeyString const *a_key)
{
	return hutils_fnv1a32(a_key->str, strlen(a_key->str));
}

HTEST(Int)
{
	struct HTInt table;
	size_t i;

	HTInt_init(&table);
	for (i = 0; N > i; ++i) {
		struct HTIntEntry *inserted;
		struct Int *j;

		CALLOC(j, 1);
		j->key.i = i;
		inserted = HTInt_insert(&table, j);
		HTRY_PTR(inserted->t, ==, j);
	}
	/* Find all keys. */
	for (i = 0; N > i; ++i) {
		struct KeyInt key;
		struct HTIntEntry *j;

		key.i = i;
		j = HTInt_find(&table, &key);
		HTRY_PTR(NULL, !=, j);
		HTRY_I(key.i, ==, j->t->key.i);
	}
	/* Make sure we don't put in copies. */
	for (i = 0; N > i; ++i) {
		struct Int j;
		struct HTIntEntry *inserted;

		j.key.i = i;
		inserted = HTInt_insert(&table, &j);
		HTRY_PTR(inserted->t, !=, &j);
	}
	/* Remove a bunch of items and find the rest. */
	for (i = 0; N > i; i += 3) {
		struct KeyInt key;
		struct HTIntEntry *j;

		key.i = i;
		j = HTInt_find(&table, &key);
		HTInt_erase(&table, j);
	}
	for (i = 0; N > i; ++i) {
		struct KeyInt key;
		struct HTIntEntry *j;

		if (0 == i % 3) {
			continue;
		}
		key.i = i;
		j = HTInt_find(&table, &key);
		HTRY_PTR(NULL, !=, j);
	}
	HTInt_shutdown(&table);
}

HTEST(Strings)
{
	struct HTString table;
	size_t i;

#define MAKE_STR(str) do {\
		int ret;\
		ret = snprintf(str, sizeof str, "abc_%"PRIz, i);\
		HTRY_I(sizeof str, >, ret);\
	} WHILE_0

	HTString_init(&table);
	for (i = 0; N > i; ++i) {
		struct HTStringEntry *inserted;
		struct String *s;

		CALLOC(s, 1);
		MAKE_STR(s->key.str);
		inserted = HTString_insert(&table, s);
		HTRY_PTR(inserted->t, ==, s);
	}
	for (i = 0; N > i; ++i) {
		struct HTStringEntry *find;
		struct KeyString key;

		MAKE_STR(key.str);
		find = HTString_find(&table, &key);
		HTRY_PTR(NULL, !=, find);
		HTRY_STR(find->t->key.str, ==, key.str);
	}
	for (i = 0; N > i; ++i) {
		struct HTStringEntry *inserted;
		struct String s;

		MAKE_STR(s.key.str);
		inserted = HTString_insert(&table, &s);
		HTRY_PTR(inserted->t, !=, &s);
	}
	HTString_shutdown(&table);
}

HTEST_SUITE(HashTable)
{
	HTEST_ADD(Int);
	HTEST_ADD(Strings);
}
