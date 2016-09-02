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

#ifndef HUTILS_QUADTREE_H
#define HUTILS_QUADTREE_H

#include <hutils/aabb3.h>
#include <hutils/assert.h>
#include <hutils/vector2.h>

enum QuadtreeTraversal {
	QUADTREE_CLEAR,
	QUADTREE_CONTINUE,
	QUADTREE_STOP
};

#define QUADTREE_ENTRY(NodeType)\
struct {\
	struct	Vector2f grid;\
	int	level;\
	struct	AABB3f aabb;\
	struct	NodeType *child[4];\
}

#define QUADTREE_HEAD(TreeType, NodeType, UserData)\
struct TreeType {\
	size_t	grid_size;\
	float	world_size;\
	struct	UserData *user_data;\
	struct	NodeType *root;\
}

#define QUADTREE_PROTOTYPE(name, TreeType, NodeType)\
static struct NodeType *name##_node_alloc_(struct TreeType *, struct Vector2f\
    const *, int);\
static void name##_node_clear_(struct TreeType *, struct NodeType *);\
static void name##_node_free_(struct TreeType *, struct NodeType **);\
static void name##_node_visit_(struct TreeType *, struct NodeType *);\
static void name##_free(struct TreeType *);\
static void name##_init(struct TreeType *, float, float);\
static void name##_traverse(struct TreeType *)

#define QUADTREE_IMPLEMENT(name, TreeType, NodeType, UserType, alloc, free_,\
    visitor, entry)\
struct NodeType		*alloc(struct TestData *);\
void			free_(struct TestData *, struct NodeType **);\
enum QuadtreeTraversal	visitor(struct TestData *, struct NodeType *);\
static struct NodeType *\
name##_node_alloc_(struct TreeType *a_tree, struct Vector2f const *a_grid,\
    int a_level)\
{\
	struct	NodeType *node;\
	float	A, B, node_size;\
\
	node = alloc(a_tree->user_data);\
	COPY(node->entry.grid, *a_grid);\
	node->entry.level = a_level;\
	A = a_tree->world_size / a_tree->grid_size;\
	B = -a_tree->world_size / 2;\
	node_size = a_tree->world_size / (1 << a_level);\
	vector3f_set(&node->entry.aabb.min,\
	    A * a_grid->x + B, 0.0f,\
	    A * a_grid->y + B);\
	vector3f_set(&node->entry.aabb.max,\
	    node->entry.aabb.min.x + node_size, 2.0e3f,\
	    node->entry.aabb.min.z + node_size);\
	node->entry.child[0] = NULL;\
	return node;\
}\
static void name##_node_free_(struct TreeType *, struct NodeType **);\
static void \
name##_node_clear_(struct TreeType *a_tree, struct NodeType *a_node)\
{\
	ASSERT(void *, p, NULL, !=, a_node);\
	if (NULL != a_node->entry.child[0]) {\
		size_t i;\
\
		for (i = 0; LENGTH(a_node->entry.child) > i; ++i) {\
			name##_node_free_(a_tree, &a_node->entry.child[i]);\
		}\
	}\
}\
static void \
name##_node_free_(struct TreeType *a_tree, struct NodeType **a_node)\
{\
	name##_node_clear_(a_tree, *a_node);\
	free_(a_tree->user_data, a_node);\
	ASSERT(void *, p, NULL, ==, *a_node);\
}\
static void \
name##_node_visit_(struct TreeType *a_tree, struct NodeType *a_node)\
{\
	enum QuadtreeTraversal trav;\
	size_t i;\
\
	trav = visitor(a_tree->user_data, a_node);\
	switch (trav) {\
	case QUADTREE_CONTINUE:\
		break;\
	case QUADTREE_CLEAR:\
		name##_node_clear_(a_tree, a_node);\
		return;\
	case QUADTREE_STOP:\
		return;\
	}\
	if (NULL == a_node->entry.child[0]) {\
		struct	Vector2f s;\
		float	side2;\
		int	level;\
\
		level = a_node->entry.level + 1;\
		side2 = a_tree->grid_size / (1 << level);\
		vector2f_set(&s,\
		    a_node->entry.grid.x,\
		    a_node->entry.grid.y);\
		a_node->entry.child[0] = name##_node_alloc_(a_tree, &s,\
		    level);\
		vector2f_set(&s,\
		    a_node->entry.grid.x + side2,\
		    a_node->entry.grid.y);\
		a_node->entry.child[1] = name##_node_alloc_(a_tree, &s,\
		    level);\
		vector2f_set(&s,\
		    a_node->entry.grid.x,\
		    a_node->entry.grid.y + side2);\
		a_node->entry.child[2] = name##_node_alloc_(a_tree, &s,\
		    level);\
		vector2f_set(&s,\
		    a_node->entry.grid.x + side2,\
		    a_node->entry.grid.y + side2);\
		a_node->entry.child[3] = name##_node_alloc_(a_tree, &s,\
		    level);\
	}\
	for (i = 0; LENGTH(a_node->entry.child) > i; ++i) {\
		name##_node_visit_(a_tree, a_node->entry.child[i]);\
	}\
}\
static void \
name##_free(struct TreeType *a_tree)\
{\
	name##_node_free_(a_tree, &a_tree->root);\
}\
static void \
name##_init(struct TreeType *a_tree, struct UserType *a_user_data, float\
    a_grid_size, float a_world_size)\
{\
	struct	Vector2f const c_origin = {0.0f, 0.0f};\
\
	a_tree->grid_size = a_grid_size;\
	a_tree->world_size = a_world_size;\
	a_tree->user_data = a_user_data;\
	a_tree->root = name##_node_alloc_(a_tree, &c_origin, 0);\
}\
static void \
name##_traverse(struct TreeType *a_tree)\
{\
	ASSERT(void *, p, NULL, !=, a_tree->root);\
	name##_node_visit_(a_tree, a_tree->root);\
}\
struct TreeType

#endif
