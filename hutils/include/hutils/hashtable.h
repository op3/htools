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

#ifndef HUTILS_HASHTABLE_H
#define HUTILS_HASHTABLE_H

#include <assert.h>
#include <hutils/funcattr.h>
#include <hutils/memory.h>

#define HASHTABLE_HEAD(HashTableType, Type)\
struct HashTableType##Entry {\
	uint32_t	hash;\
	struct	Type *t;\
};\
struct HashTableType {\
	uint32_t	mask;\
	uint32_t	num;\
	struct	HashTableType##Entry *array;\
}

#define HASHTABLE_DIB(a_ht, hash, i) (i - (hash & a_ht->mask)) & a_ht->mask

#define HASHTABLE_IMPLEMENT(HashTableType, Type, Key, key, start_size,\
    hash_, equal, free_, field)\
static size_t \
HashTableType##_find_(struct HashTableType *a_ht, struct Key const *a_key)\
{\
	size_t i, dib;\
	uint32_t hash;\
	i = hash = hash_(a_key);\
	dib = 0;\
	for (;;) {\
		struct HashTableType##Entry *entry;\
		size_t d;\
		i &= a_ht->mask;\
		entry = &a_ht->array[i];\
		if (NULL == entry->t) {\
			return -1;\
		}\
		d = HASHTABLE_DIB(a_ht, entry->hash, i);\
		if (dib > d) {\
			return -1;\
		}\
		if (hash == entry->hash && equal(a_key, &entry->t->key)) {\
			return i;\
		}\
		++i;\
	}\
}\
static struct HashTableType##Entry *\
HashTableType##_insert_(struct HashTableType *a_ht, struct Type *a_t,\
    uint32_t a_hash)\
{\
	struct HashTableType##Entry *ret;\
	struct Type *t;\
	size_t i, dib;\
	uint32_t hash;\
	ret = NULL;\
	t = a_t;\
	i = hash = a_hash;\
	dib = 0;\
	for (;;) {\
		struct HashTableType##Entry *entry;\
		size_t d;\
		i &= a_ht->mask;\
		entry = &a_ht->array[i];\
		if (NULL == entry->t) {\
			entry->hash = hash;\
			entry->t = t;\
			++a_ht->num;\
			return NULL == ret ? entry : ret;\
		}\
		if (hash == entry->hash && equal(&t->key, &entry->t->key)) {\
			assert(NULL == ret);\
			return entry;\
		}\
		d = HASHTABLE_DIB(a_ht, entry->hash, i);\
		if (dib > d) {\
			struct Type *new_t;\
			uint32_t new_hash;\
			ret = NULL == ret ? entry : ret;\
			new_hash = hash;\
			hash = entry->hash;\
			entry->hash = new_hash;\
			new_t = t;\
			t = entry->t;\
			entry->t = new_t;\
			dib = d;\
		}\
		++i;\
		++dib;\
	}\
}\
static void \
HashTableType##_rehash_(struct HashTableType *a_ht, size_t a_size)\
{\
	struct HashTableType##Entry *old_array;\
	size_t i;\
	uint32_t old_mask;\
	assert(IS_POW2(a_size));\
	old_mask = a_ht->mask;\
	old_array = a_ht->array;\
	a_ht->mask = a_size - 1;\
	a_ht->num = 0;\
	CALLOC(a_ht->array, a_size);\
	if (NULL != old_array) {\
		for (i = 0; old_mask >= i; ++i) {\
			struct HashTableType##Entry *entry;\
			entry = &old_array[i];\
			if (NULL != entry->t) {\
				HashTableType##_insert_(a_ht, entry->t,\
				    entry->hash);\
			}\
		}\
		FREE(old_array);\
	}\
}\
FUNC_UNUSED static void \
HashTableType##_erase(struct HashTableType *a_ht, struct HashTableType##Entry\
    *a_entry)\
{\
	struct HashTableType##Entry *cur;\
	size_t i;\
	cur = a_entry;\
	free_(cur->t);\
	cur->t = NULL;\
	i = cur - a_ht->array;\
	for (;;) {\
		struct HashTableType##Entry *next;\
		size_t dib, j;\
		j = (i + 1) & a_ht->mask;\
		next = &a_ht->array[j];\
		if (NULL == next->t) {\
			return;\
		}\
		dib = HASHTABLE_DIB(a_ht, next->hash, j);\
		if (0 == dib) {\
			return;\
		}\
		cur->hash = next->hash;\
		cur->t = next->t;\
		next->t = NULL;\
		i = j;\
		cur = next;\
	}\
}\
static struct HashTableType##Entry *\
HashTableType##_find(struct HashTableType *a_ht, struct Key const *a_key)\
{\
	size_t i;\
	i = HashTableType##_find_(a_ht, a_key);\
	return (size_t)-1 == i ? NULL : &a_ht->array[i];\
}\
static void \
HashTableType##_init(struct HashTableType *a_ht)\
{\
	a_ht->mask = 0;\
	a_ht->num = 0;\
	a_ht->array = NULL;\
}\
static struct HashTableType##Entry *\
HashTableType##_insert(struct HashTableType *a_ht, struct Type *a_t)\
{\
	if (a_ht->num >= (9 * a_ht->mask) / 10) {\
		uint32_t goal, size;\
		goal = MAX(start_size, (a_ht->mask + 1));\
		for (size = 1; goal >= size; size <<= 1)\
			;\
		HashTableType##_rehash_(a_ht, size);\
	}\
	return HashTableType##_insert_(a_ht, a_t, hash_(&a_t->key));\
}\
static void \
HashTableType##_shutdown(struct HashTableType *a_ht)\
{\
	size_t i;\
	if (NULL == a_ht->array) {\
		return;\
	}\
	for (i = 0; a_ht->mask >= i; ++i) {\
		free_(a_ht->array[i].t);\
	}\
	FREE(a_ht->array);\
	a_ht->mask = 0;\
	a_ht->num = 0;\
}\
struct HashTableType

#endif
