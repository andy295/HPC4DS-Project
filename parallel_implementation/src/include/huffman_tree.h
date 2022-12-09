#pragma once

#include "global_constants.h"

#include "char_freq.h"

#include "../utils/print_utils.h"

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

LinkedListTreeNodeItem* newNode(char character, int frequency);
LinkedListTreeNodeItem* getMinFreq(LinkedListTreeNodeItem *start);
LinkedListTreeNodeItem* orderedAppendToFreq(LinkedListTreeNodeItem *start, LinkedListTreeNodeItem *item);
LinkedListTreeNodeItem* createLinkedList(CharFreqDictionary *dict);

extern LinkedListTreeNodeItem* createHuffmanTree(CharFreqDictionary* dict);

extern void printHuffmanTree(TreeNode* root, int depth);
void print(TreeNode* root, int depth);
void print2D(TreeNode* root);
void print2DUtil(TreeNode* root, int space);

extern BYTE* encodeTreeToByteArray(TreeNode* root, int* byteSizeOfTree);