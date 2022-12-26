#include "include/huffman_decoding.h"

int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;

    int remainder = abs(numToRound) % multiple;
    if (remainder == 0)
        return numToRound;

    if (numToRound < 0)
        return -(abs(numToRound) - remainder);
    else
        return numToRound + multiple - remainder;
}

int calculatePrevTextSize(unsigned short *dimensions, int nrOfBlocks) {
	int prevTextSize = 0;

	for (int i = 0; i < nrOfBlocks; i++) {
		// size of block in bits
		int blockSize = dimensions[i]; 

		// round up to multiple of 8
		if (blockSize % BIT_8 != 0)
			blockSize = roundUp(blockSize, BIT_8);

		// convert to bytes 
		blockSize /= BIT_8;
		prevTextSize += blockSize;
	}

	return prevTextSize;
}

void calculateBlockRange(int nrOfBlocks, int nrOfProcs, int pid, int *start, int *end) {
    int quotient = nrOfBlocks / nrOfProcs;
    int quoto =	nrOfBlocks % nrOfProcs;

	*start = (pid * quotient);
	if (quoto != 0 && pid > 0)
		*start += (pid > quoto) ? quoto : pid;

	*end = *start + quotient;
	if (quoto != 0 && (pid == 0 || pid < quoto))
		++(*end);
}

int huffman_decoding() {
	printf("\nDecoding\n");

	char *decodedText1 = NULL;
	char *decodedText2 = NULL;

	int bufferSize = 0;

	BYTE *buffer1 = NULL;
	BYTE *buffer2 = NULL;

	FILE *fp = openFile(inputFile, READ_B, 0);
	if (fp == NULL) {
		fprintf(stderr, "Error opening file %s\n", inputFile);
		return 1;
	}

	FileHeader header = {.byteStartOfDimensionArray = 0};
	parseHeader(&header, fp);
	printf("Header size: %lu\n", FILE_HEADER_ELEMENTS * sizeof(unsigned int));
	printf("Encoded arrayPosStartPos: %d\n", header.byteStartOfDimensionArray);

	TreeNode *root = malloc(sizeof(TreeNode));
	parseHuffmanTree(root, fp);
	int nodes = countTreeNodes(root);
	int treeByteSize = nodes * sizeof(TreeArrayItem);
	printf("Encoded tree size: %d\n", treeByteSize);
	printf("Huffman tree nodes number: %d\n", nodes);
	// printHuffmanTree(root, 0);

	int fileSize = getFileSize(inputFile);
	int number_of_blocks = (fileSize - header.byteStartOfDimensionArray) / sizeof(unsigned short);
	printf("\nTotal number of blocks: %d\n", number_of_blocks);
	printf("File size: %d\n", fileSize);

	unsigned short *dimensions = malloc(sizeof(unsigned short) * number_of_blocks);
	parseBlockLengths(dimensions, fp, number_of_blocks, header.byteStartOfDimensionArray);

	for (int i = 0; i < number_of_blocks; i++)
		printf("dimensions[%d]: %d\n", i, dimensions[i]);

	DecodingText decodingText = {.length = 0, .decodedText = NULL};

	for (int i = 0; i < NUM_OF_PROCESSES_DEC; i++) {
		int start = 0;
		int end = 0;
		calculateBlockRange(number_of_blocks, NUM_OF_PROCESSES_DEC, i, &start, &end);
		printf("Process %d - Block range: %d - %d - Blocks nr: %d\n", i, start, end - 1, end - start);

		int startPos = (sizeof(FileHeader) * FILE_HEADER_ELEMENTS) + treeByteSize;
		startPos += (i != 0) ? calculatePrevTextSize(dimensions, start) : 0;

		if (i == 0) {
			decodingText.decodedText = decodeFromFile(
			startPos,
			dimensions,
			start,
			end - start,
			fp,
			root);

			decodingText.length = strlen(decodingText.decodedText) + 1;

			// printf("decodedText0: %s\n", decodingText.decodedText);
		}
		else if (i == 1) {
			decodedText1 = decodeFromFile(
			startPos,
			dimensions,
			start,
			end - start,
			fp,
			root);

			// printf("decodedText1: %s\n", decodedText1);
		}
		else if (i == 2) {
			decodedText2 = decodeFromFile(
			startPos,
			dimensions,
			start,
			end - start,
			fp,
			root);

			// printf("decodedText2: %s\n", decodedText2);
		}

		// send decoded text to master process
		switch (i) {
			case 1:
				bufferSize = 0;
				buffer1 = getMessage(decodedText1, MSG_TEXT, &bufferSize);
				break;
			case 2:
				bufferSize = 0;
				buffer2 = getMessage(decodedText2, MSG_TEXT, &bufferSize);
				break;

			default:
				if (i != 0)
					printf("Process %d unknown\n", i);
		}
	}

	freeBuffer(decodedText1);
	freeBuffer(decodedText2);

	for (int i = 1; i < NUM_OF_PROCESSES_DEC; i++) {
		DecodingText rcvText = {.length = 0, .decodedText = NULL};

		switch (i) {
		case 1:
			setMessage(&rcvText, buffer1);
			// printf("decodedText1:\n%s\n", rcvText.decodedText);
			break;
		case 2:
			setMessage(&rcvText, buffer2);
			// printf("decodedText2:\n%s\n", rcvText.decodedText);
			break;

		default:
			if (i != 0)
				printf("Process %d unknown\n", i);
		}

		mergeDecodedText(&decodingText, &rcvText);

		// printf ("decodedText0 merged with decodedText%d:\n%s\n", i, decodingText.decodedText);

		freeBuffer(rcvText.decodedText);
	}

	printf("\nDecoded text:\n%s\n\n", decodingText.decodedText);

	freeBuffer(buffer1);
	freeBuffer(buffer2);
	freeTree(root);

	return 0;
}
