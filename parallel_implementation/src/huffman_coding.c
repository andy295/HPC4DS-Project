#include "include/huffman_coding.h"

void freeBuffer(void* buffer) {
	if (buffer != NULL)
		free(buffer);
}

int main() {
	MPI_Init(NULL, NULL);

	int proc_number;
	int pid; 

	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

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

		// reinit the dictionary
		allChars.number_of_chars = 0;
		freeBuffer(allChars.charFreqs);
	} else {
		for (int i = 1; i < proc_number; i++) {
			MPI_Status status;
			MPI_Probe(i, 0, MPI_COMM_WORLD, &status);

			// when probe returns, the status object has the size and other
		    // attributes of the incoming message
			// get the message size
			int bufferSize;
		    MPI_Get_count(&status, MPI_BYTE, &bufferSize);

			// now receive the message with the allocated buffer
    		BYTE *buffer = malloc(sizeof(BYTE) * bufferSize);
			MPI_Recv(buffer, bufferSize, MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);

			CharFreqDictionary rcvChars = {.number_of_chars = 0, .charFreqs = NULL};
			setMessage(&rcvChars, buffer);

			mergeCharFreqs(&allChars, rcvChars.charFreqs, rcvChars.number_of_chars);
			
			freeBuffer(buffer);
		}

		sortCharFreqs(&allChars);

		// send the complete sorted dictionary to each process
		int bufferSize = 0;
		BYTE *buffer = getMessage(&allChars, MSG_DICTIONARY, &bufferSize);
		if (buffer != NULL)
			for (int i = 1; i < proc_number; i++)
				MPI_Send(buffer, bufferSize, MPI_BYTE, i, 0, MPI_COMM_WORLD);
		else {
			// eventually print an error message
		}

		freeBuffer(buffer);
	}

	if (pid != 0) {
		for (int i = 1; i < proc_number; i++) {
			MPI_Status status;
			MPI_Probe(0, 0, MPI_COMM_WORLD, &status);

			// when probe returns, the status object has the size and other
			// attributes of the incoming message
			// get the message size
			int bufferSize;
			MPI_Get_count(&status, MPI_BYTE, &bufferSize);

			// now receive the message with the allocated buffer
			BYTE *buffer = malloc(sizeof(BYTE) * bufferSize);
			MPI_Recv(buffer, bufferSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD, &status);

			setMessage(&allChars, buffer);

			freeBuffer(buffer);
		}
	}

	// creates the huffman tree
	LinkedListTreeNodeItem* root = createHuffmanTree(&allChars);

	// CharEncoding* encodings = getEncodingFromTree(&allChars, root); 
	// printEncodings(encodings, allChars.number_of_chars);
	// encode_to_file(text, encodings, res->number_of_letters, count); 

	// send encoding table to each process and each one encodes its portion of the text

	// printCharFreqs(&allChars);	

	freeBuffer(allChars.charFreqs);
	freeBuffer(text);

	MPI_Finalize();

	return 0;
}
