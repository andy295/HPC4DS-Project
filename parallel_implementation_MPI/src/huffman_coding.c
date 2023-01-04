#include "include/huffman_coding.h"

int main(int argc, char *argv[]) {
	MPI_Init(NULL, NULL);

	int proc_number;
	int pid; 

	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	int thread_count = stringToInt(argv[1]);
	if (thread_count <= 0 || thread_count > MAX_THREADS) {
		fprintf(stderr, "Invalid number of threads: %d\n", thread_count);
		return 1;
	}

	omp_set_dynamic(0);
	omp_set_num_threads(thread_count);

	initDataLogger(MASTER_PROCESS, (pid == MASTER_PROCESS) ? true : false);
	takeTime(pid);

	CharFreqDictionary allChars = {.number_of_chars = 0, .charFreqs = NULL};
	LinkedListTreeNodeItem *root = NULL;
	CharEncodingDictionary encodingDict = {.number_of_chars = 0, .charEncoding = NULL};
	EncodingText encodingText = {.nr_of_dim = 0, .nr_of_bytes = 0, .nr_of_bits = 0, .dimensions = NULL, .encodedText = NULL};

	char *text = NULL;
	long processes_text_length = readFilePortionForProcess(SRC_FILE, &text, pid, proc_number);

	if (processes_text_length <= 0) {
		fprintf(stderr, "Process %d: Error while reading file %s\n", pid, SRC_FILE);
		return 1;
	}

	printf("Process %d: %ld characters read\n", pid, processes_text_length);
	addLogData(pid, intToString(proc_number));
	addLogData(pid, intToString(thread_count));
	addLogData(pid, intToString(processes_text_length));

	getCharFreqsFromText(&allChars, text, processes_text_length, pid);

	MPI_Datatype charFreqDictType;
	buildDatatype(MSG_DICTIONARY, &charFreqDictType);

	// send the character frequencies to the master process
	if (pid != 0) {
		MsgHeader header = {.id = MSG_DICTIONARY, .size = 0, .type = &charFreqDictType, .position = 0};
		BYTE *buffer = getMessage(&header, &allChars);

		if (buffer == NULL || header.size <= 0) {
			fprintf(stderr, "Process %d: Error while creating message %s\n", pid, getMsgName(header.id));
			return 1;
		}

		MPI_Send(buffer, header.position, MPI_PACKED, 0, 0, MPI_COMM_WORLD);

		freeBuffer(buffer);
	} else { // receive the character frequencies dictionary from the other processes
		for (int i = 1; i < proc_number; i++) {
			MPI_Status status;
			CharFreqDictionary rcvCharFreq = {.number_of_chars = 0, .charFreqs = NULL};
			MsgProbe probe = {.header.id = MSG_DICTIONARY, .header.size = 0, .header.type = &charFreqDictType, .header.position = 0, .pid = i, .tag = 0};
			BYTE *buffer = prepareForReceive(&probe, &status);

			MPI_Recv(buffer, probe.header.size, MPI_PACKED, probe.pid, probe.tag, MPI_COMM_WORLD, &status);
			setMessage(&probe.header, &rcvCharFreq, buffer);

			mergeCharFreqs(&allChars, &rcvCharFreq, LAST_R);

			freeBuffer(buffer);
			freeBuffer(rcvCharFreq.charFreqs);
		}

		oddEvenSort(&allChars);

		// creates the huffman tree
		root = createHuffmanTree(&allChars);

		// creates the encoding dictionary
		getEncodingFromTree(&encodingDict, &allChars, root->item);
	}

	MPI_Type_free(&charFreqDictType);

	MPI_Datatype charEncDictType;
	buildDatatype(MSG_ENCODING_DICTIONARY, &charEncDictType);

	// send/receive the complete encoding table
	MPI_Bcast(&encodingDict.number_of_chars , 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (pid != 0)
		encodingDict.charEncoding = calloc(encodingDict.number_of_chars, sizeof(CharEncoding));

	for (int i = 0; i < encodingDict.number_of_chars; i++) {
		MPI_Bcast(&encodingDict.charEncoding[i], 1, charEncDictType, 0, MPI_COMM_WORLD);

		if (pid != 0)
			encodingDict.charEncoding[i].encoding = calloc(encodingDict.charEncoding[i].length, sizeof(char));

		MPI_Bcast(encodingDict.charEncoding[i].encoding, encodingDict.charEncoding[i].length, MPI_CHAR, 0, MPI_COMM_WORLD);
	}

	MPI_Type_free(&charEncDictType);

	encodeStringToByteArray(&encodingText, &encodingDict, text, processes_text_length);

	// send the encoded text to the master process
	if (pid != 0) {
		MsgHeader header = {.id = MSG_ENCODING_TEXT, .size = 0, .type = NULL, .position = 0};
		BYTE *buffer = getMessage(&header, &encodingText);

		if (buffer == NULL || header.size <= 0) {
			fprintf(stderr, "Process %d: Error while creating message %s\n", pid, getMsgName(header.id));
			return 1;
		}

		MPI_Send(buffer, header.position, MPI_PACKED, 0, 0, MPI_COMM_WORLD);

		freeBuffer(buffer);
	} else { // receive the encoded text from each process and store in unique buffer
		for (int i = 1; i < proc_number; i++) {
			MPI_Status status;
			EncodingText rcvEncTxt = {.nr_of_dim = 0, .nr_of_bytes = 0, .nr_of_bits = 0, .dimensions = NULL, .encodedText = NULL};
			MsgProbe probe = {.header.id = MSG_ENCODING_TEXT, .header.size = 0, .header.type = NULL, .header.position = 0, .pid = i, .tag = 0};
			BYTE *buffer = prepareForReceive(&probe, &status);

			MPI_Recv(buffer, probe.header.size, MPI_PACKED, probe.pid, 0, MPI_COMM_WORLD, &status);
			setMessage(&probe.header, &rcvEncTxt, buffer);

			mergeEncodedText(&encodingText, &rcvEncTxt);

			freeBuffer(buffer);
			freeBuffer(rcvEncTxt.dimensions);
			freeBuffer(rcvEncTxt.encodedText);
		}
	}

	// master process writes data into file
	if (pid == 0) {

		// convert tree into a suitable form for writing to file
		int byteSizeOfTree;
		BYTE* encodedTree = encodeTreeToByteArray(root->item, &byteSizeOfTree);
		int nodes = countTreeNodes(root->item);

		// write the header
		FileHeader fileHeader = {.byteStartOfDimensionArray = sizeof(FileHeader) + byteSizeOfTree + encodingText.nr_of_bytes};
		BYTE *startPos = (BYTE*)&fileHeader;
		writeBufferToFile(ENCODED_FILE, startPos, sizeof(unsigned int) * FILE_HEADER_ELEMENTS, WRITE_B, 0);

		if (DEBUG(pid)) {
			printf("Header size: %lu\n", sizeof(unsigned int) * FILE_HEADER_ELEMENTS);
			printf("Encoded arrayPosStartPos: %d\n", fileHeader.byteStartOfDimensionArray);
		}

		// write the huffman tree
		writeBufferToFile(ENCODED_FILE, encodedTree, byteSizeOfTree, APPEND_B, 0);

		if (DEBUG(pid)) {
			printf("Encoded tree size: %d\n", getByteSizeOfTree(root->item));
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
		}

		if (DEBUG(pid)) {
			printf("Total number of blocks: %d\n", encodingText.nr_of_dim);
			printf("Encoded file size: %d\n", getFileSize(ENCODED_FILE));
			printf("Original file size: %d\n", getFileSize(SRC_FILE));
		}
	}

	takeTime(pid);
	printTime(pid, "Time elapsed");
	// saveTime(pid, TIME_LOG_FILE, "Time elapsed");

	float time = getTime(pid, "Time elapsed");
	addLogData(pid, floatToString(time));

	terminateDataLogger();

	freeLinkedList(root);

	for (int i = 0; i < encodingDict.number_of_chars; i++)
		freeBuffer(encodingDict.charEncoding[i].encoding);

	freeBuffer(encodingDict.charEncoding);

	freeBuffer(encodingText.dimensions);
	freeBuffer(encodingText.encodedText);

	freeBuffer(text);
	freeBuffer(allChars.charFreqs);

	MPI_Finalize();

	return 0;
}
