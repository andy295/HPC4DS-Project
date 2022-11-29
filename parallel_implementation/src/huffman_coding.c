#include "include/huffman_coding.h"

// void encode_to_file(char* text, struct LetterEncoding* encodings, int unique_letters, int total_letters){

// 	FILE *fp;
// 	fp = fopen(ENCODED_FILE, "wb");

// 	int charIndex = 0;
// 	char c = 0;
// 	for (int i = 0; i < total_letters; i++) {
// 		for (int j = 0; j < unique_letters; j++) {
// 			if (text[i] == encodings[j].letter) {
// 				append_string_to_binary_file(encodings[j].encoding, fp, &charIndex, &c);
// 			}
// 		}
// 	}

// 	// appends and writes custom end of file character
// 	append_string_to_binary_file(encodings[unique_letters-1].encoding, fp, &charIndex, &c);
// 	fwrite(&c, sizeof(char), 1, fp);

// 	fclose(fp);
// }

// void decode_from_file(struct TreeNode* root){

// 	FILE *fp2;
// 	fp2 = fopen(ENCODED_FILE, "rb");

// 	char c;
// 	char lastContinuousChar = 0;

// 	struct TreeNode* intermediateNode = root;

// 	int endReached = 0;
// 	while (fread(&c, sizeof(char), 1, fp2)) {

// 		if (endReached == 1) {
// 			break;
// 		}

// 		for (int i = 0; i < 8; i++) {
// 			if (intermediateNode->letter != '$') {

// 				if (intermediateNode->letter == '#'){
// 					endReached = 1;
// 					break;
// 				}

// 				if (VERBOSE){
// 					printf("%c", intermediateNode->letter);
// 				}

// 				intermediateNode = root;
// 			}

// 			if (c & (1 << i)) {
// 				intermediateNode = intermediateNode->rightChild;
// 			} else {
// 				intermediateNode = intermediateNode->leftChild;
// 			}
// 		}
// 	}

// 	fclose(fp2);
// }

void freeBuffer(void* buffer) {
	if (buffer != NULL)
		free(buffer);
}

void get_encoding_from_tree(CharFreqDictionary* dict, TreeNode* root, CharEncoding* encodings){
	for (int i = 0; i < dict->number_of_chars; i++){

		encodings[i].character = dict->charFreqs[i].character;
		encodings[i].encoding = malloc(sizeof(char) * dict->number_of_chars);

		find_encoding(dict->charFreqs[i].character, root, encodings[i].encoding, 0);
	}
}

bool find_encoding(char character, TreeNode* root, char* dst, int depth) {

	bool found = false;
	if (root->character == character) {
		dst[depth] = '\0';
		return true;
	} else {
		if (root->leftChild != NULL) {
			dst[depth] = '0';
			found = find_encoding(character, root->leftChild, dst, depth+1);
		}

		if (found == 0 && root->rightChild != NULL){
			dst[depth] = '1';
			found = find_encoding(character, root->rightChild, dst, depth+1);
		}
	}

	return found;
}

int main() {
	MPI_Init(NULL, NULL);

	int proc_number;
	int pid; 

	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	// get the processes' portion of text
	char *text = NULL;
	long total_text_length = read_file(SRC_FILE, &text, pid, proc_number);

	// get characters frequencies for the processes' portion of text
	CharFreqDictionary allChars = {.number_of_chars = 0, .charFreqs = NULL};
	getCharFreqsFromText(&allChars, text, total_text_length, pid);

	if (pid != 0) {
		// send CharFreqDictionary to master process
		MsgDictionary* msgDictSnd = createMsgDictionaryFromFreqs(&allChars);
		// printMessageHeader(&msgDictSnd);
		// serialize the message
		int bufferSize = sizeof (MsgDictionary) + sizeof(CharFreq) * msgDictSnd->charsNr;
		BYTE *buffer = createMessageBufferFromDict(msgDictSnd, bufferSize);
		// maybe we could use the send version that uses the mpi buffer 
		// in this way we can empty the msgDict.charFreqs without risks
		MPI_Send(buffer, bufferSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD);

		freeBuffer(buffer);
		freeBuffer(msgDictSnd->charFreqs);
	} else {
		// master process receives data from all the slaves processes
	 	for (int i = 1; i < proc_number; i++) {
			MPI_Status status;
			// probe for an incoming message from process zero
			MPI_Probe(i, 0, MPI_COMM_WORLD, &status);

			// when probe returns, the status object has the size and other
		    // attributes of the incoming message
			// get the message size
			int bufferSize;
		    MPI_Get_count(&status, MPI_BYTE, &bufferSize);

			// allocate a buffer to hold the incoming numbers
    		BYTE *buffer = malloc(sizeof(BYTE) * bufferSize);
			
			// now receive the message with the allocated buffer
			MPI_Recv(buffer, bufferSize, MPI_BYTE, i, 0, MPI_COMM_WORLD, &status);

			// deserialize the message
			MsgDictionary msgRcv;
			createMsgDictFromByteBuffer(&msgRcv, buffer); 
			// printMessageHeader(&msgRcv);
			// add the received charFreqs to the dictionary
			mergeCharFreqs(&allChars, msgRcv.charFreqs, msgRcv.charsNr);
			sortFreqs(&allChars);

			freeBuffer(buffer);
			freeBuffer(msgRcv.charFreqs);
		}

		printCharFreqs(&allChars);

	}

	// 	// send encoding table to all processes
	// 	for (int i = 1; i < NUM_OF_PROCESSES; i++) {
	// 		MPI_Send(encodings, sizeof(struct LetterEncoding) * allLetters->number_of_letters, MPI_BYTE, i, 0, MPI_COMM_WORLD);
	// 	}
	// }

	// // all processes receive the encoding table
	// if (pid != 0) {
	// 	struct LetterEncoding* encodings = malloc(sizeof(struct LetterEncoding) * allLetters->number_of_letters);
	// 	MPI_Recv(encodings, sizeof(struct LetterEncoding) * allLetters->number_of_letters, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// 	// noi abbiamo fatto solo encode to file, ma servirebbe un encode to byte array,
	// 	// che poi viene mandato al master process, che lo scrive su file (questa riga di commento è stata scritta da copilot)

	// 	// encode the processes' portion of text to buffer
	// 	char* buffer = encode_to_buffer(subtext, encodings, allLetters->number_of_letters);

	// 	// add to buffer sync character
	// 	buffer = strcat(buffer, encodings[allLetters->number_of_letters-1].encoding); //TODO: check if this is correct

	// 	// send the buffer to the master process
	// 	MPI_Send(buffer, strlen(buffer), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
	// }

	// // master process receives all buffers
	// if (pid == 0) {
	// 	FILE *fp;
	// 	fp = fopen(ENCODED_FILE, "wb");

	// 	// master writes his own buffer to file
	// 	fwrite(buffer, sizeof(char), strlen(buffer), fp);

	// 	for (int i = 1; i < NUM_OF_PROCESSES; i++) {
	// 		// receive buffer
	// 		char* buffer = malloc(sizeof(char) * total_text_length);
	// 		MPI_Recv(buffer, total_text_length, MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

	// 		// write buffer to file
	// 		fwrite(buffer, sizeof(char), strlen(buffer), fp);
	// 	}

	// 	fclose(fp);
	// }

	// DECODING
	// decode_from_file(root);

	// if (VERBOSE){
	// 	struct timespec end = get_time();
	// 	printf("\nDecoding time: %f seconds\n", get_execution_time(start, end));
	// }

	// printf("\nCompression stats: \n");
	// printf("Original file size: %d bits\n", get_file_size("text.txt"));
	// printf("Compressed file size: %d bits\n", get_file_size("output"));
	// printf("Compression rate: %.2f%%\n", (1 - (double)get_file_size("output") / (double)get_file_size("text.txt")) * 100);

	// printf("\n");

	freeBuffer(allChars.charFreqs);
	freeBuffer(text);

	MPI_Finalize();

	return 0;
}
