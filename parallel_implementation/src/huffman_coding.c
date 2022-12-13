#include "include/huffman_coding.h"

void freeBuffer(void* buffer) {
	if (buffer != NULL)
		free(buffer);
}

BYTE* prepareForReceive(MPI_Status *status, int *bufferSize, int pid, int tag) {
	MPI_Probe(pid, tag, MPI_COMM_WORLD, status);

	// when probe returns, the status object has the size and other
	// attributes of the incoming message
	// get the message size
	MPI_Get_count(status, MPI_BYTE, bufferSize);

	// now receive the message with the allocated buffer
	BYTE *buffer = malloc(sizeof(BYTE) * (*bufferSize));

	return buffer;
}

int main() {
	MPI_Init(NULL, NULL);

	int proc_number;
	int pid; 

	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	takeTime();

	char *text = NULL;
	long processes_text_length = readFilePortionForProcess(SRC_FILE, &text, pid, proc_number);
	printf("Process %d: %ld characters read in bytes \n", pid, processes_text_length);

	CharFreqDictionary allChars = {.number_of_chars = 0, .charFreqs = NULL};
	getCharFreqsFromText(&allChars, text, processes_text_length, pid);

	CharEncodingDictionary encodingsDict = {.number_of_chars = allChars.number_of_chars, .charEncoding = NULL};

	if (pid != 0) {
		int bufferSize = 0;
		BYTE *buffer = getMessage(&allChars, MSG_DICTIONARY, &bufferSize);
		if (buffer != NULL)
			// maybe we could use the send version that uses the mpi buffer 
			// in this way we can empty the msgDict.charFreqs without risks
			MPI_Send(buffer, bufferSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
		else
			// eventually print an error message
			printf("Error while sending all character frequencies to the master process\n");

		freeBuffer(buffer);
	} else {
		for (int i = 1; i < proc_number; i++) {
			MPI_Status status;
			int bufferSize = 0;

			BYTE *buffer = prepareForReceive(&status, &bufferSize, i, 0);
			MPI_Recv(buffer, bufferSize, MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);

			CharFreqDictionary rcvChars = {.number_of_chars = 0, .charFreqs = NULL};
			setMessage(&rcvChars, buffer);

			mergeCharFreqs(&allChars, rcvChars.charFreqs, rcvChars.number_of_chars);
			
			freeBuffer(buffer);
		}
		printf("Received all character frequencies\n"); 

		sortCharFreqs(&allChars);
		appendToCharFreqs(&allChars, ENDTEXT, FIRST);

		printf("Sorted all character frequencies\n");
		
		// creates the huffman tree
		LinkedListTreeNodeItem* root = createHuffmanTree(&allChars);
		printf("Created the huffman tree\n");
		getEncodingFromTree(&encodingsDict, &allChars, root->item);
		printf("Created the encoding dictionary\n");

		//printHuffmanTree(root->item, 0);

		// send the complete encoding table to each process
		// and each one encodes its portion of the text
		int bufferSize = 0;
		BYTE *buffer = getMessage(&encodingsDict, MSG_ENCODING_DICTIONARY, &bufferSize);
		if (buffer != NULL)
			for (int i = 1; i < proc_number; i++)
				MPI_Send(buffer, bufferSize, MPI_BYTE, i, 0, MPI_COMM_WORLD);
		else
			// eventually print an error message
			printf("Error while sending encoding dictionary to the other processes\n");

		freeBuffer(buffer);

		takeTime();
		printTime("Time elapsed");
		saveTime(LOG_FILE, "Time elapsed");

		int byteSizeOfTree; 
		BYTE* encodedTree = encodeTreeToByteArray(root->item, &byteSizeOfTree);

		writeBufferToFile(ENCODED_FILE, encodedTree, byteSizeOfTree, true);
		printf("Encoded tree size: %d\n", getByteSizeOfTree(root->item));

		// BYTE endblock = ENDBLOCK; 
		// writeBufferToFile(ENCODED_FILE, &endblock, sizeof(BYTE));

		int byteArrayIndex = 0;
		BYTE* encodedText = encodeStringToByteArray(text, &encodingsDict, processes_text_length, &byteArrayIndex);
		writeBufferToFile(ENCODED_FILE, encodedText, byteArrayIndex, false);

		//printEncodings(&encodingsDict);

		printf("Process %d\n", pid); 
		printf("Encoded text length: %d\n", byteArrayIndex);

		for (int i = 0; i < byteArrayIndex; i++) {
			printf("%d ", encodedText[i]);
		}
		printf("\n");

		// problem is that the entire buffer is full of 1s
		// because encodings are wrong, just "", "1" and "11"
	}

	if (pid != 0) {
		for (int i = 1; i < proc_number; i++) {
			MPI_Status status;
			int bufferSize = 0;

			BYTE *buffer = prepareForReceive(&status, &bufferSize, 0, 0);
			MPI_Recv(buffer, bufferSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);

			encodingsDict.number_of_chars = 0;
			setMessage(&encodingsDict, buffer);
			//printEncodings(&rcvEncodingsDict);

			freeBuffer(buffer);

			int byteArrayIndex = 0;
			BYTE* encodedText = encodeStringToByteArray(text, &encodingsDict, processes_text_length, &byteArrayIndex);

			printf("Process %d\n", pid); 
			printf("Encoded text length: %d\n", byteArrayIndex);
			//printf("Encoded text: %s", encodedText);

			// send to master process the encoded text
			MPI_Send(encodedText, byteArrayIndex, MPI_BYTE, 0, 0, MPI_COMM_WORLD);

			freeBuffer(encodedText);
		}
	}

	// encode_to_file(text, encodings, res->number_of_letters, count); 

	if (pid == 0) {
		for (int i = 1; i < proc_number; i++) {
			MPI_Status status;
			int bufferSize = 0;

			BYTE *buffer = prepareForReceive(&status, &bufferSize, i, 0);
			MPI_Recv(buffer, bufferSize, MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);

			writeBufferToFile(ENCODED_FILE, buffer, bufferSize, false);

			freeBuffer(buffer);
		}

		printf("Encoded file size: %d\n", getFileSize(ENCODED_FILE));
		printf("Original file size: %d\n", getFileSize(SRC_FILE));
	}

	freeBuffer(allChars.charFreqs);
	freeBuffer(text);

	MPI_Finalize();

	return 0;
}
