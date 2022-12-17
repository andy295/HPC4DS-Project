#include "include/encoding.h"

bool findEncodingFromTree(char character, TreeNode *root, CharEncoding *dst, int depth) {
	bool found = false;
	if (root->character == character) {
		dst->encoding = realloc(dst->encoding, sizeof(char) * (depth+1));
		dst->encoding[depth] = ENDTEXT;
		dst->length = depth;
		return true;  
	} else {
		if (root->leftChild != NULL) {
			dst->encoding = realloc(dst->encoding, sizeof(char) * (depth+1));
			dst->encoding[depth] = '0'; 
			found = findEncodingFromTree(character, root->leftChild, dst, depth+1); 
		}

		if (!found && root->rightChild != NULL) {
			if (!found)
				dst->encoding = realloc(dst->encoding, sizeof(char) * (depth+1));

			dst->encoding[depth] = '1';
			found = findEncodingFromTree(character, root->rightChild, dst, depth+1); 
		}
	}

	return found;
}

void getEncodingFromTree(CharEncodingDictionary *encodingDict, CharFreqDictionary *charFreqDict, TreeNode *root) {
	encodingDict->charEncoding = malloc(sizeof(CharEncoding) * charFreqDict->number_of_chars);
	encodingDict->number_of_chars = charFreqDict->number_of_chars;

	for (int i = 0; i < encodingDict->number_of_chars; i++) {
		encodingDict->charEncoding[i].character = charFreqDict->charFreqs[i].character;  
		encodingDict->charEncoding[i].length = 0;
		encodingDict->charEncoding[i].encoding = malloc(sizeof(char));

		findEncodingFromTree(charFreqDict->charFreqs[i].character, root, &encodingDict->charEncoding[i], encodingDict->charEncoding[i].length); 
	}
}

void appendStringToByteArray(CharEncoding *charEncoding, EncodingText *encodingText, char* currentChar) {
	BYTE *byte_array = encodingText->encodedText;
	int *byteArrayIndex = &encodingText->nr_of_bytes;

	for (int k = 0; k < charEncoding->length; k++) {
		
		if (charEncoding->encoding[k] == '1')
			SetBit(*currentChar, encodingText->nr_of_bits);
		
		if (encodingText->nr_of_bits == 7) {
			byte_array[*byteArrayIndex] = *currentChar; 
			++(*byteArrayIndex);
			encodingText->nr_of_bits = 0; 
			*currentChar = 0; 
		}
		else
			++encodingText->nr_of_bits;
	}
}

void encodeStringToByteArray(EncodingText *encodingText, CharEncodingDictionary* encodingDict, char *text, int total_letters) {
	char c = 0;

	encodingText->nr_of_pos =  (encodingText->nr_of_pos % CHARS_PER_BLOCK != 0) ? (total_letters / CHARS_PER_BLOCK) + 1 : total_letters / CHARS_PER_BLOCK;
	encodingText->positions = malloc(sizeof(short) * encodingText->nr_of_pos); 

	encodingText->encodedText = malloc(sizeof(BYTE) * total_letters); 

	for (int i = 0; i < total_letters; i++) {
		for (int j = 0; j < encodingDict->number_of_chars; j++)
			if (text[i] == encodingDict->charEncoding[j].character)
				appendStringToByteArray(&encodingDict->charEncoding[j], encodingText, &c);

		if (i % CHARS_PER_BLOCK == 0 && i > 0) {
			int idx = (i / CHARS_PER_BLOCK) - 1;
			encodingText->positions[idx] = (encodingText->nr_of_bytes * BIT_8) + encodingText->nr_of_bits;
		}
	}

	// reduce encoded text size if this last is smaller than the allocated one
	if (sizeof(BYTE) * total_letters > sizeof(BYTE) * encodingText->nr_of_bytes)
		encodingText->encodedText = realloc(encodingText->encodedText, sizeof(BYTE) * encodingText->nr_of_bytes);

	// maybe we don't need it
	// appends and writes custom end of file character
	// appendStringToByteArray(&encodingDict->charEncoding[encodingDict->number_of_chars-1], encodingText, &charIndex, &c); 
}

void mergeEncodedText(EncodingText *dst, EncodingText *src) {
	dst->encodedText = realloc(dst->encodedText, sizeof(BYTE) * (dst->nr_of_bytes + src->nr_of_bytes));
	memcpy(dst->encodedText + dst->nr_of_bytes, src->encodedText, src->nr_of_bytes);
	dst->nr_of_bytes += src->nr_of_bytes;

	dst->positions = realloc(dst->positions, sizeof(short) * (dst->nr_of_pos + src->nr_of_pos));
	memcpy(dst->positions + dst->nr_of_pos, src->positions, src->nr_of_pos);
	dst->nr_of_pos += src->nr_of_pos;
}

void printEncodings(CharEncodingDictionary* dict) {
	for (int i = 0; i < dict->number_of_chars; i++) {
		printFormattedChar(dict->charEncoding[i].character);
		printf(": %s\t\nlength: %d\n", dict->charEncoding[i].encoding, dict->charEncoding[i].length);
	}
}