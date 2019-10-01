all: Dictionary.o FrequencyTree.o MinHeap.o Tree.o fileCompressor.o fileCompressor 

Dictionary.o: Dictionary.c
	gcc -c Dictionary.c

FrequencyTree.o: FrequencyTree.c
	gcc -c FrequencyTree.c

MinHeap.o: MinHeap.c
	gcc -c MinHeap.c

Tree.o: Tree.c
	gcc -c Tree.c

fileCompressor.o: fileCompressor.c
	gcc -c fileCompressor.c

fileCompressor: Dictionary.o FrequencyTree.o MinHeap.o Tree.o fileCompressor.o
	gcc -o fileCompressor Dictionary.o FrequencyTree.o MinHeap.o Tree.o fileCompressor.o

clean:
	rm -f fileCompressor *.o
