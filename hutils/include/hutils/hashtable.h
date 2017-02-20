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

#define HASHTABLE_COLLISION_LENGTH 3

#include <hutils/memory.h>
#include <hutils/queue.h>

#define HASHTABLE_ENTRY(Type) TAILQ_ENTRY(Type)
#define HASHTABLE_HEAD(HashTableType, Type)\
TAILQ_HEAD(HashTableType##Chain_, Type);\
struct HashTableType {\
	size_t	size;\
	struct	HashTableType##Chain_ *table;\
}

#define HASHTABLE_IMPLEMENT(HashTableType, Type, Key, key, start_size,\
    hash, equal, free_, field)\
static struct HashTableType##Chain_ *\
HashTableType##_get_chain_(struct HashTableType *a_ht, struct Key const\
    *a_key)\
{\
	return &a_ht->table[hash(a_key) % a_ht->size];\
}\
static struct Type *\
HashTableType##_get_node_(struct HashTableType##Chain_ *a_chain, struct Key\
    const *a_key, size_t *a_len)\
{\
	struct Type *node;\
	*a_len = 0;\
	TAILQ_FOREACH(node, a_chain, field) {\
		if (equal(a_key, &node->key)) {\
			return node;\
		}\
		++*a_len;\
	}\
	return NULL;\
}\
static void \
HashTableType##_rehash_(struct HashTableType *a_ht, size_t a_size)\
{\
	struct HashTableType##Chain_ *old_table;\
	size_t old_size, i;\
	old_table = a_ht->table;\
	old_size = a_ht->size;\
	CALLOC(a_ht->table, a_size);\
	a_ht->size = a_size;\
	for (i = 0; a_size > i; ++i) {\
		TAILQ_INIT(&a_ht->table[i]);\
	}\
	for (i = 0; old_size > i; ++i) {\
		struct Type *cur, *next;\
		TAILQ_FOREACH_SAFE(cur, &old_table[i], field, next) {\
			struct HashTableType##Chain_ *chain;\
			chain = HashTableType##_get_chain_(a_ht, &cur->key);\
			TAILQ_INSERT_TAIL(chain, cur, field);\
		}\
	}\
	FREE(old_table);\
}\
static struct Type *\
HashTableType##_find(struct HashTableType *a_ht, struct Key const *a_key)\
{\
	struct HashTableType##Chain_ *chain;\
	size_t dummy;\
	chain = HashTableType##_get_chain_(a_ht, a_key);\
	return HashTableType##_get_node_(chain, a_key, &dummy);\
}\
static void \
HashTableType##_init(struct HashTableType *a_ht)\
{\
	a_ht->size = 0;\
	a_ht->table = NULL;\
}\
static struct Type *\
HashTableType##_insert(struct HashTableType *a_ht, struct Type *a_t)\
{\
	struct HashTableType##Chain_ *chain;\
	struct Type *node;\
	size_t len;\
	if (0 == a_ht->size) {\
		HashTableType##_rehash_(a_ht, start_size);\
	}\
	chain = HashTableType##_get_chain_(a_ht, &a_t->key);\
	node = HashTableType##_get_node_(chain, &a_t->key, &len);\
	if (NULL != node) {\
		return node;\
	}\
	if (HASHTABLE_COLLISION_LENGTH <= len) {\
		HashTableType##_rehash_(a_ht, a_ht->size * 2);\
		chain = HashTableType##_get_chain_(a_ht, &a_t->key);\
	}\
	TAILQ_INSERT_TAIL(chain, a_t, field);\
	return a_t;\
}\
static void \
HashTableType##_shutdown(struct HashTableType *a_ht)\
{\
	size_t i;\
	if (0 == a_ht->size) {\
		return;\
	}\
	for (i = 0; a_ht->size > i; ++i) {\
		struct Type *cur, *next;\
		TAILQ_FOREACH_SAFE(cur, &a_ht->table[i], field, next) {\
			free_(cur);\
		}\
	}\
	FREE(a_ht->table);\
	a_ht->size = 0;\
}\
struct HashTableType

#endif
