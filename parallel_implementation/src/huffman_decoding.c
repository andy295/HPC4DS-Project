#include "include/huffman_decoding.h"

TreeNode* readTreeFromFile(FILE* fp) {
	TreeNode* node = malloc(sizeof(TreeNode));
	node->character = fgetc(fp);
	node->frequency = 10; // not used
	node->leftChild = NULL;
	node->rightChild = NULL;

	char left = fgetc(fp);
	char right = fgetc(fp);
	if (left) {
		node->leftChild = readTreeFromFile(fp);
	} 
	
	if (right) {
		node->rightChild = readTreeFromFile(fp);
	}

	return node;
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
	printf("Process %d: \n", pid);

	if (pid != 0) {

	} else {
		printHuffmanTree(root, 0);
	}

	MPI_Finalize();

	return 0;
}
