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
			if (!found){
				dst->encoding = realloc(dst->encoding, sizeof(char) * (depth+1));
			}
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

void appendStringToByteArray(char* string, BYTE* byte_array, int* byteArrayIndex, int* charIndex, char* currentChar) {
	int stringLength = strlen(string); 
	for (int k = 0; k < stringLength; k++) {
		
		if (string[k] == '1'){
			*currentChar |= 1 << *charIndex;
		}
		
		if (*charIndex == 7){
			byte_array[*byteArrayIndex] = *currentChar; 
			*byteArrayIndex = *byteArrayIndex + 1;
			*charIndex = 0; 
			*currentChar = 0; 
		} else{
			*charIndex += 1;
		}
	}
}

BYTE* encodeStringToByteArray(char* text, CharEncodingDictionary* encodingDict, int total_letters, int* byteArrayIndex) {
	int charIndex = 0; 
	char c = 0;
	BYTE* encoded_text = malloc(sizeof(BYTE) * total_letters); 

	for (int i = 0; i < total_letters; i++)
		for (int j = 0; j < encodingDict->number_of_chars; j++)
			if (text[i] == encodingDict->charEncoding[j].character)
				appendStringToByteArray(encodingDict->charEncoding[j].encoding, encoded_text, byteArrayIndex, &charIndex, &c);

	// appends and writes custom end of file character
	appendStringToByteArray(encodingDict->charEncoding[encodingDict->number_of_chars-1].encoding, encoded_text, byteArrayIndex, &charIndex, &c); 

	return encoded_text; 
}

void printEncodings(CharEncodingDictionary* dict) {
	for (int i = 0; i < dict->number_of_chars; i++) {
		printFormattedChar(dict->charEncoding[i].character);
		printf(": %s\t\nlength: %d\n", dict->charEncoding[i].encoding, dict->charEncoding[i].length);
	}
}

int getNewBufferSize(int length, int totalLen) { // maybe we don't need it, and we can remove it
	int bytes = 0;
	if (length == 0)
		return bytes;

	bytes += totalLen / BIT_8;
	if (totalLen % BIT_8 != 0) {
		int freeBits = BIT_8 - (totalLen % BIT_8);
		length -= freeBits;

		if (length > 0)
			++bytes;
	}

	if (length <= 0)
		return bytes;

	bytes += (length / BIT_8);
	if (length % BIT_8 != 0)
		++bytes;

	return bytes;
}

void addEncodedChar(EncodingText *encodedText, CharEncoding *encoding) { // maybe we don't need it, and we can remove it
	int currBuffSize = encodedText->number_of_bits / BIT_8;
	if (encodedText->number_of_bits % BIT_8 != 0)
		currBuffSize += 1;

	int byte = 0;
	int bit = 0;

	for (int i = 0; i < encoding->length; i++) {
		byte = i / BIT_8;
		bit = i % BIT_8;

		if (encoding->encoding[i] == '1')
			SetBit(encodedText->text[byte], bit);
		else if (encoding->encoding[i] == '0')
			ClrBit(encodedText->text[byte], bit);
	}

	encodedText->number_of_bits += encoding->length;
}

void encodeChar(EncodingText *encodedText, CharEncodingDictionary *dict, char c) { // maybe we don't need it, and we can remove it
	bool found = false;
	for (int j = 0; j < dict->number_of_chars && !found; j++) {
		if (c == dict->charEncoding[j].character) {

			int newSize = getNewBufferSize(dict->charEncoding[j].length, encodedText->number_of_bits);
			if (newSize > 0 && newSize > (encodedText->number_of_bits / BIT_8))
				encodedText->text = realloc(encodedText->text, sizeof(BYTE) * newSize);

			addEncodedChar(encodedText, &dict->charEncoding[j]);
			found = true;
		}
	}
}

void encodeText(EncodingText *encodedText, CharEncodingDictionary *dict, char text[], long length) { // maybe we don't need it, and we can remove it
	for (int i = 0; i < length; i++)
		encodeChar(encodedText, dict, text[i]);

	encodeChar(encodedText, dict, ENDTEXT);
}