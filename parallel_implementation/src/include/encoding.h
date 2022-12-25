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
void copyEncodedText(EncodingText *encodingText, char *currentChar);
char* appendCharacter(char *text, char c, int *idx);

extern CharEncoding* getEncoding(CharEncodingDictionary *dict, char character);
extern void getEncodingFromTree(CharEncodingDictionary *encodingDict, CharFreqDictionary *charFreqDict, TreeNode *root); 
extern void encodeStringToByteArray(EncodingText *encodingText, CharEncodingDictionary* encodingDict, char *text, int total_chars);
extern void mergeEncodedText(EncodingText *dst, EncodingText *src);

extern char* decodeFromFile(int startByte, unsigned short *dimensions, int blockStart, int blockNr, FILE *fp, TreeNode *root);
extern void mergeDecodedText(char *dst, DecodingText *src, int *dstLength);

extern void printEncodings(CharEncodingDictionary* dict);
extern void printEncodedText(BYTE *text, int length);
