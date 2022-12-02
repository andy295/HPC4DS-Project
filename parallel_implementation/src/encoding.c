#include "include/encoding.h"

bool findEncodingFromTree(char character, TreeNode* root, char* dst, int depth) {
	bool found = false; 
	if (root->character == character) {
		dst[depth] = '\0';
		return 1;  
	} else {
		if (root->leftChild != NULL) {
			dst[depth] = '0'; 
			found = findEncodingFromTree(character, root->leftChild, dst, depth+1); 
		}

		if (found == 0 && root->rightChild != NULL) {
			dst[depth] = '1'; 
			found = findEncodingFromTree(character, root->rightChild, dst, depth+1); 
		}
	}

	return found; 
}

CharEncoding* getEncodingFromTree(CharFreqDictionary* dict, TreeNode* root) {
	CharEncoding* encodings = malloc(sizeof(struct CharEncoding) * dict->number_of_chars);

	for (int i = 0; i < dict->number_of_chars; i++) {
		encodings[i].character = dict->charFreqs[i].character;  
		encodings[i].encoding = malloc(sizeof(char) * dict->number_of_chars); 

		findEncodingFromTree(dict->charFreqs[i].character, root, encodings[i].encoding, 0); 
	}

	return encodings;
}

void printEncodings(CharEncoding* encodings, int size) {
	for (int i = 0; i < size; i++) {
		printFormattedChar(encodings[i].character);
		printf(": %s\n", encodings[i].encoding);
	}
}
