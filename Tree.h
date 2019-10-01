#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TreeNode {
	char *word;
	struct TreeNode *left, *right;
} TreeNode;

TreeNode *createTreeNode(char *word, TreeNode *leftNode, TreeNode* rightNode);

void freeTree(TreeNode *root);

void printTree(TreeNode *root);
#endif