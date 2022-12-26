#include "include/huffman_decoding.h"

int roundUp(int numToRound, int multiple) {
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

int main() {
	MPI_Init(NULL, NULL);

	int proc_number;
	int pid; 
	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	takeTime(pid);
	
	FILE *fp = openFile(ENCODED_FILE, READ_B, 0);
	if (fp == NULL) {
		fprintf(stderr, "Error opening file %s\n", ENCODED_FILE);
		return 1;
	}

	FileHeader header = {.byteStartOfDimensionArray = 0};
	parseHeader(&header, fp);

	if(DEBUG(pid)){
		printf("Header size: %lu\n", FILE_HEADER_ELEMENTS * sizeof(unsigned int));
		printf("Encoded arrayPosStartPos: %d\n", header.byteStartOfDimensionArray);
	}

	TreeNode *root = malloc(sizeof(TreeNode));
	parseHuffmanTree(root, fp);
	int nodes = countTreeNodes(root);
	int treeByteSize = nodes * sizeof(TreeArrayItem);

	if(DEBUG(pid)){
		printf("Encoded tree size: %d\n", treeByteSize);
		printf("Huffman tree nodes number: %d\n", nodes);
		printHuffmanTree(root, 0);
	}

	int fileSize = getFileSize(ENCODED_FILE);
	int number_of_blocks = (fileSize - header.byteStartOfDimensionArray) / sizeof(unsigned short);
	unsigned short *dimensions = malloc(sizeof(unsigned short) * number_of_blocks);
	parseBlockLengths(dimensions, fp, number_of_blocks, header.byteStartOfDimensionArray);

	if(DEBUG(pid)){
		for (int i = 0; i < number_of_blocks; i++)
			printf("dimensions[%d]: %d\n", i, dimensions[i]);
	}

	int start = 0;
	int end = 0;
	calculateBlockRange(number_of_blocks, proc_number, pid, &start, &end);
	printf("Process %d - Block range: %d - %d - Blocks nr: %d\n", pid, start, end - 1, end - start);

	int startPos = (sizeof(FileHeader) * FILE_HEADER_ELEMENTS) + treeByteSize;
	startPos += (pid != 0) ? calculatePrevTextSize(dimensions, start) : 0;

	char *decodedText = decodeFromFile(
		startPos,
		dimensions,
		start,
		end - start,
		fp,
		root);

	if (pid != 0) {
		int bufferSize = 0;
		BYTE *buffer = getMessage(decodedText, MSG_TEXT, &bufferSize);
		if (buffer != NULL && bufferSize > 0)
			// maybe we could use the send version that uses the mpi buffer 
			// in this way we can empty the msgDict.charFreqs without risks
			MPI_Send(buffer, bufferSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
		else
			fprintf(stderr, "Error while sending %s message to the master process\n", getMsgName(MSG_TEXT));

		freeBuffer(buffer);
	} else {
		DecodingText decodingText = {.length = strlen(decodedText) + 1, .decodedText = decodedText};

		for (int i = 1; i < proc_number; i++) {
			MPI_Status status;
			int bufferSize = 0;

			BYTE *buffer = prepareForReceive(&status, &bufferSize, i, 0);
			MPI_Recv(buffer, bufferSize, MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);

			DecodingText rcvText = {.length = 0, .decodedText = NULL};
			setMessage(&rcvText, buffer);

			mergeDecodedText(&decodingText, &rcvText);

			freeBuffer(rcvText.decodedText);
			freeBuffer(buffer);
		}

		printf("\nDecoded text:\n%s\n", decodingText.decodedText);
		decodedText = decodingText.decodedText;
	}

	freeBuffer(decodedText);
	freeBuffer(dimensions);
	freeTree(root);

	fclose(fp);

	takeTime(pid);
	printTime(pid, "Time elapsed");
	saveTime(pid, LOG_FILE, "Time elapsed");

	MPI_Finalize();

	return 0;
}
