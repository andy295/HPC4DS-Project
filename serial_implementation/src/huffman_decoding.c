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
		if (blockSize % BITS_IN_BYTE != 0)
			blockSize = roundUp(blockSize, BITS_IN_BYTE);

		// convert to bytes 
		blockSize /= BITS_IN_BYTE;
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
	double time_spent = 0.0;
	clock_t t_begin = clock();

	printf("\nDecoding\n");

	FILE *fp = openFile(ENCODED_FILE, READ_B, 0);
	if (fp == NULL) {
		fprintf(stderr, "Error opening file %s\n", ENCODED_FILE);
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

	int fileSize = getFileSize(ENCODED_FILE);
	int number_of_blocks = (fileSize - header.byteStartOfDimensionArray) / sizeof(unsigned short);
	printf("\nTotal number of blocks: %d\n", number_of_blocks);
	printf("File size: %d\n", fileSize);

	unsigned short *dimensions = malloc(sizeof(unsigned short) * number_of_blocks);
	parseBlockLengths(dimensions, fp, number_of_blocks, header.byteStartOfDimensionArray);

	for (int i = 0; i < number_of_blocks; i++)
		printf("dimensions[%d]: %d\n", i, dimensions[i]);

	DecodingText decodingText = {.length = 0, .decodedText = NULL};

	int start = 0;
	int end = 0;
	calculateBlockRange(number_of_blocks, 1, 0, &start, &end);
	printf("Process %d - Block range: %d - %d - Blocks nr: %d\n", 0, start, end - 1, end - start);

	int startPos = (sizeof(FileHeader) * FILE_HEADER_ELEMENTS) + treeByteSize;
	startPos += (0 != 0) ? calculatePrevTextSize(dimensions, start) : 0;
	decodingText.decodedText = decodeFromFile(
		startPos,
		dimensions,
		start,
		end - start,
		fp,
		root);

	decodingText.length = strlen(decodingText.decodedText) + 1;

	printf("\nDecoded text:\n%s\n\n", decodingText.decodedText);

	freeTree(root);

	clock_t t_end = clock();

	// calculate elapsed time by finding difference (end - begin) and
	// dividing the difference by CLOCKS_PER_SEC to convert to seconds
	time_spent += (double)(t_end - t_begin) / CLOCKS_PER_SEC;

	printf("The elapsed time is %f seconds\n", time_spent);

	return 0;
}
