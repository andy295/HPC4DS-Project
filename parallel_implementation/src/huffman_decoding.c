#include "include/huffman_decoding.h"

void decode_from_file(struct TreeNode* root){

	FILE *fp2;
	fp2 = fopen(ENCODED_FILE, "rb");

	char c;
	struct TreeNode* intermediateNode = root;

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

	if (pid != 0) {

	} else {
		printHuffmanTree(root, 0);

		decode_from_file(root);
	}

	MPI_Finalize();

	return 0;
}
