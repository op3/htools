/*
 * Copyright (c) 2016-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/quadtree.h>
#include <htest/htest.h>
#include <hutils/memory.h>

struct TestNode {
	QUADTREE_ENTRY(TestNode)	tree;
};
struct TestData {
	int	creation_level;
	int	node_num;
};
QUADTREE_HEAD(TestTree, TestNode, TestData);

static struct TestNode		*alloc(struct TestData *) FUNC_RETURNS;
static void			free_(struct TestData *, struct TestNode **);
static enum QuadtreeTraversal	visit(struct TestData *, struct TestNode *)
	FUNC_RETURNS;
QUADTREE_PROTOTYPE(testtree, TestTree, TestNode, TestData);

QUADTREE_IMPLEMENT(testtree, TestTree, TestNode, TestData, alloc, free_,
    visit, tree);

struct TestNode *
alloc(struct TestData *a_user)
{
	struct TestNode *node;

	CALLOC(node, 1);
	++a_user->node_num;
	return node;
}

void
free_(struct TestData *a_user, struct TestNode **a_node)
{
	struct TestNode *node;

	(void)node;
	node = *a_node;
	ASSERT(void *, "p", NULL, !=, node);
	ASSERT(void *, "p", NULL, ==, node->tree.child[0]);
	FREE(*a_node);
	ASSERT(int, "d", 0, <, a_user->node_num);
	--a_user->node_num;
}

enum QuadtreeTraversal
visit(struct TestData *a_user, struct TestNode *a_node)
{
	if (a_user->creation_level > a_node->tree.level) {
		return QUADTREE_CONTINUE;
	}
	return QUADTREE_CLEAR;
}

HTEST(Root)
{
	struct TestData data;
	struct TestTree tree;

	HTRY_SIGNAL(testtree_traverse(&tree));

	ZERO(data);
	testtree_init(&tree, &data);
	HTRY_I(1, ==, data.node_num);
	HTRY_PTR(NULL, !=, tree.root);
	HTRY_FLT(0, ==, tree.root->tree.level);
	HTRY_FLT(0, ==, tree.root->tree.i);
	HTRY_FLT(0, ==, tree.root->tree.j);

	testtree_traverse(&tree);
	HTRY_I(1, ==, data.node_num);
	HTRY_PTR(NULL, !=, tree.root);

	testtree_shutdown(&tree);
	HTRY_I(0, ==, data.node_num);
	HTRY_PTR(NULL, ==, tree.root);

	HTRY_SIGNAL(testtree_traverse(&tree));
}

HTEST(Creating)
{
	struct TestData data;
	struct TestTree tree;
	struct TestNode const *node;
	size_t i;

	ZERO(data);
	testtree_init(&tree, &data);

	data.creation_level = 1;
	testtree_traverse(&tree);
	HTRY_I(1 + SQR(2), ==, data.node_num);
	node = tree.root;
	for (i = 0; 4 > i; ++i) {
		HTRY_FLT(1, ==, node->tree.child[i]->tree.level);
	}
	HTRY_FLT(0, ==, node->tree.child[0]->tree.i);
	HTRY_FLT(0, ==, node->tree.child[0]->tree.j);
	HTRY_FLT(0, ==, node->tree.child[1]->tree.i);
	HTRY_FLT(1, ==, node->tree.child[1]->tree.j);
	HTRY_FLT(1, ==, node->tree.child[2]->tree.i);
	HTRY_FLT(0, ==, node->tree.child[2]->tree.j);
	HTRY_FLT(1, ==, node->tree.child[3]->tree.i);
	HTRY_FLT(1, ==, node->tree.child[3]->tree.j);

	data.creation_level = 2;
	testtree_traverse(&tree);
	HTRY_I(1 + SQR(2) + SQR(4), ==, data.node_num);
	node = tree.root;
	for (i = 0; 4 > i; ++i) {
		HTRY_FLT(1, ==, node->tree.child[i]->tree.level);
	}
	HTRY_FLT(0, ==, node->tree.child[0]->tree.i);
	HTRY_FLT(0, ==, node->tree.child[0]->tree.j);
	HTRY_FLT(0, ==, node->tree.child[1]->tree.i);
	HTRY_FLT(1, ==, node->tree.child[1]->tree.j);
	HTRY_FLT(1, ==, node->tree.child[2]->tree.i);
	HTRY_FLT(0, ==, node->tree.child[2]->tree.j);
	HTRY_FLT(1, ==, node->tree.child[3]->tree.i);
	HTRY_FLT(1, ==, node->tree.child[3]->tree.j);
	{
		node = tree.root->tree.child[0];
		for (i = 0; 4 > i; ++i) {
			HTRY_FLT(2, ==, node->tree.child[i]->tree.level);
		}
		HTRY_FLT(0, ==, node->tree.child[0]->tree.i);
		HTRY_FLT(0, ==, node->tree.child[0]->tree.j);
		HTRY_FLT(0, ==, node->tree.child[1]->tree.i);
		HTRY_FLT(1, ==, node->tree.child[1]->tree.j);
		HTRY_FLT(1, ==, node->tree.child[2]->tree.i);
		HTRY_FLT(0, ==, node->tree.child[2]->tree.j);
		HTRY_FLT(1, ==, node->tree.child[3]->tree.i);
		HTRY_FLT(1, ==, node->tree.child[3]->tree.j);
	}
	{
		node = tree.root->tree.child[1];
		for (i = 0; 4 > i; ++i) {
			HTRY_FLT(2, ==, node->tree.child[i]->tree.level);
		}
		HTRY_FLT(0, ==, node->tree.child[0]->tree.i);
		HTRY_FLT(2, ==, node->tree.child[0]->tree.j);
		HTRY_FLT(0, ==, node->tree.child[1]->tree.i);
		HTRY_FLT(3, ==, node->tree.child[1]->tree.j);
		HTRY_FLT(1, ==, node->tree.child[2]->tree.i);
		HTRY_FLT(2, ==, node->tree.child[2]->tree.j);
		HTRY_FLT(1, ==, node->tree.child[3]->tree.i);
		HTRY_FLT(3, ==, node->tree.child[3]->tree.j);
	}
	{
		node = tree.root->tree.child[2];
		for (i = 0; 4 > i; ++i) {
			HTRY_FLT(2, ==, node->tree.child[i]->tree.level);
		}
		HTRY_FLT(2, ==, node->tree.child[0]->tree.i);
		HTRY_FLT(0, ==, node->tree.child[0]->tree.j);
		HTRY_FLT(2, ==, node->tree.child[1]->tree.i);
		HTRY_FLT(1, ==, node->tree.child[1]->tree.j);
		HTRY_FLT(3, ==, node->tree.child[2]->tree.i);
		HTRY_FLT(0, ==, node->tree.child[2]->tree.j);
		HTRY_FLT(3, ==, node->tree.child[3]->tree.i);
		HTRY_FLT(1, ==, node->tree.child[3]->tree.j);
	}
	{
		node = tree.root->tree.child[3];
		for (i = 0; 4 > i; ++i) {
			HTRY_FLT(2, ==, node->tree.child[i]->tree.level);
		}
		HTRY_FLT(2, ==, node->tree.child[0]->tree.i);
		HTRY_FLT(2, ==, node->tree.child[0]->tree.j);
		HTRY_FLT(2, ==, node->tree.child[1]->tree.i);
		HTRY_FLT(3, ==, node->tree.child[1]->tree.j);
		HTRY_FLT(3, ==, node->tree.child[2]->tree.i);
		HTRY_FLT(2, ==, node->tree.child[2]->tree.j);
		HTRY_FLT(3, ==, node->tree.child[3]->tree.i);
		HTRY_FLT(3, ==, node->tree.child[3]->tree.j);
	}

	data.creation_level = 3;
	testtree_traverse(&tree);
	HTRY_I(1 + SQR(2) + SQR(4) + SQR(8), ==, data.node_num);

	data.creation_level = 1;
	testtree_traverse(&tree);
	HTRY_I(1 + SQR(2), ==, data.node_num);

	testtree_shutdown(&tree);
	HTRY_I(0, ==, data.node_num);
}

HTEST_SUITE(Quadtree)
{
	HTEST_ADD(Root);
	HTEST_ADD(Creating);
}
