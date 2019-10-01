#ifndef DICT_H
#define DICT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include "Tree.h"

// This file is used to create a dictionary of words along with
// their huffman code. this uses a BST as underlying data structure, where
// The nodes are sorted based on the words at each node.

// Assuming A tree will have maximum 100 edges.
typedef struct DictNode {
	char *key;
	char *value;
	struct DictNode *left, *right;
} DictNode;

typedef struct Dict {
	DictNode *root;
} Dict;

Dict *createDictionary();

// Add a key to the dictionary along with its value.
// if key exists, the value is updated.
void insertDict(Dict *dict, char *key, char *value);

char* findValue(Dict *dict, char *key);

Dict* getReverseDict(Dict *dict);

void saveDictionary(Dict *dict, char *fileName);

Dict *readHuffmanCodebook(char *fileName);

Dict *dictionaryFromHuffman(TreeNode *root);

void printDict(Dict *dict);

void freeDict(Dict *dict);
#endif