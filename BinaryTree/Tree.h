#pragma once

#include<stdio.h>
#include<malloc.h>
#include<stdlib.h>
#include<GL/gl.h>

struct tree
{
	int level;
	struct tree* branch1;
	struct tree* branch2;
	struct tree* parent;
};
struct tree* root = NULL;

void newBranch(struct tree* p, int branchNo)
{
	struct tree* node = NULL;
	node = (struct tree*)malloc(sizeof(struct tree));

	if (node == NULL)
	{
		exit(-1);
	}
	node->branch1 = NULL;
	node->branch2 = NULL;
	node->parent = p;

	if (node->parent == NULL)
	{
		node->level = 0;
		root = node;
	}
	else
	{
		node->level = node->parent->level + 1;
		if (branchNo == 1)
		{
			p->branch1 = node;
		}
		else if (branchNo == 2)
		{
			p->branch2 = node;
		}
	}
}

void genTree(struct tree* newNode, int height)
{
	newBranch(newNode, 1);  // creating branch 1

	if (newNode->branch1->level < height)
	{
		genTree(newNode->branch1, height);
	}
	newBranch(newNode, 2);  // creating branch 2

	if (newNode->branch2->level < height)
	{
		genTree(newNode->branch2, height);
	}
}

void generateTree(int height)
{
	newBranch(NULL, 0); // root node 
	genTree(root, height);
}

void freeTree(struct tree *node)
{
	if (node == NULL)
	{
		return;
	}
	if (node->branch1 != NULL)
	{
		freeTree(node->branch1);
	}
	if (node->branch2 != NULL)
	{
		freeTree(node->branch2);
	}
	if (node->branch1 == NULL && node->branch2 == NULL)
	{
		if (node->parent->branch1 == node)
		{
			node->parent->branch1 = NULL;
		}
		if (node->parent->branch2 == node)
		{
			node->parent->branch2 = NULL;
		}
		free(node);
		node = NULL;
	}
}