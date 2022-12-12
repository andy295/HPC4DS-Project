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

typedef struct {
	BYTE character;
	BYTE leftChild;
	BYTE rightChild;
} TreeArrayItem;

LinkedListTreeNodeItem* newNode(char character, int frequency);
LinkedListTreeNodeItem* getMinFreq(LinkedListTreeNodeItem *start);
LinkedListTreeNodeItem* orderedAppendToFreq(LinkedListTreeNodeItem *start, LinkedListTreeNodeItem *item);
LinkedListTreeNodeItem* createLinkedList(CharFreqDictionary *dict);

int countTreeNodes(TreeNode* root);
void encodeTree(TreeNode* root, TreeArrayItem* treeArray, int* globalIndex);

void print(TreeNode* root, int depth);
void print2D(TreeNode* root);
void print2DUtil(TreeNode* root, int space);

extern LinkedListTreeNodeItem* createHuffmanTree(CharFreqDictionary* dict);
extern BYTE* encodeTreeToByteArray(TreeNode* root, int* byteSizeOfTree);
extern void printHuffmanTree(TreeNode* root, int depth);
extern int getByteSizeOfTree(TreeNode* root); 