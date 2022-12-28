#include "file_utils.h"

int getFileSize(const char *fileName) {
	FILE *fp = openFile(fileName, READ_ALL, 0); 
	long fSize = ftell(fp); 
	fseek(fp, 0, SEEK_SET); 

	fclose(fp); 

	return fSize; 
}

long readFilePortionForProcess(const char *fileName, char **fileDest, int processId, int proc_number) {
	FILE *fp = openFile(fileName, READ_ALL, 0);
	if (fp != NULL) {
		long fSize = ftell(fp);

		int residual = fSize % proc_number; 
		fSize = fSize / proc_number;
		fseek(fp, processId * fSize, SEEK_SET);

		if (processId == proc_number - 1 && residual != 0)
			fSize += residual;

		*fileDest = malloc(fSize + 1); 
		fread((*fileDest), fSize, 1, fp);
		fclose(fp); 

		(*fileDest)[fSize] = ENDTEXT;

		#if DEBUG
			printf("process %d:\n%s\n", processId, *fileDest);
			printf("total text length: %ld\n", fSize);
		#endif

		return fSize;
	}

	printf("Error opening file %s\n", fileName);
	return 0; 
}

// function to write byte buffer to file
void writeBufferToFile(const char *filename, BYTE *buffer, int bufferSize, int openMode, int bytePosition) {
	FILE *file = openFile(filename, openMode, bytePosition);
	fwrite(buffer, sizeof(BYTE), bufferSize, file);
	fclose(file);
}

FILE* openFile(const char *filename, int openMode, int bytePosition) {
	FILE *file; 

	switch (openMode) {
		case READ:
			file = fopen(filename, "r");
			break;

		case READ_ALL:
			file = fopen(filename, "r");
			fseek(file, 0, SEEK_END); 
			break;
		
		case WRITE:
			file = fopen(filename, "w");
			break;
		
		case APPEND:
			file = fopen(filename, "a+");
			break;

		case WRITE_AT:
			file = fopen(filename, "r+");
			fseek(file, bytePosition, SEEK_SET);
			break;

		case READ_B:
			file = fopen(filename, "rb");
			break;
		
		case WRITE_B:
			file = fopen(filename, "wb");
			break;
		
		case APPEND_B:
			file = fopen(filename, "ab+");
			break;

		case READ_ALL_B:
			file = fopen(filename, "rb");
			fseek(file, 0, SEEK_END); 
			break;

		case WRITE_B_AT:
			file = fopen(filename, "rb+");
			fseek(file, bytePosition, SEEK_SET);
			break;
		
		default:
			printf("Error while opening file %s, unknown open mode: %d\n", filename, openMode);
			return NULL;
	}

	if (file == NULL) {
		printf("Error while opening file %s\n", filename);
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