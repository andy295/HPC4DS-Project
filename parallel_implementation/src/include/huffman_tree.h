#pragma once

#include "global_constants.h"

#include "char_freq.h"
#include "../utils/print_utils.h"

#include <stdlib.h>

typedef struct TreeNode {
	int frequency; 
	char character; 

	struct TreeNode *leftChild; 
	struct TreeNode *rightChild; 
} TreeNode; 

typedef struct LinkedListTreeNodeItem {
	TreeNode *item; 
	struct LinkedListTreeNodeItem *next; 
} LinkedListTreeNodeItem;

LinkedListTreeNodeItem* new_node(char character, int frequency);
LinkedListTreeNodeItem* get_min_freq(LinkedListTreeNodeItem *start);
LinkedListTreeNodeItem* ordered_append_to_freq(LinkedListTreeNodeItem *start, LinkedListTreeNodeItem *item);
LinkedListTreeNodeItem* create_linked_list(CharFreqDictionary *dict);

extern TreeNode* createHuffmanTree(CharFreqDictionary *dict);
extern void printHuffmanTree(TreeNode* root, int depth);
