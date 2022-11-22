#pragma once

#include "global_constants.h"

#include "char_freq.h"
#include "message.h"
#include "huffman_tree.h"

#include "../utils/file_utils.h"
// #include <utils/time_utils.c>
// #include <utils/print_utils.c>

typedef struct CharEncoding {
	char character;
	char* encoding;
} CharEncoding;

const char *fileName = "text.txt";

void freeBuffer(void* buffer);
void get_encoding_from_tree(CharFreqDictionary* dict, TreeNode* root, CharEncoding *encodings);
bool find_encoding(char character, TreeNode* root, char* dst, int depth);
