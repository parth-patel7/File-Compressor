#include <stdio.h> 
#include <dirent.h>
#include <string.h> 
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <getopt.h>

#include "FrequencyTree.h"
#include "Tree.h"
#include "MinHeap.h"
#include "Dictionary.h"
  
#define MAX_FILES 1000000


/**
 * This function takes a starting directory path and a array to which file paths need
 * to be appended.
 * numFiles are files appended to array currently.
 * If recursive variable is not 0, then this function scans all the directories recursively.
 * Else, only the current directory is scanned.
 */
void getFilePathsFromDir(char *startPath, char **filePaths, int *numFiles, int recursive) {
	
    struct stat path_stat;
    stat(startPath, &path_stat);
    // if given path represents a file, just add the path to list and return
	if(S_ISREG(path_stat.st_mode)) {
		filePaths[*numFiles] = strdup(startPath);
		//printf("%s\n", startPath);
		*numFiles += 1;
		return;
	}

	struct dirent * dir;	
	DIR * d = opendir(startPath); // open the directory
	if(d == NULL) {
		printf("Check Input. Invalid path. Unable to open: %s\n", startPath);
		exit(1);
	}
	
	// read entries from directory.
	while ((dir = readdir(d)) != NULL) {
		
		char *pathStr = malloc(sizeof(char) * (10 + strlen(startPath) + strlen(dir->d_name)));
		pathStr[0] = '\0';
		sprintf(pathStr, "%s/%s", startPath, dir->d_name);	
			
		// if the entry is not a directory
		if(dir-> d_type != DT_DIR) {
			filePaths[*numFiles] = strdup(pathStr);
			//printf("%s\n", pathStr);
			*numFiles += 1;
			
		// or if directory is a valid child directory
		} else if(recursive && dir -> d_type == DT_DIR && 
					strcmp(dir->d_name,".")!=0 && 
					strcmp(dir->d_name,"..")!=0 ) {
								
			getFilePathsFromDir(pathStr, filePaths, numFiles, recursive);			
		}
		
		free(pathStr);
	}
	
	closedir(d); // finally close the directory
}




/***********************************************************
 * this function reads the file and creates token by splitting
 * the content by whitespace chars. It then insert all the tokens
 * including whitespaces into a FreqTree, which maintains count of 
 * inserted words.
 ***********************************************************/
void fillWordCountTree(FreqTree *tree, char *fileName) {
	
    int filedesc = open(fileName, O_RDONLY);
    if(filedesc < 0){
        printf("Error in reading file: %s\n", fileName);
        return;
    }
		
	char word[500];
	int len = 0;
	char c;
	
	// read one by one character
    while(read(filedesc, &c, 1) == 1) {
		if(isspace(c)) {
			// we need to break our word
			if(len != 0) {
				word[len] = '\0';
				insertFreqTree(tree, word);
			}
			len = 0;
			
			// now insert your whitespace also to the tree.
			word[0] = c;
			word[1] = '\0';
			insertFreqTree(tree, word);
		} else {
			// add current character to the current word
			word[len++] = c;
		}
    }
	
	// insert last word
	if(len != 0) {
		word[len] = '\0';
		insertFreqTree(tree, word);
	}
	
	close(filedesc);
}




/*************************************************
 * Utility function to check if file name ends with a 
 * .hcz extension
 ************************************************/
int endsWithHcz( char *filePath ) {
  filePath = strrchr(filePath, '.');

  if( filePath != NULL )
    return (strcmp(filePath, ".hcz") == 0);

  return 0;
}





/**********************************************************
 * this method takes a input file and a codebook and using 
 * the codebook, compresses the input file and creates a 
 * <input>.hcz compressed file.
 *********************************************************/

// compression code for single file
void compressSingleFile(char *fileName, Dict *wordToCode) {
	
    int readerDesc = open(fileName, O_RDONLY);
    if(readerDesc < 0){
        printf("Error in reading file: %s\n", fileName);
		//fflush(stdout);
        return;
    }
	
	// open file for writing the compressed version
	char *path = malloc(sizeof(char) * (strlen(fileName) + 10));
	path[0] = '\0';
	strcat(path, fileName);
	strcat(path, ".hcz");
	
    int writerDesc = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(writerDesc < 0){
        printf("Error in writing file: %s\n", path);
		//fflush(stdout);
		free(path);
        return;
    }
	
	char word[500];
	int len = 0;
	char c;
	char *code;
	
	// read character by character
    while (read(readerDesc, &c, 1) == 1) {
		if(isspace(c)) {
			// we need to break our word
			if(len != 0) {
				word[len] = '\0';
				code = findValue(wordToCode, word);
				if(code != NULL) {
					write(writerDesc, code, strlen(code));
				}
			}
			len = 0;
			
			// now insert your whitespace also to the tree.
			word[0] = c;
			word[1] = '\0';
			// check if currently read word exists in dictionary
			code = findValue(wordToCode, word);
			if(code != NULL) {
				write(writerDesc, code, strlen(code));
			}
		} else {
			// add current character to the current word
			word[len++] = c;
		}
    }
	
	// insert last word
	if(len != 0) {
		word[len] = '\0';
		code = findValue(wordToCode, word);
		if(code != NULL) {
			write(writerDesc, code, strlen(code));
		}
	}
	
	free(path);
	close(writerDesc);
	close(readerDesc);
}

// this method compresses all the given files in list using the same 
// codebook.
void compressAllFiles(char **filePaths, int numFiles, char *codeBook) {
	
	// Create Codebook with word -> Code mapping
	Dict *wordToCode = readHuffmanCodebook(codeBook);
	
	int count = 0;	
	int processedFiles = 0;
	while(count < numFiles) {
		
		if(!endsWithHcz(filePaths[count])) {
			printf("Compressing: %s\n", filePaths[count]);
			compressSingleFile(filePaths[count], wordToCode);
			processedFiles++;
		}		
		// move to next file
		count++;
	}
	
	freeDict(wordToCode);
	printf("Compressed %d files successfully using %s codebook.\n", processedFiles, codeBook);
	//fflush(stdout);
}




/**********************************************************
 * this method takes a input compressed file as <file>.hcz
 * and a code to word Dictionary. The decompressed file is 
 * written with name <file>
 * The compressed file should only contain 1 and 0.
 *********************************************************/
 
// decompression logic for a single file.
void decompressSingleFile(char *fileName, Dict *CodeToWord) {	
    int readerDesc = open(fileName, O_RDONLY);
    if(readerDesc < 0){
        printf("Error in reading file: %s\n", fileName);
        return;
    }
	
	
	// create a path for output file with .hcz removed from fileName.	
	char *outPath = malloc(sizeof(char) * (10 + strlen(fileName)));
	strncpy(outPath, fileName, strlen(fileName) - 4);
	outPath[strlen(fileName) - 4] = '\0';
	
    int writerDesc = open(outPath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(writerDesc < 0){
        printf("Error in writing file: %s\n", outPath);
		free(outPath);
        return;
    }
	
	// Now read the input file character by character
	char word[500];
	int len = 0;
	char c;
    while (read(readerDesc, &c, 1) == 1) {
		word[len++] = c;
		word[len] = '\0';
		
		// check if currently read code exists in dictionary
		char *token = findValue(CodeToWord, word);
		if(token != NULL) {
			// It was a valid word, and hence, we can print to file
			write(writerDesc, token, strlen(token));
			
			// reset current code,
			len = 0;
		}
    }
	
	// close files.
	free(outPath);
	close(writerDesc);
	close(readerDesc);
}

// this method decompresses all the given files in list using the same 
// codebook.
void decompressAllFiles(char **filePaths, int numFiles, char *codeBook) {
	
	// Original Codebook has word -> Code mapping
	Dict *wordToCode = readHuffmanCodebook(codeBook);
	
	// We need dictionary with Code -> word mapping, so 
	// get reversed dictionary
	Dict *CodeToWord = getReverseDict(wordToCode);
	
	int count = 0;	
	int processedFiles = 0;	
	while(count < numFiles) {
		
		if(endsWithHcz(filePaths[count])) {
			printf("Decompressing: %s\n", filePaths[count]);
			decompressSingleFile(filePaths[count], CodeToWord);
			processedFiles++;
		}
		
		// move to next file
		count++;
	}
	
	// free the dictionaries
	freeDict(wordToCode);
	freeDict(CodeToWord);
	printf("Decompressed %d files successfully using %s codebook.\n", processedFiles, codeBook);
}




/**************************************************************************
 * This method takes a list of files, and after reading them creates a huffman
 * codebook on the basis of words inside all the given files.
 *************************************************************************/
void createCodeBook(char **filePaths, int numFiles, char *outputCodeBook) {
	
	// Change code here to fill a created FreqTree..
	// The method should read the file, and fill FreqTree only.
	// That way, we can use it for multiple files.
	FreqTree *wordCountTree = createFreqTree();
	
	int count = 0;	
	int processedFiles = 0;	
	while(count < numFiles) {
		
		if(!endsWithHcz(filePaths[count])) {
			printf("Processing: %s\n", filePaths[count]);		
			fillWordCountTree(wordCountTree, filePaths[count]);
			processedFiles++;
		}
		count++;
	}
	//printf("Could created the Frequency tree from all files successfully\n");
	
	MinHeap *minHeap = convertFreqTreeToMinHeap(wordCountTree);
	freeFreqTree(wordCountTree);  // free FreqTree, its work is done.
	//printf("Could created the Min Heap successfully\n");
	
	TreeNode *huffmanTree = convertMinHeapToHuffManTree(minHeap);
	freeMinHeap(minHeap);  // free MinHeap, its work is done.
	//printf("Could created the Huffman Tree successfully\n");
	
	Dict *codeDictionary = dictionaryFromHuffman(huffmanTree);
	freeTree(huffmanTree);  // free huffmanTree, its work is done.
	//printf("Could created the Code Dictionary successfully\n");
	
	saveDictionary(codeDictionary, outputCodeBook);
	freeDict(codeDictionary);  // free dictionary, its work is done.
	
	printf("Processed %d files.\n", processedFiles);
	printf("Saved Codebook %s successfully\n", outputCodeBook);
}




/**************************************************************************
 * This method shows the correct way of using the utility.
 *************************************************************************/
void showUsage() {
	printf("./fileCompressor [-R] <flag> <path or file> |codebook|\n");
	printf("Valid Flags: b, c, d\n");
	printf("-R: recursive\n");
	printf("Codebook is required if flag is c or d\n");
}



/**************************************************************************
 * Main method:
 * It checks for required paramters, derives the list of files to be used
 * and then invoke appropriate module(compress, decompress, buildCodebook)
 *************************************************************************/
int main(int argc, char *argv[]) {
	int option; 

	// flags to check what user wants to do
	int buildFlag = 0;
	int compressFlag = 0;
	int decompressFlag = 0;
	
	// are all files inside folder need to be considered.
	int recursive = 0;
	
	// input directory/file path
	char *dirPath = NULL;
	
	// Codebook to be used for compress/decompress
	char *codebook = NULL;
	
	// set this module, to not display getopt module errors.
	// we are showing our own errors.
	opterr = 0;

	// Below we are allowing b, c, d as options to contains value
	// and R to be without value.
	while  ((option = getopt(argc, argv, "bcdR")) != -1) {
		switch (option)
		{
		case 'b':
			buildFlag = 1;
			break;
		case 'c':
			compressFlag = 1;
			break;
		case 'd':
			decompressFlag = 1;
			break;
		case 'R':
			recursive = 1;
			break;
		case '?':
			if (isprint(optopt))
				printf ("Unknown option `-%c'.\n", optopt);
			else
				printf ("Unknown option character `\\x%x'.\n", optopt);
			exit(1);
		default:
			exit(1);
		}
	}
	
	while (optind < argc) {
		
		// Extra parms go in dirPath and codeBook
		
		if(dirPath == NULL) {
			dirPath = argv[optind];
		} else if(codebook == NULL) {
			codebook = argv[optind];
		}
		// else ignore extra params
		optind++;  // go to the next argument
	}
	
	int setFlags = 0;
	if (buildFlag) setFlags++;
	if (compressFlag) setFlags++;
	if (decompressFlag) setFlags++;
	
	if(setFlags != 1) {
		printf("Exactly one flag out of b, c & d can be set.\n");
		showUsage();
		return 1;
	}
	
	if((compressFlag || decompressFlag) && (codebook == NULL)) { 
		printf("Flags c & d must provide codebook.\n");
		showUsage();
		return 1;
	}
	
	printf("Starting to Read all the files or the file paths in dictionaries\n");
	//fflush(stdout);
	
	// create the list of all the files (recursive if required)
	char **filePaths = malloc(sizeof(char *) * MAX_FILES);
	int numFiles = 0;
	getFilePathsFromDir(dirPath, filePaths, &numFiles, recursive);
	
	// Based on user operation, call required method.
	if(buildFlag) {
		createCodeBook(filePaths, numFiles, "HuffmanCodebook");
	} else if(compressFlag) {
		compressAllFiles(filePaths, numFiles, codebook);
	} else if(decompressFlag) {
		decompressAllFiles(filePaths, numFiles, codebook);
	}
	
	// free memory from file array
	int i;
	for(i=0; i<numFiles; i++) {
		free(filePaths[i]);
	}
	free(filePaths);
	
    return 0; 
} 
