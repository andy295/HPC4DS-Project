#include "include/huffman_coding.h"

static int charsPerBlock = 125;

void unorderedSendRecv(int proc_number, int pid, CharFreqDictionary *dict, MPI_Datatype *charFreqDictType, bool withMaster) {
    int half = proc_number / 2;

    if (pid >= (half + (withMaster ? 0 : 1))) {
        if (pid != 0) {
			MsgHeader header = {.id = MSG_DICTIONARY, .size = 0, .type = charFreqDictType, .position = 0};
			BYTE *buffer = getMessage(&header, dict);

			if (buffer == NULL || header.size <= 0) {
				fprintf(stderr, "Process %d: Error while creating message %s\n", pid, getMsgName(header.id));
				return;
			}

			int receiver = pid - half;
			MPI_Send(buffer, header.position, MPI_PACKED, receiver, 0, MPI_COMM_WORLD);

			freeBuffer(buffer);
        }
    } else {
        if (pid != 0 || (pid == 0 && withMaster)) {
			MPI_Status status;
			CharFreqDictionary rcvCharFreq = {.number_of_chars = 0, .charFreqs = NULL};
			MsgProbe probe = {
				.header.id = MSG_DICTIONARY,
				.header.size = 0,
				.header.type = charFreqDictType,
				.header.position = 0,
				.pid = pid + half,
				.tag = 0};
			BYTE *buffer = prepareForReceive(&probe, &status);

			MPI_Recv(buffer, probe.header.size, MPI_PACKED, probe.pid, probe.tag, MPI_COMM_WORLD, &status);
			setMessage(&probe.header, &rcvCharFreq, buffer);

			mergeCharFreqs(dict, &rcvCharFreq, LAST_R);

			freeBuffer(buffer);
			freeBuffer(rcvCharFreq.charFreqs);
        }

        if (half % 2 != 0 && withMaster)
            --half;
        else if (half % 2 != 0 && !withMaster) {
            ++half;
            withMaster = true;
        }

        unorderedSendRecv(half, pid, dict, charFreqDictType, withMaster);
    }
}

int main(int argc, char *argv[]) {
	MPI_Init(NULL, NULL);

	int proc_number;
	int pid; 

	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	int thread_number = stringToInt(argv[1]);
	if (thread_number <= 0 || thread_number > MAX_THREADS) {
		fprintf(stderr, "Invalid number of threads: %d\n", thread_number);
		return 1;
	}

	if (argc > 2)
		charsPerBlock = stringToInt(argv[2]);
	
	if (charsPerBlock <= 0 || charsPerBlock > MAX_CHARS_PER_BLOCK) {
		fprintf(stderr, "Invalid number of characters per block: %d\n", charsPerBlock);
		return 1;
	}

	omp_set_dynamic(0);
	omp_set_num_threads(thread_number);

	initDataLogger(MASTER_PROCESS, (pid == MASTER_PROCESS) ? true : false, ENCODING);

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

	// printf("Process %d: %ld characters read\n", pid, processes_text_length);
	addLogData(pid, intToString(proc_number));
	addLogData(pid, intToString(thread_number));
	addLogData(pid, intToString(processes_text_length));
	addLogData(pid, intToString(charsPerBlock)); 

	timeCheckPoint(pid, "Read File");

	getCharFreqsFromText(&allChars, text, processes_text_length, pid);

	timeCheckPoint(pid, "Get Char Frequencies");

	if (proc_number > 1) {
		MPI_Datatype charFreqDictType;
		buildDatatype(MSG_DICTIONARY, &charFreqDictType);

		bool withMaster = true;
		int np = proc_number;
		if (np % 2 != 0) {
			--np;
			withMaster = false;
		}

		unorderedSendRecv(np, pid, &allChars, &charFreqDictType, withMaster);

		MPI_Type_free(&charFreqDictType);
	}

	if (pid == 0) {
		timeCheckPoint(pid, "Merge Char Frequencies");

		oddEvenSort(&allChars);

		timeCheckPoint(pid, "Sort Char Frequencies");

		// creates the huffman tree
		root = createHuffmanTree(&allChars);

		timeCheckPoint(pid, "Create Huffman Tree");

		// creates the encoding dictionary
		getEncodingFromTree(&encodingDict, &allChars, root->item);

		timeCheckPoint(pid, "Get Encoding from Tree");
	}

	if (proc_number > 1) {
		MPI_Datatype charEncDictType;
		buildDatatype(MSG_ENCODING_DICTIONARY, &charEncDictType);

		// send/receive the complete encoding table
	#if ENCODING_DICTIONARY_STR == 0
		MPI_Bcast(&encodingDict.number_of_chars , 1, MPI_INT, 0, MPI_COMM_WORLD);

		if (pid != 0)
			encodingDict.charEncoding = calloc(encodingDict.number_of_chars, sizeof(CharEncoding));

		for (int i = 0; i < encodingDict.number_of_chars; i++) {
			MPI_Bcast(&encodingDict.charEncoding[i], 1, charEncDictType, 0, MPI_COMM_WORLD);

			if (pid != 0)
				encodingDict.charEncoding[i].encoding = calloc(encodingDict.charEncoding[i].length, sizeof(char));

			MPI_Bcast(encodingDict.charEncoding[i].encoding, encodingDict.charEncoding[i].length, MPI_CHAR, 0, MPI_COMM_WORLD);
		}
	#elif ENCODING_DICTIONARY_STR == 1
		MsgHeader header = {.id = MSG_ENCODING_DICTIONARY, .size = 0, .type = &charEncDictType, .position = 0};

		BYTE *buffer = NULL;

		if (pid == 0)
			buffer = getMessage(&header, &encodingDict);

		MPI_Bcast(&header.size, 1, MPI_INT, 0, MPI_COMM_WORLD);

		if (pid != 0)
			buffer = calloc(header.size, sizeof(BYTE));

		MPI_Bcast(buffer, header.size, MPI_PACKED, 0, MPI_COMM_WORLD);

		if (pid != 0)
			setMessage(&header, &encodingDict, buffer);

		freeBuffer(buffer);
	#endif

		MPI_Type_free(&charEncDictType);
	}

	encodeStringToByteArray(&encodingText, &encodingDict, text, processes_text_length, charsPerBlock);

	timeCheckPoint(pid, "Encode Single Text");

	if (proc_number > 1) {
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

			timeCheckPoint(pid, "Merge Encoded Texts");
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

	timeCheckPoint(pid, "Write Encoded Text");

	int initialFileSize = getFileSize(SRC_FILE); 
	int encodedFileSize = getFileSize(ENCODED_FILE); 

	float compressionRatio = ((float)encodedFileSize)*100 / initialFileSize; 

	addLogData(pid, floatToString(compressionRatio)); 

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
