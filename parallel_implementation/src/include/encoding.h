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
	int nr_of_pos;
	int nr_of_bytes;
	short *positions;
	BYTE *encodedText;
} EncodingText;

bool findEncodingFromTree(char character, TreeNode *root, CharEncoding *dst, int depth);
void appendStringToByteArray(CharEncoding *charEncoding, EncodingText *encodingText, int* charIndex, char* currentChar);

extern void getEncodingFromTree(CharEncodingDictionary *encodingDict, CharFreqDictionary *charFreqDict, TreeNode *root); 
extern void encodeStringToByteArray(EncodingText *encodingText, CharEncodingDictionary* encodingDict, char *text, int total_letters);
extern void printEncodings(CharEncodingDictionary* dict);
