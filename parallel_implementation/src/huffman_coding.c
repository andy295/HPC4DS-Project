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
	long total_text_length = readFilePortionForProcess(SRC_FILE, &text, pid, proc_number);

	CharFreqDictionary allChars = {.number_of_chars = 0, .charFreqs = NULL};
	getCharFreqsFromText(&allChars, text, total_text_length, pid);

	if (pid != 0) {
		int bufferSize = 0;
		BYTE *buffer = getMessage(&allChars, MSG_DICTIONARY, &bufferSize);
		if (buffer != NULL)
			// maybe we could use the send version that uses the mpi buffer 
			// in this way we can empty the msgDict.charFreqs without risks
			MPI_Send(buffer, bufferSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
		else {
			// eventually print an error message
		}

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

		sortCharFreqs(&allChars);
		appendToCharFreqs(&allChars, '\0', FIRST);
		
		// creates the huffman tree
		LinkedListTreeNodeItem* root = createHuffmanTree(&allChars);

		CharEncodingDictionary encodingsDict = {.number_of_chars = allChars.number_of_chars, .charEncoding = NULL};
		getEncodingFromTree(&encodingsDict, &allChars, root->item);

		// send the complete encoding table to each process
		// and each one encodes its portion of the text
		int bufferSize = 0;
		BYTE *buffer = getMessage(&encodingsDict, MSG_ENCODING, &bufferSize);
		if (buffer != NULL)
			for (int i = 1; i < proc_number; i++)
				MPI_Send(buffer, bufferSize, MPI_BYTE, i, 0, MPI_COMM_WORLD);
		else {
			// eventually print an error message
		}

		freeBuffer(buffer);

		takeTime();
		printTime("Time elapsed");
		saveTime(LOG_FILE, "Time elapsed");
	}

	if (pid != 0) {
		for (int i = 1; i < proc_number; i++) {
			MPI_Status status;
			int bufferSize = 0;

			BYTE *buffer = prepareForReceive(&status, &bufferSize, 0, 0);
			MPI_Recv(buffer, bufferSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);

			CharEncodingDictionary rcvEncodingsDict = {.number_of_chars = 0, .charEncoding = NULL};
			setMessage(&rcvEncodingsDict, buffer);

			freeBuffer(buffer);
		}
	}

	// encode_to_file(text, encodings, res->number_of_letters, count); 

	freeBuffer(allChars.charFreqs);
	freeBuffer(text);

	MPI_Finalize();

	return 0;
}
