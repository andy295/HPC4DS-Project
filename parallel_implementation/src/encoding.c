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

void appendStringToByteArray(CharEncoding *charEncoding, EncodingText *encodingText, char *currentChar) {
	for (int k = 0; k < charEncoding->length; k++) {
		
		if (charEncoding->encoding[k] == '1')
			SetBit(*currentChar, encodingText->nr_of_bits);
		
		if (encodingText->nr_of_bits == 7)
			copyEncodedText(encodingText, currentChar);
		else
			++encodingText->nr_of_bits;
	}

	#if VERBOSE <= 2
		printf("char: ");
		printFormattedChar(charEncoding->character);
		printf(" encoding: %s\n", charEncoding->encoding);
	#endif
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

			if (encodingText->nr_of_bits > 0 && encodingText->nr_of_bits % BIT_8 != 0)
				copyEncodedText(encodingText, &c);
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

bool isLeaf(TreeNode *node) {
	return node->leftChild == NULL && node->rightChild == NULL;
}

void appendCharacter(char *text, char c, int *idx) {
	text = realloc(text, sizeof(char) * (*idx + 1));
	text[*idx] = c;
	++(*idx);
	printf("%c\n", text[*idx - 1]);
}

char* decodeFromFile(int startByte, unsigned short *dimensions, int blockStart, int blockNr, FILE *fp, TreeNode *root) {
	char *decodedText = malloc(sizeof(char));
	int idx = 0;

	BYTE byte;
	TreeNode *intermediateNode = root;

	// maybe we can move it inot file_utils.c
	fseek(fp, 0, SEEK_SET);
	fseek(fp, startByte, SEEK_SET);
	
	if (isLeaf(root)) {
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

				printEncodedText(&byte, sizeof(BYTE));
			}

			if (IsBit(byte, bit)) {
				if (intermediateNode->rightChild != NULL) {
					intermediateNode = intermediateNode->rightChild;

					if (isLeaf(intermediateNode)) {
						printf("%d. ", i);
						appendCharacter(decodedText, intermediateNode->character, &idx);
						found = true;
					}
					else
						printf("bit: %d - %c\n", bit, intermediateNode->character);
				}
			} else {
				if (intermediateNode->leftChild != NULL) {
					intermediateNode = intermediateNode->leftChild;

					if (isLeaf(intermediateNode)) {
						printf("%d. ", i);
						appendCharacter(decodedText, intermediateNode->character, &idx);
						found = true;
					}
					else
						printf("bit: %d - %c\n", bit, intermediateNode->character);
				}
			}

			if (bit + 1 == BIT_8) {
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
