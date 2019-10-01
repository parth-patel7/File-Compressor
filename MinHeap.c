#include "MinHeap.h"

/**
 * MinHeap class, which contains a array representing the minHeap
 * in tree format. The priority comparison is done on basis of 
 * the cost in HeapNode
 */

MinHeap* createMinHeap(int numEntries) {
	MinHeap* heap = (MinHeap *) malloc(sizeof(MinHeap));
	heap->nodes = (HeapNode **) malloc(numEntries * sizeof(HeapNode *));
	heap->heapCapacity = numEntries;
	heap->heapSize = 0;
	return heap;
}


void increaseCapacity(MinHeap* heap) {
	int s = heap->heapSize;
	int i;
	
	HeapNode **tmp = malloc((s + 10) * sizeof(HeapNode *));
	for(i=0; i<s; i++) {
		tmp[i] = heap->nodes[i];
	}
	
	// free old array
	free(heap->nodes);
	
	// assign the new array on old reference
	heap->nodes = tmp;
	heap->heapCapacity = s + 10;
}

// swap 2 indices in the heap array
void swap(MinHeap *heap, int i, int j) { 
    HeapNode *temp = heap->nodes[i]; 
    heap->nodes[i] = heap->nodes[j];
    heap->nodes[j] = temp;
}
  

// Methods to find the indices corresponding to a node.
int parent(int i) { return (i-1)/2; }
int left(int i) { return (2*i + 1); }
int right(int i) { return (2*i + 2); }


void insertInMinHeap(MinHeap* heap, HeapNode *heapNode) {
	if(heap->heapSize == heap->heapCapacity) {
		increaseCapacity(heap);
	}
	
    int i = heap->heapSize;
    heap->nodes[i] = heapNode;
	heap->heapSize += 1;
  
    // Bubble up, till we reach parent, or parent is lesser than new node.
    while (i != 0 && heap->nodes[parent(i)]->cost > heap->nodes[i]->cost) { 
       swap(heap, i, parent(i));
       i = parent(i);
    }
}

// this method heapify the children starting from a particular 
// node index
void bubbleDown(MinHeap* heap, int i) {
	
    HeapNode *current = heap->nodes[i]; 
    HeapNode *l = NULL; 
    HeapNode *r = NULL; 
	
    HeapNode *smallest = current;
	
	if(left(i) < heap->heapSize) {
		l = heap->nodes[left(i)];
	}
	if(right(i) < heap->heapSize) {
		r = heap->nodes[right(i)];
	}
	
    if ((l != NULL) && (l->cost < smallest->cost)) { 
        smallest = l; 
	}
    if ((r != NULL) && (r->cost < smallest->cost)) { 
        smallest = r; 
	}
	
    if (smallest != current) {
		int swapIndex = (smallest == l) ? left(i) : right(i);
		swap(heap, i, swapIndex);
        bubbleDown(heap, swapIndex); 
    }
}

// we just remove the heapNode, it is on external program to free
// up the space.
HeapNode* removeFromMinHeap(MinHeap* heap) {
	
	HeapNode *result = NULL;
	
	// If heap is empty.
    if (heap->heapSize <= 0) {
        return result; 
	}
  
    // Store the minimum value, and remove it from heap 
	result = heap->nodes[0];
    heap->heapSize--;
    heap->nodes[0] = heap->nodes[heap->heapSize]; 
	heap->nodes[heap->heapSize] = NULL;
	if(heap->heapSize != 0) {
		bubbleDown(heap, 0);
	}
  
    return result; 
}

TreeNode* convertMinHeapToHuffManTree(MinHeap *heap) {
	
	// Special Case, when there is just one node in minHeap,
	// The that should be represented as 2 node tree,
	// the root containing cost, and then left child of that
	// will contain the actual word.
	if(heap->heapSize == 1) {
		TreeNode *tmpRoot = createTreeNode("", heap->nodes[0]->data, NULL);
		heap->nodes[0]->data = tmpRoot;
	}
	
	// printMinHeap(heap);
	
	// If there is only one node., then no need to further compress
	while(heap->heapSize > 1) {
		HeapNode *min1 = removeFromMinHeap(heap);		
		HeapNode *min2 = removeFromMinHeap(heap);
		
		// create a consolidated TreeNode from both minHeap nodes.
		// We need not store the cost in the tree,
		// As we are already storing the root node's cost in HeapNode.
		HeapNode *consolidatedNode = (HeapNode *) malloc(sizeof(HeapNode));
		consolidatedNode->cost = min1->cost + min2->cost;
		consolidatedNode->data = createTreeNode("", min1->data, min2->data);
		
		// consolidatedNode's tree can have "" as word in the internal nodes.
		// only leaf nodes contain valid words.
		
		// Now, we can free the min1 and min2 node.
		free(min1);
		free(min2);
		
		// insert new node to the heap
		insertInMinHeap(heap, consolidatedNode);
	}
	
	// Now, ideally Heap should have just one node.. and inside 
	// that the treeNode has huffman tree
	if(heap->heapSize > 0) {
		return heap->nodes[0]->data;
	} else {
		return NULL;
	}
}


// Utility function for debugging
void printMinHeap(MinHeap* heap) {
	int i = 0;
	while(i < heap->heapSize) {
		printf("Heap node %d cost: %d\n", i, heap->nodes[i]->cost);
		printTree(heap->nodes[i]->data);
		printf("\n");
		i++;
	}
	printf("HeapSize: %d\n", heap->heapSize);
}

void freeMinHeap(MinHeap* heap) {
	int i = 0;
	while(i < heap->heapSize) {
		if(heap->nodes[i] != NULL) {
			free(heap->nodes[i]);
		}
		i++;
	}
	free(heap->nodes);
	free(heap);
}

