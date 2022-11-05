
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "letter_freq.c"
#include "huffman_tree.c"
#include "letter_encoding.c"

#include "utils/time_utils.c"
#include "utils/file_utils.c"
#include "utils/print_utils.c"

#define VERBOSE 1
#define MAX_UNIQUE_LETTERS 100
#define ENCODED_FILE "encoded_file"
#define NUM_OF_PROCESSES 4

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

void get_encoding_from_tree(struct LetterFreqDictionary* allLetters, struct TreeNode* root, struct LetterEncoding* encodings){

	for (int i = 0; i < allLetters->number_of_letters; i++){
		
		encodings[i].letter = allLetters->letterFreqs[i].letter;  
		encodings[i].encoding = malloc(sizeof(char) * allLetters->number_of_letters); 

		find_encoding(allLetters->letterFreqs[i].letter, root, encodings[i].encoding, 0); 
	}
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

void decode_from_file(struct TreeNode* root){

	FILE *fp2;
	fp2 = fopen(ENCODED_FILE, "rb");

	char c;
	char lastContinuousChar = 0;

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
	char* subtext = substring(text, i * substring_length, substring_length);
	// get letter frequencies for the processes' portion of text
	struct LetterFreqDictionary* allLetters = get_letter_freqs(subtext, total_text_length);

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

		// create the Huffman tree
		struct TreeNode* root = create_huffman_tree(allLetters);

		// get the encodings for each letter
		struct LetterEncoding* encodings = malloc(sizeof(struct LetterEncoding) * allLetters->number_of_letters);
		get_encoding_from_tree(allLetters, root, encodings);

		// send encoding table to all processes
		for (int i = 1; i < NUM_OF_PROCESSES; i++) {
			MPI_Send(encodings, sizeof(struct LetterEncoding) * allLetters->number_of_letters, MPI_BYTE, i, 0, MPI_COMM_WORLD);
		}
	}

	// all processes receive the encoding table
	if (pid != 0) {
		struct LetterEncoding* encodings = malloc(sizeof(struct LetterEncoding) * allLetters->number_of_letters);
		MPI_Recv(encodings, sizeof(struct LetterEncoding) * allLetters->number_of_letters, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		
		// noi abbiamo fatto solo encode to file, ma servirebbe un encode to byte array, 
		// che poi viene mandato al master process, che lo scrive su file (questa riga di commento è stata scritta da copilot)
		
		// encode the processes' portion of text
		// encode_to_file(subtext, encodings, allLetters->number_of_letters, total_text_length);
	}



	// struct timespec start = get_time();

	// ENCODING
	// struct LetterFreqDictionary res;
	// res.number_of_letters = 0; 
	// res.letterFreqs = malloc(sizeof(struct LetterFreq) * MAX_UNIQUE_LETTERS); 
	// get_freqs_from(text, count, &res); 
	// sort_freqs(&res);
	// struct TreeNode* root = create_huffman_tree(&res); 
	// struct LetterEncoding* encodings = malloc(sizeof(struct LetterEncoding) * res.number_of_letters);
	// get_encoding_from_tree(&res, root, encodings); 
	// encode_to_file(text, encodings, res.number_of_letters, count); 

	// if (VERBOSE){
	// 	struct timespec end = get_time();
	// 	printf("\nEncoding time: %f seconds\n", get_execution_time(start, end));
	// 	start = get_time();
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

	MPI_Finalize();

	return 0;
}
