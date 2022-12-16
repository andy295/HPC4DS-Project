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
		if (buffer != NULL && bufferSize > 0)
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

		//printEncodings(&encodingsDict);

		// send the complete encoding table to each process
		// and each one encodes its portion of the text
		int bufferSize = 0;
		BYTE *buffer = getMessage(&encodingsDict, MSG_ENCODING_DICTIONARY, &bufferSize);
		if (buffer != NULL && bufferSize > 0)
			for (int i = 1; i < proc_number; i++)
				MPI_Send(buffer, bufferSize, MPI_BYTE, i, 0, MPI_COMM_WORLD);
		else
			// eventually print an error message
			printf("Error while sending encoding dictionary to the other processes\n");

		freeBuffer(buffer);

		int byteSizeOfTree; 
		BYTE* encodedTree = encodeTreeToByteArray(root->item, &byteSizeOfTree);

		// write the encoded tree to the file
		writeBufferToFile(ENCODED_FILE, encodedTree, byteSizeOfTree, true);
		printf("Encoded tree size: %d\n", getByteSizeOfTree(root->item));

		// encode the text for process 0
		EncodingText encodingText = {.nr_of_pos = 0, .nr_of_bytes = 0, .nr_of_bits = 0, .positions = NULL, .encodedText = NULL};
		encodeStringToByteArray(&encodingText, &encodingsDict, text, processes_text_length);
		writeBufferToFile(ENCODED_FILE, encodingText.encodedText, encodingText.nr_of_bytes, false);

		// write the start positions of each encoded block to the file
		// we have to wait before writing the header because we don't know the positions coming from the other processes
		int byteSizeOfHeader = encodingText.nr_of_pos * sizeof(short);
		writeBufferToFile(ENCODED_FILE, (BYTE*)encodingText.positions, byteSizeOfHeader, false);
		printf("Header size: %d\n", byteSizeOfHeader);

		// write the encoded text to file
		writeBufferToFile(ENCODED_FILE, encodingText.encodedText, encodingText.nr_of_bytes, false);

		//printEncodings(&encodingsDict);
	}

	EncodingText encodingText = {.nr_of_pos = 0, .nr_of_bytes = 0, .positions = NULL, .encodedText = NULL};
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

			encodeStringToByteArray(&encodingText, &encodingsDict, text, processes_text_length);

			// send to master process the encoded text
			bufferSize = 0;
			buffer = getMessage(&encodingText, MSG_ENCODING_TEXT, &bufferSize);

			if (buffer != NULL && bufferSize > 0)
				MPI_Send(buffer, bufferSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
			else
				// eventually print an error message
				printf("Error while sending encoded text to process 0\n");

			freeBuffer(buffer);
		}
	}

	if (pid == 0) {
		for (int i = 1; i < proc_number; i++) {
			MPI_Status status;
			int bufferSize = 0;

			BYTE *buffer = prepareForReceive(&status, &bufferSize, i, 0);
			MPI_Recv(buffer, bufferSize, MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);

			setMessage(&encodingText, buffer);	

			writeBufferToFile(ENCODED_FILE, buffer, bufferSize, false);

			freeBuffer(buffer);
		}

		printf("Encoded file size: %d\n", getFileSize(ENCODED_FILE));
		printf("Original file size: %d\n", getFileSize(SRC_FILE));
	}

	takeTime();
	printTime("Time elapsed");
	saveTime(LOG_FILE, "Time elapsed");

	freeBuffer(allChars.charFreqs);
	freeBuffer(text);

	MPI_Finalize();

	return 0;
}
