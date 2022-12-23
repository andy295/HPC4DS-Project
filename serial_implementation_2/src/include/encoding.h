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
	unsigned int nr_of_dim; // number of elemnts in the positions array
	unsigned int nr_of_bytes; // number of bytes in the encodedText array
	unsigned int nr_of_bits; // number of used bits in the last byte of the encodedText array
	unsigned short *dimensions;
	BYTE *encodedText;
} EncodingText;

typedef struct DecodingText {
	int length;
	char *decodedText;
} DecodingText;

bool findEncodingFromTree(char character, TreeNode *root, CharEncoding *dst, int depth);
void appendStringToByteArray(CharEncoding *charEncoding, EncodingText *encodingText, char* currentChar);

extern CharEncoding* getEncoding(CharEncodingDictionary *dict, char character);
extern void getEncodingFromTree(CharEncodingDictionary *encodingDict, CharFreqDictionary *charFreqDict, TreeNode *root); 
extern void encodeStringToByteArray(EncodingText *encodingText, CharEncodingDictionary* encodingDict, char *text, int total_chars);
extern void mergeEncodedText(EncodingText *dst, EncodingText *src);

// maybe we don't need if we use the dimensions instead of the positions
extern void appendToEncodingText(EncodingText *encodingText, CharEncoding *charEncoding, char character);

extern char* decodeFromFile(FILE *fp, TreeNode *root, int bytesToProcess, int numberOfChars);

extern void printEncodings(CharEncodingDictionary* dict);
extern void printEncodedText(BYTE *text, int length);
