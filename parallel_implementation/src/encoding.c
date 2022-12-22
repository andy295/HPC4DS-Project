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
	unsigned int *byteArrayIndex = &encodingText->nr_of_bytes;

	#if VERBOSE <= 2
		printf("char: %c, encoding: %s\n", charEncoding->character, charEncoding->encoding);
	#endif

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

	encodingText->nr_of_pos = (total_letters % CHARS_PER_BLOCK != 0) ? (total_letters / CHARS_PER_BLOCK) + 1 : total_letters / CHARS_PER_BLOCK;
	encodingText->positions = malloc(sizeof(short) * encodingText->nr_of_pos); 

	encodingText->encodedText = malloc(sizeof(BYTE) * total_letters); // TODO: check if this is the right size

	unsigned short bitSizeOfBlock = 0;
	for (int i = 0; i < total_letters; i++) {
		bool found = false;
		for (int j = 0; j < encodingDict->number_of_chars && !found; j++){
			if (text[i] == encodingDict->charEncoding[j].character){
				appendStringToByteArray(&encodingDict->charEncoding[j], encodingText, &c);
				bitSizeOfBlock += encodingDict->charEncoding[j].length;

				found = true;
			}
		}

		if ((i+1) % CHARS_PER_BLOCK == 0 && i > 0) {
			int idx = ((i+1) / CHARS_PER_BLOCK) - 1;
			encodingText->positions[idx] = bitSizeOfBlock;
		}
	}

	if (encodingText->nr_of_bits > 0) {
		encodingText->encodedText[encodingText->nr_of_bytes] = c;
		++encodingText->nr_of_bytes;
		encodingText->positions[encodingText->nr_of_pos-1] = bitSizeOfBlock;
	}

	// reduce encoded text size if this last is smaller than the allocated one
	if (sizeof(BYTE) * total_letters > sizeof(BYTE) * encodingText->nr_of_bytes)
		encodingText->encodedText = realloc(encodingText->encodedText, sizeof(BYTE) * encodingText->nr_of_bytes);
}

void mergeEncodedText(EncodingText *dst, EncodingText *src) {
	dst->encodedText = realloc(dst->encodedText, sizeof(BYTE) * (dst->nr_of_bytes + src->nr_of_bytes));
	memcpy(dst->encodedText + dst->nr_of_bytes, src->encodedText, src->nr_of_bytes);
	dst->nr_of_bytes += src->nr_of_bytes;

	dst->positions = realloc(dst->positions, sizeof(unsigned short) * (dst->nr_of_pos + src->nr_of_pos));
	memcpy(dst->positions + dst->nr_of_pos, src->positions, src->nr_of_pos * sizeof(unsigned short));
	dst->nr_of_pos += src->nr_of_pos;
}

char* decodeFromFile(FILE *fp, TreeNode *root, int bytesToProcess, int numberOfChars) {
	char byte;
	bool found = false;
	bool updateByte = true;
	int bit = 0;

	TreeNode *intermediateNode = root;
	char *decodedText = malloc(sizeof(char) * numberOfChars);

	for (int i; i < numberOfChars;) {
		if (updateByte) {
			fread(&byte, sizeof(char), 1, fp);
			updateByte = false;
		}

		for (; bit < BIT_8 && !found; bit++) {
			if (IsBit(byte, bit)) {
				if (intermediateNode->rightChild != NULL)
					intermediateNode = intermediateNode->rightChild;
				else {
					found = true;
					decodedText[i] = intermediateNode->character;
				}
			} else {
				if (intermediateNode->leftChild != NULL)
					intermediateNode = intermediateNode->leftChild;
				else {
					found = true;
					decodedText[i] = intermediateNode->character;
				}
			} 
		}

		if (bit >= BIT_8) {
			bit = 0;
			updateByte = true;
		}

		if (found) {
			++i;
			intermediateNode = root;
			found = false;
		}
	}

	return decodedText;
}

void printEncodings(CharEncodingDictionary* dict) {
	for (int i = 0; i < dict->number_of_chars; i++) {
		printFormattedChar(dict->charEncoding[i].character);
		printf(": %s\t\nlength: %d\n", dict->charEncoding[i].encoding, dict->charEncoding[i].length);
	}
}


void printEncodedText(BYTE *text, int length) {
	for (int i = 0; i < length; i++) {
        printf("\t");

		for (int j = 0; j < BIT_8; j++)
            printf("%d", !!((text[i] << j) & 0x80));
        
        printf("\n");
    }
    
    printf("\n");
}
