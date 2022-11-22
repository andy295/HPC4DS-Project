#pragma once

#include "global_constants.h"

#include "char_freq.h"

typedef struct TreeNode_tag {
	int frequency; 
	char character; 

	struct TreeNode_tag *leftChild; 
	struct TreeNode_tag *rightChild; 
} TreeNode; 

typedef struct LinkedListTreeNodeItem_tag {
	TreeNode *item; 
	struct LinkedListTreeNodeItem_tag *next; 
} LinkedListTreeNodeItem;

LinkedListTreeNodeItem* new_node(char character, int frequency);
LinkedListTreeNodeItem* get_min_freq(LinkedListTreeNodeItem *start);
LinkedListTreeNodeItem* ordered_append_to_freq(LinkedListTreeNodeItem *start, LinkedListTreeNodeItem *item);
LinkedListTreeNodeItem* create_linked_list(CharFreqDictionary *dict);

extern TreeNode* create_huffman_tree(CharFreqDictionary *dict);