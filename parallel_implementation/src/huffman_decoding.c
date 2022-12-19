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

void calculateBlockRange(int nrOfBlocks, int nrOfProcs, int pid, int *start, int *end) {
    int quotient = nrOfBlocks / nrOfProcs;
    int quoto =	nrOfBlocks % nrOfProcs;

    // printf("result: %d - rest: %d\n", result, rest);

	*start = (pid * quotient) + 1;
	if (quoto != 0 && pid > 0) {
		if (pid > quoto)
			++(*start);
		else
			*start += pid;
	}

	*end = *start + quotient;
	if (quoto != 0 && (pid == 0 || pid < quoto))
		++(*end);
		
	// printf("cycle %d: start: %d - end: %d = %d\n", i, *start, *end - 1, *end - *start);
}

int main() {
	MPI_Init(NULL, NULL);

	int proc_number;
	int pid; 

	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);
	
	if (pid == 0) {
		FILE *fp2;
		fp2 = openFile(ENCODED_FILE, READ_B, 0);

		FileHeader *header = parseHeader(fp2);
		printf("Encoded arrayPosStartPos: %d\n", header->byteSizeOfPositionArray);
		int number_of_blocks = (getFileSize(ENCODED_FILE) - header->byteSizeOfPositionArray) / sizeof(unsigned short);
		printf("Number of blocks: %d\n", number_of_blocks); 

		fseek(fp2, 0, SEEK_SET);
		fseek(fp2, sizeof(FileHeader), SEEK_SET);
		TreeNode* root = parseHuffmanTree(fp2);
		int nodes = countTreeNodes(root);
		int treeByteSize = nodes * sizeof(TreeArrayItem);

		fseek(fp2, 0, SEEK_SET);
		fseek(fp2, header->byteSizeOfPositionArray, SEEK_SET);
		unsigned short *blockLengths = parseBlockLengths(fp2, number_of_blocks);

		if (pid == 0) {
			for (int i = 0; i < number_of_blocks; i++) {
				printf("Block %d: %d byte\n", i, blockLengths[i]);
			}
		}

		float partBlockPerProcess = ((float)number_of_blocks) / proc_number;
		int idealBlocksPerProcess = (int)partBlockPerProcess + (partBlockPerProcess - (int)partBlockPerProcess > 0 ? 1 : 0); // ceil
		if (pid == proc_number - 1) {
			idealBlocksPerProcess = number_of_blocks - (proc_number-1)*idealBlocksPerProcess; // last process gets the remainder
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
		int startBit = treeByteSize*8 + sizeof(FileHeader)*8 + bitsToSkip; 
		int endBit = treeByteSize*8 + sizeof(FileHeader)*8 + bitsToSkip + bitsToProcess;

		printf("Process %d: %d - %d\n", pid, startBit, endBit);

		// fseek works with bytes, not bits,
		// so we first adjust the file cursor to the nearest byte, then to the bit
		int startByte = startBit / 8;
		int remainderBits = startBit % 8;
		fseek(fp2, 0, SEEK_SET);
		fseek(fp2, startByte, SEEK_SET);
		int numberOfChars = 0;
		char* decodedText = decode_from_file(fp2, root, endBit - startBit, remainderBits, &numberOfChars);

		//printf("Process %d: %d chars\n", pid, numberOfChars);
		//printf("Process %d: %s\n", pid, decodedText);

	}

	MPI_Finalize();

	return 0;
}
