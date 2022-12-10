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

int getNewBufferSize(int length, int totalLen); // maybe we don't need it, and we can remove it
void addEncodedChar(EncodingText *encodedText, CharEncoding *encoding); // maybe we don't need it, and we can remove it
void encodeChar(EncodingText *encodedText, CharEncodingDictionary *dict, char c); // maybe we don't need it, and we can remove it
extern void encodeText(EncodingText *encodedText, CharEncodingDictionary *dict, char text[], long length); // maybe we don't need it, and we can remove it
extern void decodeText(EncodingText *encodedText, CharEncodingDictionary *dict, char text[]); // maybe we don't need it, and we can remove it
