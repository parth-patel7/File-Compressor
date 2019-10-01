#include "Tree.h"

TreeNode *createTreeNode(char *word, TreeNode *leftNode, TreeNode* rightNode) {
	TreeNode *tmp = (TreeNode *) malloc(sizeof(TreeNode));
	tmp->word = strdup(word);
	tmp->left = leftNode;
	tmp->right = rightNode;
	return tmp;
}

void freeTree(TreeNode *root) {
	if(root != NULL) {
		freeTree(root->left);
		freeTree(root->right);
		free(root->word);
		free(root);
	}
}


void printTree(TreeNode *root) {
	if(root != NULL) {
		printTree(root->left);
		printf("%s ", root->word ? root->word : " ");
		printTree(root->right);
	}
}