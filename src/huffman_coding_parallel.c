
#define VERBOSE 1
#define MAX_UNIQUE_LETTERS 100
#define ENCODED_FILE "encoded_file"
#define NUM_OF_PROCESSES 4

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

#include "letter_freq.c"
#include "huffman_tree.c"
#include "letter_encoding.c"

#include "utils/time_utils.c"
#include "utils/file_utils.c"
#include "utils/print_utils.c"
#include "utils/sort_utils.c"
#include "utils/string_utils.c"

int find_encoding(char letter, struct TreeNode* root, char* dst, int depth){

	int found = 0; 
	if (root->letter == letter){
		dst[depth] = '\0';
		return 1;  
	} else {
		
		if (root->leftChild != NULL){
			dst[depth] = '0'; 
			found = find_encoding(letter, root->leftChild, dst, depth+1); 
		}

		if (found == 0 && root->rightChild != NULL){
			dst[depth] = '1'; 
			found = find_encoding(letter, root->rightChild, dst, depth+1); 
		}
	}

	return found; 
}

struct LetterEncoding* get_encoding_from_tree(struct LetterFreqDictionary* allLetters, struct TreeNode* root){
	struct LetterEncoding* encodings = malloc(sizeof(struct LetterEncoding) * allLetters->number_of_letters);

	for (int i = 0; i < allLetters->number_of_letters; i++){
		
		encodings[i].letter = allLetters->letterFreqs[i].letter;  
		encodings[i].encoding = malloc(sizeof(char) * allLetters->number_of_letters); 

		find_encoding(allLetters->letterFreqs[i].letter, root, encodings[i].encoding, 0); 
	}

	return encodings;
}

void encode_to_file(char* text, struct LetterEncoding* encodings, int unique_letters, int total_letters){
	
	FILE *fp;
	fp = fopen(ENCODED_FILE, "wb");

	int charIndex = 0; 
	char c = 0;
	for (int i = 0; i < total_letters; i++) {
		for (int j = 0; j < unique_letters; j++) {
			if (text[i] == encodings[j].letter) {
				append_string_to_binary_file(encodings[j].encoding, fp, &charIndex, &c); 
			}
		}
	}

	// appends and writes custom end of file character
	append_string_to_binary_file(encodings[unique_letters-1].encoding, fp, &charIndex, &c); 
	fwrite(&c, sizeof(char), 1, fp);

	fclose(fp);
}

void append_string_to_byte_array(char* string, char* byte_array, int* byteArrayIndex, int* charIndex, char* currentChar){
	int stringLength = strlen(string); 
	for (int k = 0; k < stringLength; k++){
		
		if (string[k] == '1'){
			*currentChar |= 1 << *charIndex;
		}

		if (*charIndex == 7){
			byte_array[*byteArrayIndex] = *currentChar; 
			*byteArrayIndex = *byteArrayIndex + 1;
			*charIndex = 0; 
			*currentChar = 0; 
		} else {
			*charIndex += 1; 
		} 
	}
}

char* encode_to_byte_array(char* text, struct LetterEncoding* encodings, int unique_letters, int total_letters, int* byteArrayIndex){
	
	int charIndex = 0; 
	char c = 0;
	char* encoded_text = malloc(sizeof(char) * total_letters); 

	for (int i = 0; i < total_letters; i++) {
		for (int j = 0; j < unique_letters; j++) {
			if (text[i] == encodings[j].letter) {
				append_string_to_byte_array(encodings[j].encoding, encoded_text, byteArrayIndex, &charIndex, &c); 
			}
		}
	}

	// appends and writes custom end of file character
	append_string_to_byte_array(encodings[unique_letters-1].encoding, encoded_text, byteArrayIndex, &charIndex, &c); 

	return encoded_text; 
}

void decode_from_file(struct TreeNode* root){

	FILE *fp2;
	fp2 = fopen(ENCODED_FILE, "rb");

	char c;
	struct TreeNode* intermediateNode = root;

	int endReached = 0;
	while (fread(&c, sizeof(char), 1, fp2)) {

		if (endReached == 1) {
			break;
		}

		for (int i = 0; i < 8; i++) {
			if (intermediateNode->letter != '$') {

				if (intermediateNode->letter == '#'){
					endReached = 1;
					break;
				}

				if (VERBOSE){
					printf("%c", intermediateNode->letter);
				}

				intermediateNode = root;
			}

			if (c & (1 << i)) {
				intermediateNode = intermediateNode->rightChild;
			} else {
				intermediateNode = intermediateNode->leftChild;
			}
		}
	}

	fclose(fp2);
}

int main() {

	MPI_Init(NULL, NULL);

	int proc_number;
	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);
	int pid;
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	char* text = read_file("text.txt");
	int total_text_length = strlen(text);
	int substring_length = total_text_length / NUM_OF_PROCESSES;

	// already selects the processes' portion of text
	char* subtext = substring(text, pid * substring_length, substring_length);
	// get letter frequencies for the processes' portion of text
	struct LetterFreqDictionary* allLetters = get_letter_freqs(subtext, total_text_length);
	// setup encodings dictionary for all processes
	struct LetterEncoding* encodings = malloc(sizeof(struct LetterEncoding) * allLetters->number_of_letters);

	// send LetterFreqDictionary to master process
	if (pid != 0) {
		// copilot dice di mandare tutto come unico messaggio, con datatype MPI_BYTE
		// io lo farei con mpi struct magari più avanti
		// piccola parentesi, copilot sa anche l'italiano, e ha aiutato nella scrittura di questo commento 
		// della serie, non ho per niente paura
		MPI_Send(allLetters, sizeof(struct LetterFreqDictionary), MPI_BYTE, 0, 0, MPI_COMM_WORLD);
	} else {
		// master process receives all LetterFreqDictionary
		for (int i = 1; i < NUM_OF_PROCESSES; i++) {
			// purtroppo abbiamo ancora un lavoro, qui copilot avrebbe fatto una malloc sbagliata)
			struct LetterFreqDictionary* receivedLetters = init_letter_freq_dictionary(MAX_UNIQUE_LETTERS);
			MPI_Recv(receivedLetters, sizeof(struct LetterFreqDictionary), MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			// merge the received LetterFreqDictionary with the master's one
			merge_letter_freqs(allLetters, receivedLetters);
		}

		// sort the LetterFreqDictionary only in the master process
		sort_freqs(allLetters);

		// append the sync character to the LetterFreqDictionary
		append_to_freq(allLetters, '$', 10000);

		// create the Huffman tree
		struct TreeNode* root = create_huffman_tree(allLetters);

		// get the encodings for each letter
		struct LetterEncoding* encodings = get_encoding_from_tree(allLetters, root);

		// send encoding table to all processes
		for (int i = 1; i < NUM_OF_PROCESSES; i++) {
			MPI_Send(encodings, sizeof(struct LetterEncoding) * allLetters->number_of_letters, MPI_BYTE, i, 0, MPI_COMM_WORLD);
		}
	}

	// all processes receive the encoding table
	if (pid != 0) {
		//struct LetterEncoding* encodings = malloc(sizeof(struct LetterEncoding) * allLetters->number_of_letters);
		MPI_Recv(encodings, sizeof(struct LetterEncoding) * allLetters->number_of_letters, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		// noi abbiamo fatto solo encode to file, ma servirebbe un encode to byte array, 
		// che poi viene mandato al master process, che lo scrive su file (questa riga di commento è stata scritta da copilot)
		
		// encode the processes' portion of text to buffer	
		int byteArrayFinalSize = 0; 
		char* buffer = encode_to_byte_array(text, encodings, allLetters->number_of_letters, substring_length, &byteArrayFinalSize);
		
		// add to buffer sync character
		buffer = strcat(buffer, encodings[allLetters->number_of_letters-1].encoding); //TODO: check if this is correct
		byteArrayFinalSize += 1;

		// send the buffer to the master process
		MPI_Send(buffer, byteArrayFinalSize, MPI_BYTE, 0, 0, MPI_COMM_WORLD);
	}

	// master process receives all buffers
	if (pid == 0) {
		FILE *fp;
		fp = fopen(ENCODED_FILE, "wb");

		// master writes his own buffer to file
		int byteArrayFinalSize = 0; 
		char* buffer = encode_to_byte_array(text, encodings, allLetters->number_of_letters, substring_length, &byteArrayFinalSize);
		fwrite(buffer, sizeof(char), byteArrayFinalSize, fp);

		for (int i = 1; i < NUM_OF_PROCESSES; i++) {
			// receive buffer
			char* recv_buffer = malloc(sizeof(char) * total_text_length);
			MPI_Recv(recv_buffer, total_text_length, MPI_BYTE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			
			// write buffer to file
			fwrite(recv_buffer, sizeof(char), strlen(recv_buffer), fp);
		}

		fclose(fp);
	}

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

	MPI_Finalize();

	return 0;
}
