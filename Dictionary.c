#include "Dictionary.h"

Dict *createDictionary() {
	Dict *tree = (Dict *) malloc(sizeof(Dict));
	tree->root = NULL;
	return tree;
}

DictNode *createNewDictNode(char *key, char *value) {
	DictNode *tmp = (DictNode *) malloc(sizeof(DictNode));
	tmp->key = strdup(key);
	tmp->value = strdup(value);
	tmp->right = NULL;
	tmp->left = NULL;
	return tmp;
}

// this method inserts a new key into the BST.
// If key already exists, then its freq is increased.
void insertDict(Dict *dict, char *key, char *value) {
	if(dict->root == NULL) {
		dict->root = createNewDictNode(key, value);
	} else {
		DictNode *start = dict->root;
		
		while(1) {
			// if current node has same key
			if(strcmp(start->key, key) == 0) {
				// update value, and return
				free(start->value);
				start->value = strdup(value);
				return;
				
			} else if(strcmp(start->key, key) < 0) {
				if(start->right == NULL) {
					start->right = createNewDictNode(key, value);
					return;
				} else {
					start = start->right;
				}
			} else {
				if(start->left == NULL) {
					start->left = createNewDictNode(key, value);
					return;
				} else {
					start = start->left;
				}
			}
		}
	}
}

// Free all the dynamically created nodes and the 
// strings inside nodes.
char* findValue(Dict *dict, char *key) {
	DictNode *start = dict->root;
	
	while(start != NULL) {
		// if current node has same key
		if(strcmp(start->key, key) == 0) {
			return start->value;
			
		} else if(strcmp(start->key, key) < 0) {
			start = start->right;
		} else {
			start = start->left;
		}
	}
	return NULL;
}


/****************************************************
 * Below methods generate a reversed Dictionary where
 * the values from original dictionary are kept as key
 * in the new Dictionary. Similarly, keys from original
 * dictionary are kept as values in new dictionary.
 ****************************************************/
// Helper
void copyNodeToDict(Dict *newDict, DictNode *oldDictNode) {
	
	if(oldDictNode != NULL) {
		copyNodeToDict(newDict, oldDictNode->left);
		
		// insert into dictionary,
		// interchange key and value
		insertDict(newDict, oldDictNode->value, oldDictNode->key);
		
		copyNodeToDict(newDict, oldDictNode->right);		
	}
}

Dict* getReverseDict(Dict *dict) {
	Dict *result = createDictionary();	
	copyNodeToDict(result, dict->root);
	return result;
}




// We need to convert string in such a way, so that
// all backslash \ represent a escape char including its
// next character in file
// So, \n means newline
// \t means tab, \\ means \, and so on.
void getEscapedString(char *str, char *result) {
	int i = 0;
	int j = 0;
	
	while(str[i] != '\0') {
		// if 
		if(str[i] == '\n') {
			result[j++] = '\\';
			result[j++] = 'n';
		} else if(str[i] == '\t') {
			result[j++] = '\\';
			result[j++] = 't';
        } else if(str[i] == '\a') {
            result[j++] = '\\';
            result[j++] = 'a';
        } else if(str[i] == '\b') {
            result[j++] = '\\';
            result[j++] = 'b';
        } else if(str[i] == '\f') {
			result[j++] = '\\';
			result[j++] = 'f';
		} else if(str[i] == '\v') {
			result[j++] = '\\';
			result[j++] = 'v';
		} else if(str[i] == '\r') {
			result[j++] = '\\';
			result[j++] = 'r';
		} else if(str[i] == '\\') {
			result[j++] = '\\';
			result[j++] = '\\';
        } else {
			result[j++] = str[i];
		}
		i++;
	}
	result[j] = '\0';
}

void getUnescapedString(char *str, char *result) {
	int i = 0;
	int j = 0;
	
	while(str[i] != '\0') {
		
		if(str[i] == '\\') {
			// found a 2 characters escaped string.
			if(str[i+1] == 'n') {
				result[j++] = '\n';
			} else if(str[i+1] == 't') {
				result[j++] = '\t';
            } else if(str[i+1] == 'a') {
                result[j++] = '\a';
            } else if(str[i+1] == 'b') {
                result[j++] = '\b';
            } else if(str[i+1] == 'f') {
				result[j++] = '\f';
			} else if(str[i+1] == 'v') {
				result[j++] = '\v';
			} else if(str[i+1] == 'r') {
				result[j++] = '\r';
			} else if(str[i+1] == '\\') {
				result[j++] = '\\';
            }
			i += 2;
		} else {
			result[j++] = str[i++];
		}	
	}
	result[j] = '\0';
}


// helper function to write the Huffman Codebook
void writeDictNode(DictNode *node, int writerDesc) {
	if(node != NULL) {
		writeDictNode(node->left, writerDesc);
		
		char escaped[500];
		getEscapedString(node->key, escaped);	
        write(writerDesc,node->value, strlen(node->value));
        write(writerDesc,"\t", 1);
        write(writerDesc,escaped, strlen(escaped));
        write(writerDesc,"\n", 1);
		
		writeDictNode(node->right, writerDesc);		
	}
}

// Save the dictionary of words and their codes in a 
// Huffman codebook
void saveDictionary(Dict *dict, char *fileName) {
	
    int writerDesc = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(writerDesc < 0){
        printf("Error in writing file: %s\n", fileName);
        exit(1);
    }
	
	writeDictNode(dict->root, writerDesc);
	close(writerDesc);
}

// This function reads a huffman codebook file into a dictionary
Dict *readHuffmanCodebook(char *fileName) {
	
    int readerDesc = open(fileName, O_RDONLY);
    if(readerDesc < 0){
        printf("Error in reading file: %s\n", fileName);
        exit(1);
    }
	
	Dict *dict = createDictionary();
	char token[500], code[500], unescapedToken[500];
	int codeLen = 0;
	int tokenLen = 0;
	int codeStarted = 1;
	int tokenStarted = 0;
	
	char c;
    while (read(readerDesc, &c, 1) != 0) {
		
		// if line contains a new line at last, remove it.
		if((c == '\n') && tokenStarted) {
			token[tokenLen] = '\0';
			code[codeLen] = '\0';
			getUnescapedString(token, unescapedToken);	
			// insert the token & code into dictionary
			insertDict(dict, unescapedToken, code);
			
			tokenLen = 0;
			codeLen = 0;
			tokenStarted = 0;
			codeStarted = 1;
		} else if(c == '\t' && codeStarted) {
			// we are starting tokenNow.
			tokenStarted = 1;
			codeStarted = 0;
		} else if(codeStarted) {
			code[codeLen++] = c;
		} else if(tokenStarted) {
			token[tokenLen++] = c;
		}		
	};
	close(readerDesc);
	return dict;
}


/*****************************************************************
 * Below method converts a huffman tree to a dictionary of 
 * words, where dictionary contains the word and its code
 * in huffman representation.
 *****************************************************************/
// Helper
void addWordsFromHuffman(Dict *dict, TreeNode *node, char *path) {
	
	//printf("|%s|: %s\n", node->key ? node->key : " ", path);
	if(node != NULL) {
		
		// check if leaf node.
		if((node->left == NULL) && (node->right == NULL)) {
			insertDict(dict, node->word, path);
		} else {
			char *newPath = malloc(sizeof(char) * (10 + strlen(path)));
			newPath[0] = '\0';
			strcat(newPath, path);
			strcat(newPath, "0");
			addWordsFromHuffman(dict, node->left, newPath);
			
			newPath[0] = '\0';
			strcat(newPath, path);
			strcat(newPath, "1");	
			addWordsFromHuffman(dict, node->right, newPath);
			
			free(newPath);
		}
	}
}

Dict *dictionaryFromHuffman(TreeNode *root) {
	Dict *dict = createDictionary();
	addWordsFromHuffman(dict, root, "");
	return dict;
}

// Utility function for debugging
void printDictNode(DictNode *root) {
	
	if(root != NULL) {
		printDictNode(root->left);
		printf("(%s |%s|) ", root->key ? root->key : "", root->value);
		printDictNode(root->right);
	}
}
void printDict(Dict *dict) {
	printDictNode(dict->root);
	printf("\n");
}




// Free all the dynamically created nodes and the 
// strings inside nodes.
void freeDictNode(DictNode *root) {
	if(root != NULL) {
		DictNode *del = root;
		freeDictNode(root->left);
		freeDictNode(root->right);
		free(root->key);
		free(root->value);
		free(root);
	}	
}
void freeDict(Dict *dict) {
	freeDictNode(dict->root);
	free(dict);
}
