
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char text[] = 
	"Life is a dream " 
	"A gift we receive"
	"To live and to love"
	"We forge the path"

	"Our nightmare in birth"
	"Our struggle for worth"
	"In vain we carry on"
	"Our mission to become"

	"Adapt to this world"
	"This chance we must take"
	"We\'ll sing our song"
	"We\'ll play our hand"

	"Why allow (broken world)"
	"This crooked fate? (Retaliate)"
	"The tunnel\'s light (accept defeat)"
	"Is unmoving (don\'t be afraid)"
	"It burns your eyes (obscures the path)"
	"But you persevere"
	"You must adapt because it\'s there"

	"Climb, find your way"
	"Scale high, don\'t look back"
	"With hope you will find"
	"The life you seek"

	"Judgement from birth"
	"Criticize our worth"
	"We struggled to uphold"
	"Struck down, will we fold?"

	"Climb, find your way"
	"Scale high, don\'t look back"
	"With hope you will find"
	"The life you seek"

	"I wish I could be some one"
	"Be the moon and be the sun"
	"Ambition burned my wings ablaze"
	"Shed a tear in these dark days"
	"Tearing down the walls of love"
	"Soaring through the fire above"
	"Pride exists to be reclaimed"
	"Roll the dice don\'t lose the game"
	"Carry the weight of the world"
	"The summit says you\'ll prove my worth"
	"Let me claim what\'s rightfully"
	"Yours to take and mine to keep"
	"Stars begin to swarm around"
	"The path of fate this tired ground"
	"Challenge met but all the while"
	"A mountain build on greed and guile"; 

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

int MAX_UNIQUE_LETTERS = 100; 

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

int main() {
	int count = sizeof(text) / sizeof(text[0]);
	
	struct LetterFreqDict res;
	res.number_of_letters = 0; 
	res.letterFreqs = malloc(sizeof(struct LetterFreq) * MAX_UNIQUE_LETTERS); 

	get_freqs_from(text, count, &res); 

	sort_freqs(&res);

	struct TreeNode* root = create_huffman_tree(&res); 

	struct LetterEncoding* encodings = malloc(sizeof(struct LetterEncoding) * res.number_of_letters);
 
	get_encoding_from_tree(&res, root, encodings); 

	for (int i = 0; i < res.number_of_letters; i++) {
		printf("%c, %s\n", encodings[i].letter, encodings[i].encoding); 
	}
	
	// pass through song and replace letters with encoding

	// use reverse encoding table to decode

	for (int i = 0; i < res.number_of_letters; i++) {
		printf("%c: %d\n", res.letterFreqs[i].letter, res.letterFreqs[i].frequency); 
	}
	
	return 0;
}
