#include "include/encoding.h"

bool findEncodingFromTree(char character, TreeNode *root, CharEncoding *dst, unsigned int depth) {
	bool found = false;
	if (root->character == character) {
		dst->encoding = realloc(dst->encoding, sizeof(char) * (depth+1));
		dst->encoding[depth] = '\0';
		dst->length = depth;
		return true;  
	} else {
		if (root->leftChild != NULL) {
			dst->encoding = realloc(dst->encoding, sizeof(char) * (depth+1));
			dst->encoding[depth] = '0'; 
			found = findEncodingFromTree(character, root->leftChild, dst, depth+1); 
		}

		if (found == 0 && root->rightChild != NULL) {
			if (root->leftChild == NULL)
				dst->encoding = realloc(dst->encoding, sizeof(char) * (depth+1));
		
			dst->encoding[depth] = '1';
			found = findEncodingFromTree(character, root->rightChild, dst, depth+1); 
		}
	}

	return found;
}

void getEncodingFromTree(CharEncodingDictionary *encodingDict, CharFreqDictionary *charFreqDict, TreeNode *root) {
	encodingDict->charEncoding = malloc(sizeof(CharEncoding) * encodingDict->number_of_chars);

	for (int i = 0; i < encodingDict->number_of_chars; i++) {
		encodingDict->charEncoding[i].character = charFreqDict->charFreqs[i].character;  
		encodingDict->charEncoding[i].length = 0;
		encodingDict->charEncoding[i].encoding = malloc(sizeof(char));
		findEncodingFromTree(charFreqDict->charFreqs[i].character, root, &encodingDict->charEncoding[i], encodingDict->charEncoding[i].length); 
	}
}

void printEncodings(CharEncodingDictionary* dict) {
	for (int i = 0; i < dict->number_of_chars; i++) {
		printFormattedChar(dict->charEncoding[i].character);
		printf(": %s\t\nlength: %d\n", dict->charEncoding[i].encoding, dict->charEncoding[i].length);
	}
}
