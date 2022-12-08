#pragma once

#include "global_constants.h"

#include "char_freq.h"
#include "huffman_tree.h"

#include "../utils/print_utils.h"

typedef struct CharEncoding {
	char character; 
	unsigned int length;
	char* encoding; 
} CharEncoding;

typedef struct CharEncodingDictionary {
	int number_of_chars; 
	CharEncoding *charEncoding; 
} CharEncodingDictionary;

extern void getEncodingFromTree(CharEncodingDictionary *encodingDict, CharFreqDictionary *charFreqDict, TreeNode *root); 

extern bool findEncodingFromTree(char character, TreeNode *root, CharEncoding *dst, unsigned int depth);
extern void printEncodings(CharEncodingDictionary* dict);
