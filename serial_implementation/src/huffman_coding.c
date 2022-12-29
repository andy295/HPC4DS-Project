#include "include/huffman_coding.h"

int huffman_coding() {
	MPI_Init(NULL, NULL);

	int proc_number;
	int pid; 

	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	// takeTime(pid);
	initDataLogger();
	setDataLoggerReferenceProcess(0);
	addLogColumn(pid, "N.Processes");
	addLogColumn(pid, "N.Characters");
	addLogColumn(pid, "Time");

	char str[128];

	char *text = NULL;
	takeTime(pid);
	sprintf(str, "Pid %d: Time to get chars from text", pid);
	long processes_text_length = readFilePortionForProcess(SRC_FILE, &text, pid, proc_number);
	takeTime(pid);
	printTime(pid, str);

	printf("Process %d: %ld characters read\n", pid, processes_text_length);
	addLogData(pid, intToString(proc_number));
	addLogData(pid, intToString(processes_text_length));

	CharFreqDictionary allChars = {.number_of_chars = 0, .charFreqs = NULL};
	takeTime(pid);
	sprintf(str, "Pid %d: Time to chars frequency", pid);
	getCharFreqsFromText(&allChars, text, processes_text_length, pid);
	takeTime(pid);
	printTime(pid, str);

	CharEncodingDictionary encodingsDict = {.number_of_chars = allChars.number_of_chars, .charEncoding = NULL};
	LinkedListTreeNodeItem *root = NULL;
	EncodingText encodingText = {.nr_of_dim = 0, .nr_of_bytes = 0, .nr_of_bits = 0, .dimensions = NULL, .encodedText = NULL};

	takeTime(pid);
	sprintf(str, "Pid %d: Time to sort charFreq dict", pid);
	sortCharFreqs(&allChars);
	takeTime(pid);
	printTime(pid, str);

	// creates the huffman tree
	takeTime(pid);
	sprintf(str, "Pid %d: Time to create huffman tree", pid);
	root = createHuffmanTree(&allChars);
	takeTime(pid);
	printTime(pid, str);

	takeTime(pid);
	sprintf(str, "Pid %d: Time to create encoding dict", pid);
	getEncodingFromTree(&encodingsDict, &allChars, root->item);
	takeTime(pid);
	printTime(pid, str);

	takeTime(pid);
	sprintf(str, "Pid %d: Time to encode text", pid);
	encodeStringToByteArray(&encodingText, &encodingsDict, text, processes_text_length);
	takeTime(pid);
	printTime(pid, str);

	freeBuffer(allChars.charFreqs);
	freeBuffer(text);

	// write an empty header to the file
	FileHeader fileHeader = {.byteStartOfDimensionArray = 0};
	BYTE *startPos = (BYTE*)&fileHeader;
	writeBufferToFile(ENCODED_FILE, startPos, sizeof(unsigned int) * FILE_HEADER_ELEMENTS, WRITE_B, 0);
	
	if(DEBUG(pid)) {
		printf("Header size: %lu\n", sizeof(unsigned int) * FILE_HEADER_ELEMENTS);
		printf("Encoded arrayPosStartPos: %d\n", fileHeader.byteStartOfDimensionArray);
	}

	// write the encoded tree to the file
	int byteSizeOfTree;
	BYTE *encodedTree = encodeTreeToByteArray(root->item, &byteSizeOfTree);
	writeBufferToFile(ENCODED_FILE, encodedTree, byteSizeOfTree, APPEND_B, 0);

	if (DEBUG(pid))
		printf("Encoded tree size: %d\n", getByteSizeOfTree(root->item));

	int nodes = countTreeNodes(root->item);

	if (DEBUG(pid)) {
		printf("Huffman tree nodes number: %d\n", nodes);
		printHuffmanTree(root->item, 0);
	}

	writeBufferToFile(ENCODED_FILE, encodingText.encodedText, encodingText.nr_of_bytes, APPEND_B, 0);

	if (DEBUG(pid))
		printf("Encoded text size: %d\n", encodingText.nr_of_bytes);

	BYTE *dimensions = (BYTE*)encodingText.dimensions;
	writeBufferToFile(ENCODED_FILE, dimensions, encodingText.nr_of_dim * sizeof(unsigned short), APPEND_B, 0);

	if (DEBUG(pid)) {
		printf("Dimensions array size: %ld\n", encodingText.nr_of_dim * sizeof(unsigned short));
		for (int i = 0; i < encodingText.nr_of_dim; i++)
			printf("\tdimension[%d] = %d\n", i, encodingText.dimensions[i]);

		// printEncodedText(encodingText.encodedText, encodingText.nr_of_bytes);
		// printf("\n");
	}

	fileHeader.byteStartOfDimensionArray = sizeof(FileHeader) + byteSizeOfTree + encodingText.nr_of_bytes;
	startPos = (BYTE*)&fileHeader;
	writeBufferToFile(ENCODED_FILE, startPos, sizeof(unsigned int) * FILE_HEADER_ELEMENTS, WRITE_B_AT, 0);

	if(DEBUG(pid)) {
		printf("Total number of blocks: %d\n", encodingText.nr_of_dim);
		printf("Encoded file size: %d\n", getFileSize(ENCODED_FILE));
		printf("Original file size: %d\n", getFileSize(SRC_FILE));
	}

	freeBuffer(encodingText.dimensions);
	freeBuffer(encodingText.encodedText);
	freeLinkedList(root);

	// takeTime(pid);
	// printTime(pid, "Time elapsed");
	// saveTime(pid, TIME_LOG_FILE, "Time elapsed");

	float time = getTime(pid, "Time elapsed");
	addLogData(pid, floatToString(time));

	MPI_Finalize();

	return 0;
}
