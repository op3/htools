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

#define N 1000

struct KeyInt {
	int	i;
};
struct Int {
	struct	KeyInt key;
	HASHTABLE_ENTRY(Int)	ht;
};
HASHTABLE_HEAD(HashTableInt, Int);

struct KeyString {
	char	str[10];
};
struct String {
	struct	KeyString key;
	HASHTABLE_ENTRY(String)	ht;
};
HASHTABLE_HEAD(HashTableString, String);

static int	equal_int(struct KeyInt const *, struct KeyInt const *)
	FUNC_RETURNS;
static int	equal_string(struct KeyString const *, struct KeyString const
    *) FUNC_RETURNS;
static uint32_t	hash_int(struct KeyInt const *) FUNC_RETURNS;
static uint32_t	hash_string(struct KeyString const *) FUNC_RETURNS;

HASHTABLE_IMPLEMENT(HashTableInt, Int, KeyInt, key, 10, hash_int, equal_int,
    free, ht);
HASHTABLE_IMPLEMENT(HashTableString, String, KeyString, key, 10, hash_string,
    equal_string, free, ht);

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
	struct HashTableInt table;
	size_t i;

	HashTableInt_init(&table);
	for (i = 0; N > i; ++i) {
		struct Int *j, *inserted;

		CALLOC(j, 1);
		j->key.i = i;
		inserted = HashTableInt_insert(&table, j);
		HTRY_PTR(inserted, ==, j);
	}
	for (i = 0; N > i; ++i) {
		struct KeyInt key;

		key.i = i;
		HTRY_PTR(NULL, !=, HashTableInt_find(&table, &key));
	}
	for (i = 0; N > i; ++i) {
		struct Int j, *inserted;

		j.key.i = i;
		inserted = HashTableInt_insert(&table, &j);
		HTRY_PTR(inserted, !=, &j);
	}
	HashTableInt_shutdown(&table);
}

HTEST(Strings)
{
	struct HashTableString table;
	size_t i;

	HashTableString_init(&table);
	for (i = 0; N > i; ++i) {
		struct String *s, *inserted;

		CALLOC(s, 1);
		sprintf(s->key.str, "abc_%"PRIz, i);
		inserted = HashTableString_insert(&table, s);
		HTRY_PTR(inserted, ==, s);
	}
	for (i = 0; N > i; ++i) {
		struct KeyString key;

		sprintf(key.str, "abc_%"PRIz, i);
		HTRY_PTR(NULL, !=, HashTableString_find(&table, &key));
	}
	for (i = 0; N > i; ++i) {
		struct String s, *inserted;

		sprintf(s.key.str, "abc_%"PRIz, i);
		inserted = HashTableString_insert(&table, &s);
		HTRY_PTR(inserted, !=, &s);
	}
	HashTableString_shutdown(&table);
}

HTEST_SUITE(HashTableInt)
{
	HTEST_ADD(Int);
	HTEST_ADD(Strings);
}
