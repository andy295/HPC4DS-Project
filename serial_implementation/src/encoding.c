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

void copyEncodedText(EncodingText *encodingText, char *currentChar) {
	encodingText->encodedText[encodingText->nr_of_bytes] = *currentChar;
	++encodingText->nr_of_bytes;
	encodingText->nr_of_bits = 0;
	*currentChar = 0;
}

void updateDimensions(int nrOfChars, unsigned short *dimensions, unsigned short *bitSizeOfBlock, int index) {
	int idx = (nrOfChars > 0) ? ((nrOfChars+1) / CHARS_PER_BLOCK) - 1 : index;
	dimensions[idx] = *bitSizeOfBlock;
	*bitSizeOfBlock = 0;
}

void appendStringToByteArray(CharEncoding *charEncoding, EncodingText *encodingText, char *currentChar) {
	for (int k = 0; k < charEncoding->length; k++) {
		
		if (charEncoding->encoding[k] == '1')
			SetBit(*currentChar, encodingText->nr_of_bits);
		
		if (encodingText->nr_of_bits == 7)
			copyEncodedText(encodingText, currentChar);
		else
			++encodingText->nr_of_bits;
	}

	#if DEBUG
		printf("char: ");
		printFormattedChar(charEncoding->character);
		printf(" encoding: %s\n", charEncoding->encoding);
	#endif
}

void encodeStringToByteArray(EncodingText *encodingText, CharEncodingDictionary *encodingDict, char *text, int total_chars) {
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
			updateDimensions(i, encodingText->dimensions, &bitSizeOfBlock, 0);

			if (encodingText->nr_of_bits > 0 && encodingText->nr_of_bits % BITS_IN_BYTE != 0)
				copyEncodedText(encodingText, &c);
		}
	}

	// after the end of the loop, we could have some pending data
	// it is necessarily to add those information otherwise the decoding phase will fail
	if (bitSizeOfBlock > 0 || (encodingText->nr_of_bits > 0 && encodingText->nr_of_bits % BITS_IN_BYTE != 0)) {
		if (encodingText->nr_of_bits > 0)
			copyEncodedText(encodingText, &c);

		if (bitSizeOfBlock > 0)
			updateDimensions(0, encodingText->dimensions, &bitSizeOfBlock, encodingText->nr_of_dim-1);
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

char* appendCharacter(char *text, char c, int *idx) {
	text = realloc(text, sizeof(char) * (*idx+1));
	text[*idx] = c;
	++(*idx);

	return text;
}

char* decodeFromFile(int startByte, unsigned short *dimensions, int blockStart, int blockNr, FILE *fp, TreeNode *root) {
	char *decodedText = malloc(sizeof(char));
	int idx = 0;

	BYTE byte;
	TreeNode *intermediateNode = root;

	// maybe we can move it into file_utils.c
	fseek(fp, 0, SEEK_SET);
	fseek(fp, startByte, SEEK_SET);
	
	if (isNodeALeaf(root)) {
		decodedText[0] = root->character;
		return decodedText;
	}

	for (int i = blockStart; i < blockStart + blockNr; i++) {
		int bit = 0;
		bool update_byte = true;
		bool found = false;

		for (int j = 0; j < dimensions[i]; ++j) {

			if (update_byte) {
				fread(&byte, sizeof(BYTE), 1, fp);
				update_byte = false;
			}

			if (IsBit(byte, bit)) {
				if (intermediateNode->rightChild != NULL) {
					intermediateNode = intermediateNode->rightChild;

					if (isNodeALeaf(intermediateNode)) {
						decodedText = appendCharacter(decodedText, intermediateNode->character, &idx);
						found = true;
					}
				}
			} else {
				if (intermediateNode->leftChild != NULL) {
					intermediateNode = intermediateNode->leftChild;

					if (isNodeALeaf(intermediateNode)) {
						decodedText = appendCharacter(decodedText, intermediateNode->character, &idx);
						found = true;
					}
				}
			}

			if (bit + 1 == BITS_IN_BYTE) {
				bit = 0;
				update_byte = true;
			}
			else
				++bit;

			if (found) {
				intermediateNode = root;
				found = false;
			}
		}
	}

	decodedText[idx] = ENDTEXT;
	return decodedText;
}

CharEncoding* getEncoding(CharEncodingDictionary *dict, char character) {
	for (int i = 0; i < dict->number_of_chars; i++)
		if (dict->charEncoding[i].character == character)
			return &dict->charEncoding[i];

	return NULL;
}

void mergeDecodedText(DecodingText *dst, DecodingText *src) {
	int oldLength = --(dst->length);
	dst->length += src->length;
	dst->decodedText = realloc(dst->decodedText, sizeof(char) * dst->length);

	memcpy(dst->decodedText + oldLength, src->decodedText, src->length);
}
