#include "include/huffman_decoding.h"

static const int NUM_OF_PROCESSES_DEC = 3;

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

	*start = (pid * quotient);
	if (quoto != 0 && pid > 0)
		*start += (pid > quoto) ? quoto : pid;

	*end = *start + quotient;
	if (quoto != 0 && (pid == 0 || pid < quoto))
		++(*end);
		
	// printf("cycle %d: start: %d - end: %d = %d\n", i, *start, *end - 1, *end - *start);
}

int huffman_decoding() {
	FILE *fp;

	printf("\nDecoding\n");

	unsigned short dimensions[10] = {8, 7, 8, 6, 7, 7, 6, 6, 7, 4};

	for (int i = 0; i < NUM_OF_PROCESSES; i++) {
		switch (i) {
			case 0:
				fp = openFile(inputFile, READ_B, 0);

				FileHeader header = {.byteStartOfDimensionArray = 0};
				parseHeader(&header, fp);
				printf("Header size: %lu\n", FILE_HEADER_ELEMENTS * sizeof(unsigned int));
				printf("Encoded arrayPosStartPos: %d\n", header.byteStartOfDimensionArray);

				TreeNode* root = malloc(sizeof(TreeNode));
				parseHuffmanTree(root, fp);
				int nodes = countTreeNodes(root);
				int treeByteSize = nodes * sizeof(TreeArrayItem);
				printf("Encoded tree size: %d\n", treeByteSize);
				printf("Huffman tree nodes number: %d\n", nodes);
				// printHuffmanTree(root, 0);

				int fileSize = getFileSize(inputFile);
				int number_of_blocks = (fileSize - header.byteStartOfDimensionArray) / sizeof(unsigned short);

				int start = 0;
				int end = 0;
				calculateBlockRange(number_of_blocks, NUM_OF_PROCESSES_DEC, 0, &start, &end);
				printf("Process 0 - Block range: %d - %d - Blocks nr: %d\n", start, end - 1, end - start);

				start = 0;
				end = 0;
				calculateBlockRange(number_of_blocks, NUM_OF_PROCESSES_DEC, 1, &start, &end);
				printf("Process 1 - Block range: %d - %d - Blocks nr: %d\n", start, end - 1, end - start);


				start = 0;
				end = 0;
				calculateBlockRange(number_of_blocks, NUM_OF_PROCESSES_DEC, 2, &start, &end);
				printf("Process 2 - Block range: %d - %d - Blocks nr: %d\n", start, end - 1, end - start);

				if (NUM_OF_PROCESSES_DEC == 4) {
					start = 0;
					end = 0;
					calculateBlockRange(number_of_blocks, NUM_OF_PROCESSES_DEC, 3, &start, &end);
					printf("Process 3 - Block range: %d - %d - Blocks nr: %d\n", start, end - 1, end - start);
				}

				// unsigned short *blockLengths = malloc(sizeof(unsigned short) * number_of_blocks);
				// parseBlockLengths(blockLengths, fp, number_of_blocks, sizeof(FileHeader) + treeByteSize);
				// printf("Dimension array size: %ld\n", number_of_blocks * sizeof(short));

				// for (int i = 0; i < number_of_blocks; i++)
				// 	printf("\tdimension [%d] = %d\n", i, blockLengths[i]);

				printf("\nTotal number of blocks: %d\n", number_of_blocks);

				fclose(fp);
				break;
			
			// case 1:
			// 	fp = openFile(encodedFileName, READ_B, 0);

			// 	fclose(fp);
			// 	break;
			
			// case 2:
			// 	fp = openFile(encodedFileName, READ_B, 0);

			// 	fclose(fp);
			// 	break;

			// case 3:
			// 	fp = openFile(encodedFileName, READ_B, 0);

			// 	fclose(fp);
			// 	break;

			default:
				break;
		}
	}

	// float partBlockPerProcess = ((float)number_of_blocks) / proc_number;
	// int idealBlocksPerProcess = (int)partBlockPerProcess + (partBlockPerProcess - (int)partBlockPerProcess > 0 ? 1 : 0); // ceil
	// if (pid == proc_number - 1) {
	// 	idealBlocksPerProcess = number_of_blocks - (proc_number-1)*idealBlocksPerProcess; // last process gets the remainder
	// }
	// printf("Process %d: %d blocks\n", pid, idealBlocksPerProcess);

	// int bitsToProcess = 0;
	// for (int i = 0; i < idealBlocksPerProcess; i++) {
	// 	bitsToProcess += blockLengths[pid * idealBlocksPerProcess + i];
	// }
	// int bitsToSkip = 0;
	// for (int i = 0; i < pid * idealBlocksPerProcess; i++) {
	// 	bitsToSkip += blockLengths[i];
	// }
	// int startBit = treeByteSize*8 + sizeof(FileHeader)*8 + bitsToSkip; 
	// int endBit = treeByteSize*8 + sizeof(FileHeader)*8 + bitsToSkip + bitsToProcess;

	// printf("Process %d: %d - %d\n", pid, startBit, endBit);
	// // fseek works with bytes, not bits,
	// // so we first adjust the file cursor to the nearest byte, then to the bit
	// int startByte = startBit / 8;
	// int remainderBits = startBit % 8;
	// fseek(fp2, 0, SEEK_SET);
	// fseek(fp2, startByte, SEEK_SET);
	// int numberOfChars = 0;
	// char* decodedText = decode_from_file(fp2, root, endBit - startBit, remainderBits, &numberOfChars);

	//printf("Process %d: %d chars\n", pid, numberOfChars);
	//printf("Process %d: %s\n", pid, decodedText);

	return 0;
}
