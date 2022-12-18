#pragma once

#include "global_constants.h"

#include "char_freq.h"

#include "../utils/print_utils.h"

enum Childern {
    LEFT = 0,
    RIGHT = 1,

	MAX_CHILDREN = 8
};

typedef struct TreeNode_t {
	int frequency; 
	char character; 

	struct TreeNode_t *leftChild; 
	struct TreeNode_t *rightChild; 
} TreeNode; 

typedef struct LinkedListTreeNodeItem_t {
	TreeNode *item; 
	struct LinkedListTreeNodeItem_t *next; 
} LinkedListTreeNodeItem;

typedef struct TreeArrayItem_t {
	BYTE character;
	BYTE children;
} TreeArrayItem;

LinkedListTreeNodeItem* newNode(char character, int frequency);
LinkedListTreeNodeItem* getMinFreq(LinkedListTreeNodeItem *start);
LinkedListTreeNodeItem* orderedAppendToFreq(LinkedListTreeNodeItem *start, LinkedListTreeNodeItem *item);
LinkedListTreeNodeItem* createLinkedList(CharFreqDictionary *dict);

int countTreeNodes(TreeNode *root);
void encodeTree(TreeNode *root, TreeArrayItem *treeArray, int *globalIndex);

void print(TreeNode *root, int depth);
void print2D(TreeNode *root);
void print2DUtil(TreeNode *root, int space);

extern LinkedListTreeNodeItem* createHuffmanTree(CharFreqDictionary *dict);
extern BYTE* encodeTreeToByteArray(TreeNode *root, int *byteSizeOfTree);
extern void printHuffmanTree(TreeNode *root, int depth);
extern int getByteSizeOfTree(TreeNode *root);
