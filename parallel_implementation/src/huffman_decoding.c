#include "include/huffman_decoding.h"

void decode_from_file(FILE* fp2, TreeNode* root){
	char c;
	TreeNode* intermediateNode = root;

	int endReached = 0;
	while (fread(&c, sizeof(char), 1, fp2)) {

		if (endReached == 1) {
			break;
		}

		for (int i = 0; i < 8; i++) {
			if (intermediateNode->character != '$') {

				if (intermediateNode->character == '#'){
					endReached = 1;
					break;
				}

				printf("%c", intermediateNode->character);
			

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


typedef struct EncodedFileHeader {
	short number_of_blocks;
	//short chars_per_block; 
	short *block_lengths;
} EncodedFileHeader;

EncodedFileHeader* readHeaderFromFile(FILE* fp) {
	EncodedFileHeader* header = malloc(sizeof(EncodedFileHeader));
	fread(&header->number_of_blocks, sizeof(short), 1, fp);
	//fread(&header->chars_per_block, sizeof(short), 1, fp);

	header->block_lengths = malloc(sizeof(short) * header->number_of_blocks);
	fread(header->block_lengths, sizeof(short), header->number_of_blocks, fp);

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

	TreeNode* root = readTreeFromFile(fp2);

	int nodes = countTreeNodes(root);
	int treeByteSize = nodes * sizeof(TreeArrayItem);

	fseek(fp2, 0, SEEK_SET);
	fseek(fp2, treeByteSize, SEEK_SET);
	EncodedFileHeader *header = readHeaderFromFile(fp2);
	int headerByteSize = 2*sizeof(short) + header->number_of_blocks * sizeof(short);

	int idealBlocksPerProcess = header->number_of_blocks / proc_number;
	if (pid == proc_number - 1) {
		idealBlocksPerProcess += header->number_of_blocks % proc_number;
	}
	printf("Process %d: %d blocks\n", pid, idealBlocksPerProcess);

	int bitsToProcess = 0;
	for (int i = 0; i < idealBlocksPerProcess; i++) {
		bitsToProcess += header->block_lengths[pid * idealBlocksPerProcess + i];
	}
	int bitsToSkip = 0;
	for (int i = 0; i < pid * idealBlocksPerProcess; i++) {
		bitsToSkip += header->block_lengths[i];
	}
	int startBit = treeByteSize*8 + headerByteSize*8 + bitsToSkip; 
	int endBit = treeByteSize*8 + headerByteSize*8 + bitsToSkip + bitsToProcess;

	printf("Process %d: %d - %d\n", pid, startBit, endBit);

	//fseek(fp2, 0, SEEK_SET);
	//fseek(fp2, treeByteSize + headerByteSize, SEEK_SET);
	//decode_from_file(fp2, root);
//
	//if (pid == 0) {
	//	printHuffmanTree(root, 0);
	//}

	MPI_Finalize();

	return 0;
}
