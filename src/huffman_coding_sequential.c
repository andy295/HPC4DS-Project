
#define VERBOSE 1
#define MAX_UNIQUE_LETTERS 100
#define ENCODED_FILE "encoded_file"
#define SOURCE_FILE "text.txt"

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
#include "utils/sort_utils.c"

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
	char* text = read_file(SOURCE_FILE);
	int count = strlen(text);

	struct timespec start = get_time();

	// ENCODING
	struct LetterFreqDictionary* res = get_letter_freqs(text, count);
	sort_freqs(res);
	struct TreeNode* root = create_huffman_tree(res); 
	struct LetterEncoding* encodings = malloc(sizeof(struct LetterEncoding) * res->number_of_letters);
	get_encoding_from_tree(res, root, encodings); 
	//encode_to_file(text, encodings, res->number_of_letters, count); 

	int byteArrayFinalSize = 0; 
	char* encoded_data = encode_to_byte_array(text, encodings, res->number_of_letters, count, &byteArrayFinalSize);
	// write to file
	FILE *fp;
	fp = fopen(ENCODED_FILE, "wb");
	fwrite(encoded_data, sizeof(char), byteArrayFinalSize, fp); // encode only the necessary bytes
	fclose(fp);

	if (VERBOSE){
		struct timespec end = get_time();
		printf("\nEncoding time: %f seconds\n", get_execution_time(start, end));
		start = get_time();
	}

	// DECODING
	decode_from_file(root); 

	if (VERBOSE){
		struct timespec end = get_time();
		printf("\nDecoding time: %f seconds\n", get_execution_time(start, end));
	}

	printf("\nCompression stats: \n");
	printf("Original file size: %d bits\n", get_file_size(SOURCE_FILE));
	printf("Compressed file size: %d bits\n", get_file_size(ENCODED_FILE));
	printf("Compression rate: %.2f%%\n", (1 - (double)get_file_size(ENCODED_FILE) / (double)get_file_size(SOURCE_FILE)) * 100);
	
	printf("\n");

	return 0;
}
