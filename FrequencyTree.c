#include "FrequencyTree.h"

/***************************************************************
 * this class's underlying data structure is BST, on each node,
 * we keep a word along with its count till now. 
 **************************************************************/
FreqTree *createFreqTree() {
	FreqTree *tree = (FreqTree *) malloc(sizeof(FreqTree));
	tree->root = NULL;
	tree->size = 0;
	return tree;
}

// helper
FTNode *createFTNode(char *word) {
	FTNode *node = (FTNode *) malloc(sizeof(FTNode));
	
	// copy the passed string with initial frequency 1
	node->word = strdup(word);
	node->freq = 1;
	node->left = NULL;
	node->right = NULL;
	return node;
}

// this method inserts a new word into the FreqTree.
// If word already exists, then its freq is increased.

// helper method.
FTNode *insertNode(FTNode *root, char *word, int *treeCount) {
	if(root == NULL) {
		root = createFTNode(word);
		*treeCount += 1;
	} else {
		if(strcmp(root->word, word) == 0) {
			root->freq += 1;
		} else if(strcmp(root->word, word) < 0) {
			root->right = insertNode(root->right, word, treeCount);
		} else {
			root->left = insertNode(root->left, word, treeCount);
		}
	}
	return root;
}

void insertFreqTree(FreqTree *tree, char *word) {
	tree->root = insertNode(tree->root, word, &(tree->size));
}


// FreqTree to MinHeap
// FreqTree comparison is based on word, while MinHeap comparison
// is based on the count of word in file
void insertMinHeapInOrder(MinHeap* heap, FTNode *node) {
	if(node != NULL) {
		// process left subtree
		insertMinHeapInOrder(heap, node->left);
		
		// process current node
		HeapNode *tmp = (HeapNode *) malloc(sizeof(HeapNode));
		tmp->cost = node->freq;
		tmp->data = createTreeNode(node->word, NULL, NULL);
		insertInMinHeap(heap, tmp);

		// process right subtree
		insertMinHeapInOrder(heap, node->right);
	}
}

MinHeap* convertFreqTreeToMinHeap(FreqTree *tree) {
	MinHeap* heap = createMinHeap(tree->size);
	
	insertMinHeapInOrder(heap, tree->root);
	
	return heap;
}

void printFTNode(FTNode *root) {
	if(root != NULL) {
		printFTNode(root->left);
		printf("%s: %d\n", root->word, root->freq);
		printFTNode(root->right);
	}
}

void printFreqTree(FreqTree *tree) {
	printFTNode(tree->root);
}


// Free all the dynamically created nodes and the 
// strings inside nodes.
void freeFTNode(FTNode *root) {
	if(root != NULL) {
		freeFTNode(root->left);
		freeFTNode(root->right);
		free(root->word);
		free(root);
	}	
}
void freeFreqTree(FreqTree *tree) {
	freeFTNode(tree->root);
	free(tree);
}