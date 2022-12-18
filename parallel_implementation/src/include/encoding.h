#pragma once

#include "global_constants.h"

#include "char_freq.h"
#include "huffman_tree.h"

#include "../utils/print_utils.h"

typedef struct CharEncoding {
	char character; 
	int length;
	char* encoding; 
} CharEncoding;

typedef struct CharEncodingDictionary {
	int number_of_chars; 
	CharEncoding *charEncoding; 
} CharEncodingDictionary;

typedef struct EncodingText {
	unsigned int nr_of_pos; // number of elemnts in the positions array
	unsigned int nr_of_bytes; // number of bytes in the encodedText array
	unsigned int nr_of_bits; // number of used bits in the last byte of the encodedText array
	unsigned short *positions;
	BYTE *encodedText;
} EncodingText;

bool findEncodingFromTree(char character, TreeNode *root, CharEncoding *dst, int depth);
void appendStringToByteArray(CharEncoding *charEncoding, EncodingText *encodingText, char* currentChar);

extern void getEncodingFromTree(CharEncodingDictionary *encodingDict, CharFreqDictionary *charFreqDict, TreeNode *root); 
extern void encodeStringToByteArray(EncodingText *encodingText, CharEncodingDictionary* encodingDict, char *text, int total_letters);
extern void mergeEncodedText(EncodingText *dst, EncodingText *src);
extern void printEncodings(CharEncodingDictionary* dict);