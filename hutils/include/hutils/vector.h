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

#ifndef HUTILS_VECTOR_H
#define HUTILS_VECTOR_H

#include <hutils/assert.h>
#include <hutils/memory.h>

#ifndef NDEBUG
#	define VECTOR_INVARIANT_\
    ASSERT(size_t, PRIz, a_vec->ofs + a_vec->size, <=, a_vec->capacity)
#endif

#define VECTOR_HEAD(VectorType, Type) struct VectorType {\
	size_t	ofs;\
	size_t	size;\
	size_t	capacity;\
	struct	Type *data;\
}

#define VECTOR_IMPLEMENT(VectorType, Type, chunk)\
static void \
VectorType##_reserve_(struct VectorType *a_vec, size_t a_capacity)\
{\
	REALLOCARRAY(a_vec->data, a_capacity);\
	if (a_vec->ofs + a_vec->size > a_capacity) {\
		a_vec->size = a_capacity - a_vec->ofs;\
	}\
	a_vec->capacity = a_capacity;\
	VECTOR_INVARIANT_;\
}\
static void \
VectorType##_try_grow_(struct VectorType *a_vec)\
{\
	if (a_vec->ofs + a_vec->size >= a_vec->capacity) {\
		VectorType##_reserve_(a_vec, a_vec->ofs + a_vec->size +\
		    chunk);\
	}\
}\
static void \
VectorType##_try_shrink_(struct VectorType *a_vec)\
{\
	if (a_vec->ofs + a_vec->size + chunk < a_vec->capacity) {\
		VectorType##_reserve_(a_vec, a_vec->ofs + a_vec->size +\
		    chunk);\
	}\
}\
FUNC_UNUSED static struct Type *\
VectorType##_at(struct VectorType *a_vec, size_t a_i)\
{\
	VECTOR_INVARIANT_;\
	if (a_vec->size <= a_i) {\
		fprintf(stderr, "Vector index %"PRIz" out of bounds "\
		    "(%"PRIz"+%"PRIz"<=%"PRIz").\n", a_i, a_vec->ofs,\
		    a_vec->size, a_vec->capacity);\
		abort();\
	}\
	return &a_vec->data[a_vec->ofs + a_i];\
}\
FUNC_UNUSED static struct Type *\
VectorType##_back(struct VectorType *a_vec)\
{\
	VECTOR_INVARIANT_;\
	return &a_vec->data[a_vec->ofs + a_vec->size - 1];\
}\
FUNC_UNUSED static void \
VectorType##_create(struct VectorType *a_vec, size_t a_size)\
{\
	a_vec->ofs = 0;\
	a_vec->size = a_size;\
	a_vec->capacity = a_size;\
	if (0 == a_size) {\
		a_vec->data = NULL;\
	} else {\
		CALLOC(a_vec->data, a_size);\
	}\
	VECTOR_INVARIANT_;\
}\
FUNC_UNUSED static void \
VectorType##_free(struct VectorType *a_vec)\
{\
	a_vec->ofs = 0;\
	a_vec->size = 0;\
	a_vec->capacity = 0;\
	FREE(a_vec->data);\
}\
FUNC_UNUSED static struct Type *\
VectorType##_front(struct VectorType *a_vec)\
{\
	VECTOR_INVARIANT_;\
	return &a_vec->data[a_vec->ofs];\
}\
FUNC_UNUSED static void \
VectorType##_pop_back(struct VectorType *a_vec)\
{\
	VECTOR_INVARIANT_;\
	--a_vec->size;\
	VectorType##_try_shrink_(a_vec);\
}\
FUNC_UNUSED static void \
VectorType##_pop_front(struct VectorType *a_vec)\
{\
	VECTOR_INVARIANT_;\
	--a_vec->size;\
	memmove(a_vec->data, a_vec->data + 1, a_vec->size * sizeof\
	    *a_vec->data);\
	VectorType##_try_shrink_(a_vec);\
}\
FUNC_UNUSED static struct Type *\
VectorType##_push_back(struct VectorType *a_vec)\
{\
	VECTOR_INVARIANT_;\
	++a_vec->size;\
	VectorType##_try_grow_(a_vec);\
	VECTOR_INVARIANT_;\
	return &a_vec->data[a_vec->ofs + a_vec->size - 1];\
}\
FUNC_UNUSED static struct Type *\
VectorType##_push_front(struct VectorType *a_vec)\
{\
	VECTOR_INVARIANT_;\
	if (0 < a_vec->ofs) {\
		--a_vec->ofs;\
		++a_vec->size;\
	} else {\
		a_vec->ofs = chunk - 1;\
		++a_vec->size;\
		VectorType##_try_grow_(a_vec);\
		memmove(a_vec->data + a_vec->ofs + 1, a_vec->data,\
		    (a_vec->size - 1) * sizeof *a_vec->data);\
	}\
	VECTOR_INVARIANT_;\
	return &a_vec->data[a_vec->ofs];\
}\
FUNC_UNUSED static void \
VectorType##_reserve(struct VectorType *a_vec, size_t a_capacity)\
{\
	VECTOR_INVARIANT_;\
	VectorType##_reserve_(a_vec, a_capacity);\
}\
struct VectorType

#endif
