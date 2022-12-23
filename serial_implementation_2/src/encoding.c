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
		printf("char: ");
		printFormattedChar(charEncoding->character);
		printf(" encoding: %s\n", charEncoding->encoding);
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

void encodeStringToByteArray(EncodingText *encodingText, CharEncodingDictionary* encodingDict, char *text, int total_chars) {
	char c = 0;

	encodingText->nr_of_dim = (total_chars % CHARS_PER_BLOCK != 0) ? (total_chars / CHARS_PER_BLOCK) + 1 : total_chars / CHARS_PER_BLOCK;
	encodingText->dimensions = calloc(encodingText->nr_of_dim, sizeof(short)); 

	encodingText->encodedText = malloc(sizeof(BYTE) * total_chars); // TODO: check if this is the right size

	unsigned short bitSizeOfBlock = 0;
	for (int i = 0; i < total_chars; i++) {
		bool found = false;
		for (int j = 0; j < encodingDict->number_of_chars && !found; j++) {
			if (text[i] == encodingDict->charEncoding[j].character) {
				appendStringToByteArray(&encodingDict->charEncoding[j], encodingText, &c);
				bitSizeOfBlock += encodingDict->charEncoding[j].length;

				found = true;
			}
		}

		if ((i+1) % CHARS_PER_BLOCK == 0 && i > 0) {
			int idx = ((i+1) / CHARS_PER_BLOCK) - 1;
			encodingText->dimensions[idx] = bitSizeOfBlock;
			bitSizeOfBlock = 0;
		}
	}

	if (encodingText->nr_of_bits > 0) {
		encodingText->encodedText[encodingText->nr_of_bytes] = c;
		++encodingText->nr_of_bytes;

		if (bitSizeOfBlock > 0)
			encodingText->dimensions[encodingText->nr_of_dim-1] = bitSizeOfBlock;
	} else if (encodingText->dimensions[encodingText->nr_of_dim-1] == 0) {
		--encodingText->nr_of_dim;
		encodingText->dimensions = realloc(encodingText->dimensions, sizeof(short) * encodingText->nr_of_dim);
	}

	// reduce encoded text size if this last is smaller than the allocated one
	if (sizeof(BYTE) * total_chars > sizeof(BYTE) * encodingText->nr_of_bytes)
		encodingText->encodedText = realloc(encodingText->encodedText, sizeof(BYTE) * encodingText->nr_of_bytes);
}

void mergeEncodedText(EncodingText *dst, EncodingText *src) {
	dst->encodedText = realloc(dst->encodedText, sizeof(BYTE) * (dst->nr_of_bytes + src->nr_of_bytes));
	memcpy(dst->encodedText + dst->nr_of_bytes, src->encodedText, src->nr_of_bytes);
	dst->nr_of_bytes += src->nr_of_bytes;

	dst->dimensions = realloc(dst->dimensions, sizeof(unsigned short) * (dst->nr_of_dim + src->nr_of_dim));
	memcpy(dst->dimensions + dst->nr_of_dim, src->dimensions, src->nr_of_dim * sizeof(unsigned short));
	dst->nr_of_dim += src->nr_of_dim;
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

// maybe we don't need if we use the dimensions instead of the positions
void appendToEncodingText(EncodingText *encodingText, CharEncoding *charEncoding, char character) {
	int freeBits = BIT_8 - encodingText->nr_of_bits;

	if (freeBits < charEncoding->length) {
		int bytesNr = charEncoding->length / BIT_8;
		bytesNr += ((charEncoding->length % BIT_8) > freeBits) ? 1 : 0;

		encodingText->encodedText = realloc(encodingText->encodedText, sizeof(BYTE) * (encodingText->nr_of_bytes + bytesNr));
	}

	character = (char)encodingText->encodedText[encodingText->nr_of_bytes-1];
	appendStringToByteArray(charEncoding, encodingText, &character);

	if (encodingText->nr_of_bits == 0)
		--encodingText->nr_of_bytes;
}

CharEncoding* getEncoding(CharEncodingDictionary *dict, char character) {
	for (int i = 0; i < dict->number_of_chars; i++) {
		if (dict->charEncoding[i].character == character)
			return &dict->charEncoding[i];
	}

	return NULL;
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
