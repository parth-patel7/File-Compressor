#ifndef FREQ_TREE_H
#define FREQ_TREE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MinHeap.h"

typedef struct FTNode {
	int freq;
	char *word;
	struct FTNode *left, *right;
} FTNode;

typedef struct FreqTree {
	FTNode *root;
	int size;
} FreqTree;

FreqTree *createFreqTree();

// this method inserts a new word into the FT.
// If word already exists, then its freq is increased.
void insertFreqTree(FreqTree *root, char *word);

// Free all the dynamically created nodes and the 
// strings inside nodes.
void freeFreqTree(FreqTree *tree);

MinHeap* convertFreqTreeToMinHeap(FreqTree *tree);

void printFreqTree(FreqTree *tree);
#endif