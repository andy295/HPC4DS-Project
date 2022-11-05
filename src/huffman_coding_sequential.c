
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define VERBOSE 0

int MAX_UNIQUE_LETTERS = 100; 

struct LetterFreq {
	int frequency; 
	char letter; 
}; 

struct LetterFreqDict {
	int number_of_letters; 
	struct LetterFreq* letterFreqs; 
}; 

struct TreeNode {
	int frequency; 
	char letter; 

	struct TreeNode* leftChild; 
	struct TreeNode* rightChild; 
}; 

struct LinkedListTreeNodeItem {
	struct TreeNode* item; 
	struct LinkedListTreeNodeItem* next; 
}; 

struct LetterEncoding {
	char letter; 
	char* encoding; 
}; 

void get_freqs_from(char data[], int count, struct LetterFreqDict* res) {	
	for (int i = 0; i < count; i++){
		char letter = data[i]; 

		int assigned = 0; 
		for (int j = 0; j < res->number_of_letters; j++) {
			if (res->letterFreqs[j].letter == letter){
				res->letterFreqs[j].frequency += 1; 
				assigned = 1; 
			}
		}

		if (!assigned) {
			res->letterFreqs[res->number_of_letters].letter = letter; 
			res->letterFreqs[res->number_of_letters].frequency = 1; 
			res->number_of_letters += 1; 
		}
	}
}

void sort_freqs(struct LetterFreqDict* res){
	for (int i = 0; i < res->number_of_letters; i++) {
		
		struct LetterFreq minLetterFreq = res->letterFreqs[i];
		int indexOfMin = i;  
		for (int j = i; j < res->number_of_letters; j++){
			if (minLetterFreq.frequency > res->letterFreqs[j].frequency){
				minLetterFreq = res->letterFreqs[j]; 
				indexOfMin = j; 
			}
		}

		struct LetterFreq temp = res->letterFreqs[i]; 
		res->letterFreqs[i] = minLetterFreq;
		res->letterFreqs[indexOfMin] = temp;
	}
}

struct LinkedListTreeNodeItem* new_node(char letter, int freq){
	struct TreeNode* r = malloc(sizeof(struct TreeNode)); 
	r->letter = letter; 
	r->frequency = freq; 
	r->rightChild = NULL; 
	r->leftChild = NULL; 

	struct LinkedListTreeNodeItem* listItem = malloc(sizeof(struct LinkedListTreeNodeItem));
	listItem->item = r;
	listItem->next = NULL;   
	return listItem; 
}

struct LinkedListTreeNodeItem* get_min_freq(struct LinkedListTreeNodeItem* start){
	struct LinkedListTreeNodeItem* temp = new_node(start->item->letter, start->item->frequency); 
	temp->item->leftChild = start->item->leftChild;  
	temp->item->rightChild = start->item->rightChild;  
	return temp;
}

struct LinkedListTreeNodeItem* ordered_append_to_freq(struct LinkedListTreeNodeItem* start, struct LinkedListTreeNodeItem* item){
	struct LinkedListTreeNodeItem* prev = NULL; 
	struct LinkedListTreeNodeItem* firstStart = start; 


	if (item->item->frequency < start->item->frequency){
		item->next = start; 
		return item; 
	}

	while(start->next != NULL){
		if (item->item->frequency < start->item->frequency){
			prev->next = item; 
			item->next = start; 

			return firstStart; 
		}
		prev = start; 
		start = start->next; 
	}

	start->next = item; 
	return firstStart;
}

void print_list(struct LinkedListTreeNodeItem* start){
	while(start->next != NULL){
		printf("%c: %d\n", start->item->letter, start->item->frequency); 
		start = start->next; 
	}
	printf("%c: %d\n", start->item->letter, start->item->frequency); 
}

void print_tree(struct TreeNode* root){
	if (root != NULL){
		printf("%c, %d\n",root->letter, root->frequency); 

		print_tree(root->leftChild); 
		print_tree(root->rightChild); 
	}
}

struct LinkedListTreeNodeItem* create_linked_list(struct LetterFreqDict* res) {
	struct LinkedListTreeNodeItem* start = new_node(res->letterFreqs[0].letter, res->letterFreqs[0].frequency); 

	struct LinkedListTreeNodeItem* old; 
	old = start; 
	for (int i = 1; i < res->number_of_letters; i++)	{
		struct LinkedListTreeNodeItem* temp = new_node(res->letterFreqs[i].letter, res->letterFreqs[i].frequency); 
		old->next = temp; 
		old = temp; 
	}

	return start; 
}

struct TreeNode* create_huffman_tree(struct LetterFreqDict* res){
	struct LinkedListTreeNodeItem* start = create_linked_list(res); 

	do{
		struct TreeNode* left = get_min_freq(start)->item; 
		struct LinkedListTreeNodeItem* oldStart = start; 
		start = start->next; 
		free(oldStart); 

		struct TreeNode* right = get_min_freq(start)->item; 
		oldStart = start;  
		start = start->next; 
		free(oldStart); 

		struct LinkedListTreeNodeItem* top = new_node('$', left->frequency + right->frequency); 
		top->item->leftChild = left; 
		top->item->rightChild = right; 

		if (start == NULL) {
			start = top; 
		} else {
			start = ordered_append_to_freq(start, top); 
		}

	} while(start->next != NULL); 

	return start->item; 
}

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

void get_encoding_from_tree(struct LetterFreqDict* allLetters, struct TreeNode* root, struct LetterEncoding* encodings){

	for (int i = 0; i < allLetters->number_of_letters; i++){
		
		encodings[i].letter = allLetters->letterFreqs[i].letter;  
		encodings[i].encoding = malloc(sizeof(char) * allLetters->number_of_letters); 

		find_encoding(allLetters->letterFreqs[i].letter, root, encodings[i].encoding, 0); 
	}
}

void print_binary(char c){
	for (int i = 0; i < 8; i++){
		printf("%d", (c >> i) & 1); 
	}

	printf("\n");
}

char* read_file(char* filename){
	FILE* fp = fopen(filename, "r"); 
	fseek(fp, 0, SEEK_END); 
	long fsize = ftell(fp); 
	fseek(fp, 0, SEEK_SET); 

	char* string = malloc(fsize + 1); 
	fread(string, fsize, 1, fp); 
	fclose(fp); 

	string[fsize] = '\0'; 

	return string; 
}

int get_string_length(char* string){
	int i = 0; 
	while(string[i] != '\0'){
		i++; 
	}

	return i; 
}

void append_string_to_binary_file(char* string, FILE* fp, int* charIndex, char* currentChar){
	int strlength = strlen(string);
	for (int k = 0; k < strlength; k++) {

		if (string[k] == '1') {
			*currentChar |= 1 << *charIndex;
		} 

		if (*charIndex == 7) {
			fwrite(currentChar, sizeof(char), 1, fp);
			*charIndex = 0;
			*currentChar = 0;
		} else {
			*charIndex += 1;
		}
	}
}

void encode_to_file(char* text, struct LetterEncoding* encodings, int unique_letters, int total_letters){
	
	FILE *fp;
	fp = fopen("output", "wb");

	int charIndex = 0; 
	char c = 0;
	for (int i = 0; i < total_letters; i++) {
		for (int j = 0; j < unique_letters; j++) {
			if (text[i] == encodings[j].letter) {
				append_string_to_binary_file(encodings[j].encoding, fp, &charIndex, &c); 
			}
		}
	}

	if (charIndex != 7) {
		fwrite(&c, sizeof(char), 1, fp);
	}

	fclose(fp);
}

void decode_from_file(struct TreeNode* root){

	FILE *fp2;
	fp2 = fopen("output", "rb");

	char c;
	char lastContinuousChar = 0;

	struct TreeNode* intermediateNode = root;

	while (fread(&c, sizeof(char), 1, fp2)) {

		for (int i = 0; i < 8; i++) {
			if (intermediateNode->letter != '$') {

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

int get_file_size(char* filename){
	FILE* fp = fopen(filename, "r"); 
	fseek(fp, 0, SEEK_END); 
	long fsize = ftell(fp); 
	fseek(fp, 0, SEEK_SET); 

	fclose(fp); 

	return fsize * 8; 
}

struct timespec get_time(){
	struct timespec time; 
	clock_gettime(CLOCK_MONOTONIC, &time); 
	return time; 
}

double get_execution_time(struct timespec start, struct timespec end){
	double time_taken; 
	time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
	time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9; 

	return time_taken; 
}

int main() {
	char* text = read_file("text.txt");
	int count = get_string_length(text);

	struct timespec start = get_time();

	// ENCODING
	struct LetterFreqDict res;
	res.number_of_letters = 0; 
	res.letterFreqs = malloc(sizeof(struct LetterFreq) * MAX_UNIQUE_LETTERS); 
	get_freqs_from(text, count, &res); 
	sort_freqs(&res);
	struct TreeNode* root = create_huffman_tree(&res); 
	struct LetterEncoding* encodings = malloc(sizeof(struct LetterEncoding) * res.number_of_letters);
	get_encoding_from_tree(&res, root, encodings); 
	encode_to_file(text, encodings, res.number_of_letters, count); 

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
	printf("Original file size: %d bits\n", get_file_size("text.txt"));
	printf("Compressed file size: %d bits\n", get_file_size("output"));
	printf("Compression rate: %.2f%%\n", (1 - (double)get_file_size("output") / (double)get_file_size("text.txt")) * 100);
	
	printf("\n");

	return 0;
}
