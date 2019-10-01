#ifndef HEAP_H
#define HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Tree.h"

typedef struct HeapNode {
	int cost;
	TreeNode *data;
} HeapNode;

typedef struct MinHeap {
	HeapNode **nodes;
	int heapSize;
	int heapCapacity;
} MinHeap;

MinHeap* createMinHeap(int numEntries);

void insertInMinHeap(MinHeap* heap, HeapNode *heapNode);

HeapNode* removeFromMinHeap(MinHeap* heap);

TreeNode* convertMinHeapToHuffManTree(MinHeap *heap);

void printMinHeap(MinHeap* heap);

void freeMinHeap(MinHeap* heap);
#endif