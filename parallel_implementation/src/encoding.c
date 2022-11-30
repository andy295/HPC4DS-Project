
#include "include/encoding.h"

bool findEncodingFromTree(char letter, TreeNode* root, char* dst, int depth){

	bool found = false; 
	if (root->character == letter){
		dst[depth] = '\0';
		return 1;  
	} else {
		
		if (root->leftChild != NULL){
			dst[depth] = '0'; 
			found = findEncodingFromTree(letter, root->leftChild, dst, depth+1); 
		}

		if (found == 0 && root->rightChild != NULL){
			dst[depth] = '1'; 
			found = findEncodingFromTree(letter, root->rightChild, dst, depth+1); 
		}
	}

	return found; 
}

CharEncoding* getEncodingFromTree(CharFreqDictionary* allLetters, TreeNode* root){
	CharEncoding* encodings = malloc(sizeof(struct CharEncoding) * allLetters->number_of_chars);

	for (int i = 0; i < allLetters->number_of_chars; i++){
		
		encodings[i].character = allLetters->charFreqs[i].character;  
		encodings[i].encoding = malloc(sizeof(char) * allLetters->number_of_chars); 

		findEncodingFromTree(allLetters->charFreqs[i].character, root, encodings[i].encoding, 0); 
	}

	return encodings;
}

void printEncodings(CharEncoding* encodings, int size){
	for (int i = 0; i < size; i++){
		printFormattedChar(encodings[i].character);
		printf(": %s\n", encodings[i].encoding);
	}
}
