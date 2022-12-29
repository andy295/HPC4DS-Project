#include "file_utils.h"

int getFileSize(const char *fileName) {
	FILE *fp = openFile(fileName, READ_ALL, 0); 
	long fSize = ftell(fp); 
	fseek(fp, 0, SEEK_SET); 

	fclose(fp); 

	return fSize; 
}

long readFilePortionForProcess(const char *fileName, char **fileDest, int pid, int proc_number) {
	FILE *fp = openFile(fileName, READ_ALL, 0);
	if (fp != NULL) {
		long fSize = ftell(fp);

		int residual = fSize % proc_number; 
		fSize = fSize / proc_number;
		fseek(fp, pid * fSize, SEEK_SET);

		if (pid == proc_number - 1 && residual != 0)
			fSize += residual;

		*fileDest = malloc(fSize + 1); 
		fread((*fileDest), fSize, 1, fp);
		fclose(fp); 

		(*fileDest)[fSize] = ENDTEXT;

		#if DEBUG
			printf("process %d:\n%s\n", pid, *fileDest);
			printf("total text length: %ld\n", fSize);
		#endif

		return fSize;
	}

	fprintf(stderr, "Process %d: Error while opening file %s\n", pid, fileName);
	return -1;
}

// function to write byte buffer to file
void writeBufferToFile(const char *fileName, BYTE *buffer, int bufferSize, int openMode, int bytePosition) {
	FILE *file = openFile(fileName, openMode, bytePosition);
	fwrite(buffer, sizeof(BYTE), bufferSize, file);
	fclose(file);
}

FILE* openFile(const char *fileName, int openMode, int bytePosition) {
	FILE *file; 

	switch (openMode) {
		case READ:
			file = fopen(fileName, "r");
			break;

		case READ_ALL:
			file = fopen(fileName, "r");
			fseek(file, 0, SEEK_END); 
			break;
		
		case WRITE:
			file = fopen(fileName, "w");
			break;
		
		case APPEND:
			file = fopen(fileName, "a+");
			break;

		case WRITE_AT:
			file = fopen(fileName, "r+");
			fseek(file, bytePosition, SEEK_SET);
			break;

		case READ_B:
			file = fopen(fileName, "rb");
			break;
		
		case WRITE_B:
			file = fopen(fileName, "wb");
			break;
		
		case APPEND_B:
			file = fopen(fileName, "ab+");
			break;

		case READ_ALL_B:
			file = fopen(fileName, "rb");
			fseek(file, 0, SEEK_END); 
			break;

		case WRITE_B_AT:
			file = fopen(fileName, "rb+");
			fseek(file, bytePosition, SEEK_SET);
			break;
		
		default:
			fprintf(stderr, "Error while opening file %s, unknown open mode: %d\n", fileName, openMode);
			return NULL;
	}

	if (file == NULL) {
		fprintf(stderr, "Error while opening file %s\n", fileName);
		return NULL;
	}

	return file;
}

void parseHeader(FileHeader *header, FILE *fp) {
	fread(header, sizeof(unsigned int), 1, fp);

	#if DEBUG
		printf("Encoded text byte size: %d\n", header->byteStartOfDimensionArray - 1);
	#endif
}

void parseBlockLengths(unsigned short *blockLengths, FILE *fp, int numberOfBlocks, int startPos) {
	fseek(fp, 0, SEEK_SET);
	fseek(fp, startPos, SEEK_SET);
	fread(blockLengths, sizeof(unsigned short), numberOfBlocks, fp);
}

void parseHuffmanTree(TreeNode* root, FILE* fp) {
	fseek(fp, 0, SEEK_SET);
	fseek(fp, sizeof(FileHeader), SEEK_SET);

	extractNode(root, fp);
}

void extractNode(TreeNode* root, FILE* fp) {
	root->character = fgetc(fp);
	root->frequency = 0; // not used
	root->leftChild = NULL;
	root->rightChild = NULL;

	char children = fgetc(fp);
	if (IsBit(children, 0)) {
		root->leftChild = malloc(sizeof(TreeNode));
		extractNode(root->leftChild, fp);
	} 
	
	if (IsBit(children, 1)) {
		root->rightChild = malloc(sizeof(TreeNode));
		extractNode(root->rightChild, fp);
	}
}

int getNumberOfLines(FILE *fp) {
    int lines = 0;
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, fp) != -1)
        lines++;

    return lines;
}