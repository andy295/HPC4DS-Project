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
	int number_of_bits;
	BYTE *text;
} EncodingText;

bool findEncodingFromTree(char character, TreeNode *root, CharEncoding *dst, int depth);
void appendStringToByteArray(char* string, BYTE* byte_array, int* byteArrayIndex, int* charIndex, char* currentChar); 

extern void getEncodingFromTree(CharEncodingDictionary *encodingDict, CharFreqDictionary *charFreqDict, TreeNode *root); 
extern BYTE* encodeStringToByteArray(char* text, CharEncodingDictionary* encodingDict, int total_letters, int* byteArrayIndex);
extern void printEncodings(CharEncodingDictionary* dict);

