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

#define VECTOR_ASSERT_BOUNDS_(add) do {\
	if (a_vec->size + add <= a_i) {\
		fprintf(stderr, __FILE__":%d: Vector index %"PRIz" out of "\
		    "bounds %"PRIz"+"#add".\n", __LINE__, a_i, a_vec->size);\
		abort();\
	}\
} WHILE_0
#define VECTOR_ASSERT_INVARIANT_(chunk) do {\
	ASSERT(size_t, PRIz, a_vec->ofs + a_vec->size, <=, a_vec->capacity);\
	ASSERT(size_t, PRIz, chunk, >=, a_vec->ofs);\
	ASSERT(size_t, PRIz, a_vec->capacity, <=, a_vec->ofs + a_vec->size +\
	    chunk);\
} WHILE_0
#define VECTOR_ASSERT_NOTEMPTY_ do {\
	if (0 == a_vec->size) {\
		fprintf(stderr, __FILE__":%d: Vector empty.\n", __LINE__);\
		abort();\
	}\
} WHILE_0
#define MEMMOVE(dst, src, num) do {\
	ASSERT(size_t, PRIz, a_vec->capacity, >=, num);\
	ASSERT(size_t, PRIz, 0, <=, dst);\
	ASSERT(size_t, PRIz, a_vec->capacity, >=, dst + num);\
	ASSERT(size_t, PRIz, 0, <=, src);\
	ASSERT(size_t, PRIz, a_vec->capacity, >=, src + num);\
	memmove(a_vec->data + dst, a_vec->data + src, (num) * sizeof\
	    *a_vec->data);\
} WHILE_0

#define VECTOR_HEAD(VectorType, Type) struct VectorType {\
	size_t	ofs;\
	size_t	size;\
	size_t	capacity;\
	struct	Type *data;\
}
#define VECTOR_FOREACH(var, vec) for (var = &(vec)->data[(vec)->ofs];\
    &(vec)->data[(vec)->ofs + (vec)->size] > var; ++var)

#define VECTOR_PROTOTYPE(link_, name, VectorType, Type)\
link_ void				name##_reserve_(struct VectorType *,\
    size_t);\
link_ void				name##_try_grow_(struct VectorType\
    *);\
link_ void				name##_try_shrink_(struct VectorType\
    *);\
FUNC_UNUSED link_ struct Type		*name##_at(struct VectorType *,\
    size_t);\
FUNC_UNUSED link_ struct Type const	*name##_atc(struct VectorType const\
    *, size_t);\
FUNC_UNUSED link_ struct Type		*name##_back(struct VectorType *);\
FUNC_UNUSED link_ void			name##_init(struct VectorType *,\
    size_t);\
FUNC_UNUSED link_ void			name##_erase(struct VectorType *,\
    size_t);\
FUNC_UNUSED link_ void			name##_clean(struct VectorType *);\
FUNC_UNUSED link_ struct Type		*name##_front(struct VectorType *);\
FUNC_UNUSED link_ struct Type		*name##_insert(struct VectorType *,\
    size_t);\
FUNC_UNUSED link_ void			name##_pop_back(struct VectorType *);\
FUNC_UNUSED link_ void			name##_pop_front(struct VectorType\
    *);\
FUNC_UNUSED link_ struct Type		*name##_push_back(struct VectorType\
    *);\
FUNC_UNUSED link_ struct Type		*name##_push_front(struct VectorType\
    *);\
FUNC_UNUSED link_ void			name##_reserve(struct VectorType *,\
    size_t);\
FUNC_UNUSED link_ void			name##_resize(struct VectorType *,\
    size_t)

/* TODO: Optimize rules for front and back margins, add hysteresis? */

#define VECTOR_IMPLEMENT(link_, name, VectorType, Type, chunk)\
link_ void \
name##_reserve_(struct VectorType *a_vec, size_t a_capacity)\
{\
	REALLOCARRAY(a_vec->data, a_capacity);\
	if (a_vec->ofs + a_vec->size > a_capacity) {\
		a_vec->size = a_capacity - a_vec->ofs;\
	}\
	a_vec->capacity = a_capacity;\
	VECTOR_ASSERT_INVARIANT_(chunk);\
}\
link_ void \
name##_try_grow_(struct VectorType *a_vec)\
{\
	if (a_vec->ofs + a_vec->size >= a_vec->capacity) {\
		name##_reserve_(a_vec, a_vec->ofs + a_vec->size + chunk);\
	}\
}\
link_ void \
name##_try_shrink_(struct VectorType *a_vec)\
{\
	if (a_vec->ofs + a_vec->size + chunk < a_vec->capacity) {\
		name##_reserve_(a_vec, a_vec->ofs + a_vec->size + chunk);\
	}\
}\
FUNC_UNUSED link_ struct Type *\
name##_at(struct VectorType *a_vec, size_t a_i)\
{\
	VECTOR_ASSERT_BOUNDS_(0);\
	return &a_vec->data[a_vec->ofs + a_i];\
}\
FUNC_UNUSED link_ struct Type const *\
name##_atc(struct VectorType const *a_vec, size_t a_i)\
{\
	VECTOR_ASSERT_BOUNDS_(0);\
	return &a_vec->data[a_vec->ofs + a_i];\
}\
FUNC_UNUSED link_ struct Type *\
name##_back(struct VectorType *a_vec)\
{\
	VECTOR_ASSERT_NOTEMPTY_;\
	return &a_vec->data[a_vec->ofs + a_vec->size - 1];\
}\
FUNC_UNUSED link_ void \
name##_init(struct VectorType *a_vec, size_t a_size)\
{\
	a_vec->ofs = 0;\
	a_vec->size = a_size;\
	a_vec->capacity = a_size;\
	if (0 == a_size) {\
		a_vec->data = NULL;\
	} else {\
		CALLOC(a_vec->data, a_size);\
	}\
	VECTOR_ASSERT_INVARIANT_(chunk);\
}\
FUNC_UNUSED link_ void \
name##_erase(struct VectorType *a_vec, size_t a_i)\
{\
	VECTOR_ASSERT_BOUNDS_(0);\
	--a_vec->size;\
	if (a_i < (a_vec->size >> 1)) {\
		++a_vec->ofs;\
		if (chunk <= a_vec->ofs) {\
			MEMMOVE(0, a_vec->ofs - 1, a_i);\
			MEMMOVE(a_i, a_vec->ofs + a_i, a_vec->size - a_i);\
			a_vec->ofs = 0;\
			name##_try_shrink_(a_vec);\
		} else {\
			MEMMOVE(a_vec->ofs, a_vec->ofs - 1, a_i);\
			VECTOR_ASSERT_INVARIANT_(chunk);\
		}\
	} else {\
		MEMMOVE(a_vec->ofs + a_i, a_vec->ofs + a_i + 1, a_vec->size -\
		    a_i);\
		name##_try_shrink_(a_vec);\
	}\
}\
FUNC_UNUSED link_ void \
name##_clean(struct VectorType *a_vec)\
{\
	a_vec->ofs = 0;\
	a_vec->size = 0;\
	a_vec->capacity = 0;\
	FREE(a_vec->data);\
}\
FUNC_UNUSED link_ struct Type *\
name##_front(struct VectorType *a_vec)\
{\
	VECTOR_ASSERT_NOTEMPTY_;\
	return &a_vec->data[a_vec->ofs];\
}\
FUNC_UNUSED link_ struct Type *\
name##_insert(struct VectorType *a_vec, size_t a_i)\
{\
	VECTOR_ASSERT_BOUNDS_(1);\
	++a_vec->size;\
	if (a_i < (a_vec->size >> 1)) {\
		if (0 < a_vec->ofs) {\
			--a_vec->ofs;\
			MEMMOVE(a_vec->ofs, a_vec->ofs + 1, a_i);\
		} else {\
			a_vec->ofs = chunk - 1;\
			name##_try_grow_(a_vec);\
			MEMMOVE(a_vec->ofs + a_i + 1, a_i, a_vec->size -\
			    a_i);\
			MEMMOVE(a_vec->ofs, 0, a_i);\
		}\
	} else {\
		name##_try_grow_(a_vec);\
		MEMMOVE(a_vec->ofs + a_i + 1, a_vec->ofs + a_i, a_vec->size -\
		    a_i);\
	}\
	VECTOR_ASSERT_INVARIANT_(chunk);\
	return &a_vec->data[a_vec->ofs + a_i];\
}\
FUNC_UNUSED link_ void \
name##_pop_back(struct VectorType *a_vec)\
{\
	VECTOR_ASSERT_NOTEMPTY_;\
	--a_vec->size;\
	name##_try_shrink_(a_vec);\
}\
FUNC_UNUSED link_ void \
name##_pop_front(struct VectorType *a_vec)\
{\
	VECTOR_ASSERT_NOTEMPTY_;\
	--a_vec->size;\
	MEMMOVE(0, a_vec->ofs + 1, a_vec->size);\
	name##_try_shrink_(a_vec);\
}\
FUNC_UNUSED link_ struct Type *\
name##_push_back(struct VectorType *a_vec)\
{\
	++a_vec->size;\
	name##_try_grow_(a_vec);\
	return &a_vec->data[a_vec->ofs + a_vec->size - 1];\
}\
FUNC_UNUSED link_ struct Type *\
name##_push_front(struct VectorType *a_vec)\
{\
	++a_vec->size;\
	if (0 < a_vec->ofs) {\
		--a_vec->ofs;\
	} else {\
		a_vec->ofs = chunk - 1;\
		name##_try_grow_(a_vec);\
		MEMMOVE(a_vec->ofs + 1, 0, a_vec->size - 1);\
	}\
	VECTOR_ASSERT_INVARIANT_(chunk);\
	return &a_vec->data[a_vec->ofs];\
}\
FUNC_UNUSED link_ void \
name##_reserve(struct VectorType *a_vec, size_t a_capacity)\
{\
	name##_reserve_(a_vec, a_capacity);\
}\
FUNC_UNUSED link_ void \
name##_resize(struct VectorType *a_vec, size_t a_size)\
{\
	name##_reserve_(a_vec, a_vec->ofs + a_size);\
}\
struct VectorType

#endif
