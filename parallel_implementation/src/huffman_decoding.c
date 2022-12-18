#include "include/huffman_decoding.h"

char* decode_from_file(FILE* fp2, TreeNode* root, int bitsToProcess, int bitsToSkip, int* numberOfChars){
	char c;
	TreeNode* intermediateNode = root;

	char* decodedText = malloc(sizeof(char) * bitsToProcess);

	int bitsProcessed = 0;
	while (fread(&c, sizeof(char), 1, fp2)) {
		for (int i = 0; i < 8; i++) {
			if (bitsToSkip > 0) { // skip the initial bits, if any
				bitsToSkip--;
				continue;
			}

			if (bitsProcessed >= bitsToProcess) {
				break;
			}
			bitsProcessed++;

			if (intermediateNode->character != '$') {

				//printf("%c", intermediateNode->character);
				decodedText[(*numberOfChars)++] = intermediateNode->character;

				intermediateNode = root;
			}

			if (c & (1 << i)) {
				intermediateNode = intermediateNode->rightChild;
			} else {
				intermediateNode = intermediateNode->leftChild;
			}
		}
	}

	fclose(fp2);
	return decodedText;
}

TreeNode* readTreeFromFile(FILE* fp) {
	TreeNode* node = malloc(sizeof(TreeNode));
	node->character = fgetc(fp);
	node->frequency = 10; // not used
	node->leftChild = NULL;
	node->rightChild = NULL;

	char children = fgetc(fp);
	if (IsBit(children, 0)) {
		node->leftChild = readTreeFromFile(fp);
	} 
	
	if (IsBit(children, 1)) {
		node->rightChild = readTreeFromFile(fp);
	}

	return node;
}

short* readBlockLengths(FILE* fp, int numberOfBlocks) {
	short* blockLengths = malloc(sizeof(short) * numberOfBlocks);
	fread(blockLengths, sizeof(short), numberOfBlocks, fp);
	return blockLengths;
}

typedef struct EncodedFileHeader {
//	short number_of_blocks;
	unsigned int encoded_text_byte_size; 
} EncodedFileHeader;

EncodedFileHeader* readHeaderFromFile(FILE* fp) {
	EncodedFileHeader* header = malloc(sizeof(EncodedFileHeader));
	//fread(&header->number_of_blocks, sizeof(short), 1, fp);
	//printf("Number of blocks: %d\n", header->number_of_blocks);
	fread(&header->encoded_text_byte_size, sizeof(unsigned int), 1, fp);
	printf("Encoded text byte size: %d\n", header->encoded_text_byte_size);
	return header;
}

int main() {
	MPI_Init(NULL, NULL);

	int proc_number;
	int pid; 

	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	
    FILE *fp2;
	fp2 = fopen(ENCODED_FILE, "rb");

	EncodedFileHeader *header = readHeaderFromFile(fp2);

//	fseek(fp2, 0, SEEK_SET);
	fseek(fp2, sizeof(EncodedFileHeader), SEEK_SET);
	TreeNode* root = readTreeFromFile(fp2);
	int nodes = countTreeNodes(root);
	int treeByteSize = nodes * sizeof(TreeArrayItem);

	int number_of_blocks = (getFileSize(ENCODED_FILE) - sizeof(EncodedFileHeader) - header->encoded_text_byte_size) / sizeof(short);
	printf("Number of blocks: %d\n", number_of_blocks); 

//	fseek(fp2, 0, SEEK_SET);
	fseek(fp2, sizeof(EncodedFileHeader) + treeByteSize + header->encoded_text_byte_size, SEEK_SET);
	short *blockLengths = readBlockLengths(fp2, number_of_blocks);

	if (pid == 0) {
		for (int i = 0; i < header->number_of_blocks; i++) {
			printf("Block %d: %d bits\n", i, blockLengths[i]);
		}
	}

	float partBlockPerProcess = ((float)header->number_of_blocks) / proc_number;
	int idealBlocksPerProcess = (int)partBlockPerProcess + (partBlockPerProcess - (int)partBlockPerProcess > 0 ? 1 : 0); // ceil
	if (pid == proc_number - 1) {
		idealBlocksPerProcess = header->number_of_blocks - (proc_number-1)*idealBlocksPerProcess; // last process gets the remainder
	}
	printf("Process %d: %d blocks\n", pid, idealBlocksPerProcess);

	int bitsToProcess = 0;
	for (int i = 0; i < idealBlocksPerProcess; i++) {
		bitsToProcess += blockLengths[pid * idealBlocksPerProcess + i];
	}
	int bitsToSkip = 0;
	for (int i = 0; i < pid * idealBlocksPerProcess; i++) {
		bitsToSkip += blockLengths[i];
	}
	int startBit = treeByteSize*8 + sizeof(EncodedFileHeader)*8 + bitsToSkip; 
	int endBit = treeByteSize*8 + sizeof(EncodedFileHeader)*8 + bitsToSkip + bitsToProcess;

	printf("Process %d: %d - %d\n", pid, startBit, endBit);

	// fseek works with bytes, not bits,
	// so we first adjust the file cursor to the nearest byte, then to the bit
	int startByte = startBit / 8;
	int remainderBits = startBit % 8;
	fseek(fp2, 0, SEEK_SET);
	fseek(fp2, startByte, SEEK_SET);
	int numberOfChars = 0;
	char* decodedText = decode_from_file(fp2, root, endBit - startBit, remainderBits, &numberOfChars);

	printf("Process %d: %d chars\n", pid, numberOfChars);
	printf("Process %d: %s\n", pid, decodedText);

	MPI_Finalize();

	return 0;
}
