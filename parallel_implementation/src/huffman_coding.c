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

// void get_encoding_from_tree(CharFreqDictionary* dict, TreeNode* root, CharEncoding* encodings){
// 	for (int i = 0; i < dict->number_of_chars; i++){

// 		encodings[i].character = dict->charFreqs[i].character;
// 		encodings[i].encoding = malloc(sizeof(char) * dict->number_of_chars);

// 		find_encoding(dict->charFreqs[i].character, root, encodings[i].encoding, 0);
// 	}
// }

// bool find_encoding(char character, TreeNode* root, char* dst, int depth) {

// 	bool found = false;
// 	if (root->character == character) {
// 		dst[depth] = '\0';
// 		return true;
// 	} else {
// 		if (root->leftChild != NULL) {
// 			dst[depth] = '0';
// 			found = find_encoding(character, root->leftChild, dst, depth+1);
// 		}

// 		if (found == 0 && root->rightChild != NULL){
// 			dst[depth] = '1';
// 			found = find_encoding(character, root->rightChild, dst, depth+1);
// 		}
// 	}

// 	return found;
// }

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
		MsgCharFreqDictionary* msgDictSnd = createMsgCharFreqDictionaryFromFreqs(&allChars);
		// printMessageHeader(&msgDictSnd);
		int bufferSize = sizeof (MsgCharFreqDictionary) + sizeof(CharFreq) * msgDictSnd->charsNr;
		BYTE *buffer = createMessageBufferFromMsgCharFreqDictionary(msgDictSnd, bufferSize);
		// maybe we could use the send version that uses the mpi buffer 
		// in this way we can empty the msgDict.charFreqs without risks
		MPI_Send(buffer, bufferSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD);

		freeBuffer(buffer);
		freeBuffer(msgDictSnd->charFreqs);
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

			// deserialize the message
			MsgCharFreqDictionary* msgRcv = createMsgCharFreqDictionaryFromByteBuffer(buffer); 
			// printMessageHeader(&msgRcv);
			mergeCharFreqs(&allChars, msgRcv->charFreqs, msgRcv->charsNr);
			sortCharFreqs(&allChars);

			freeBuffer(buffer);
			freeBuffer(msgRcv->charFreqs);

			// LinkedListTreeNodeItem* start = create_linked_list(&allChars);
			TreeNode* root = createHuffmanTree(&allChars); 
			// printHuffmanTree(root, 0);

			CharEncoding* encodings = getEncodingFromTree(&allChars, root); 
			printEncodings(encodings, allChars.number_of_chars);
			// encode_to_file(text, encodings, res->number_of_letters, count); 

			// send encoding table to each process and each one encodes its portion of the text



		}

		// printCharFreqs(&allChars);
	}

	freeBuffer(allChars.charFreqs);
	freeBuffer(text);

	MPI_Finalize();

	return 0;
}
