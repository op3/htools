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
	int	level;\
	int	x, y;\
	struct	AABB3f aabb;\
	struct	NodeType *child[4];\
}

#define QUADTREE_HEAD(TreeType, NodeType, UserType)\
struct TreeType {\
	float	world_size;\
	struct	UserType *user_data;\
	struct	NodeType *root;\
}

#define QUADTREE_PROTOTYPE(name, TreeType, NodeType, UserType)\
static struct NodeType *name##_node_alloc_(struct TreeType *, int, int, int);\
static void name##_node_clear_(struct TreeType *, struct NodeType *);\
static void name##_node_free_(struct TreeType *, struct NodeType **);\
static void name##_node_visit_(struct TreeType *, struct NodeType *);\
static void name##_shutdown(struct TreeType *);\
static void name##_init(struct TreeType *, struct UserType *, float);\
static void name##_traverse(struct TreeType *)

#define QUADTREE_IMPLEMENT(name, TreeType, NodeType, UserType, alloc, free_,\
    visit, entry)\
struct NodeType		*alloc(struct UserType *);\
void			free_(struct UserType *, struct NodeType **);\
enum QuadtreeTraversal	visit(struct UserType *, struct NodeType *);\
static struct NodeType *\
name##_node_alloc_(struct TreeType *a_tree, int a_level, int a_x, int a_y)\
{\
	struct	NodeType *node;\
	float	node_size;\
\
	node = alloc(a_tree->user_data);\
	node->entry.level = a_level;\
	node->entry.x = a_x;\
	node->entry.y = a_y;\
	node_size = 0.5f * (1 << a_level);\
	vector3f_set(&node->entry.aabb.min,\
	    a_tree->world_size * (a_x - node_size),\
	    0.0f,\
	    a_tree->world_size * (a_y - node_size));\
	node_size = a_tree->world_size / (1 << a_level);\
	vector3f_set(&node->entry.aabb.max,\
	    node->entry.aabb.min.x + node_size,\
	    node->entry.aabb.min.y + 2e3f,\
	    node->entry.aabb.min.z + node_size);\
	node->entry.child[0] = NULL;\
	return node;\
}\
static void name##_node_free_(struct TreeType *, struct NodeType **);\
static void \
name##_node_clear_(struct TreeType *a_tree, struct NodeType *a_node)\
{\
	ASSERT(void *, "p", NULL, !=, a_node);\
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
	ASSERT(void *, "p", NULL, ==, *a_node);\
}\
static void \
name##_node_visit_(struct TreeType *a_tree, struct NodeType *a_node)\
{\
	enum QuadtreeTraversal trav;\
	size_t i;\
\
	trav = visit(a_tree->user_data, a_node);\
	switch (trav) {\
	case QUADTREE_CLEAR:\
		name##_node_clear_(a_tree, a_node);\
		return;\
	case QUADTREE_CONTINUE:\
		break;\
	case QUADTREE_STOP:\
		return;\
	}\
	if (NULL == a_node->entry.child[0]) {\
		int	level;\
\
		level = a_node->entry.level + 1;\
		a_node->entry.child[0] = name##_node_alloc_(a_tree, level,\
		    a_node->entry.x, a_node->entry.y);\
		a_node->entry.child[1] = name##_node_alloc_(a_tree, level,\
		    a_node->entry.x + 1, a_node->entry.y);\
		a_node->entry.child[2] = name##_node_alloc_(a_tree, level,\
		    a_node->entry.x, a_node->entry.y + 1);\
		a_node->entry.child[3] = name##_node_alloc_(a_tree, level,\
		    a_node->entry.x + 1, a_node->entry.y + 1);\
	}\
	for (i = 0; LENGTH(a_node->entry.child) > i; ++i) {\
		name##_node_visit_(a_tree, a_node->entry.child[i]);\
	}\
}\
static void \
name##_shutdown(struct TreeType *a_tree)\
{\
	name##_node_free_(a_tree, &a_tree->root);\
}\
static void \
name##_init(struct TreeType *a_tree, struct UserType *a_user_data, float\
    a_world_size)\
{\
	a_tree->world_size = a_world_size;\
	a_tree->user_data = a_user_data;\
	a_tree->root = name##_node_alloc_(a_tree, 0, 0, 0);\
}\
static void \
name##_traverse(struct TreeType *a_tree)\
{\
	ASSERT(void *, "p", NULL, !=, a_tree->root);\
	name##_node_visit_(a_tree, a_tree->root);\
}\
struct TreeType

#endif
