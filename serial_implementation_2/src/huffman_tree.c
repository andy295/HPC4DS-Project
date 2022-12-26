#include "include/huffman_tree.h"

LinkedListTreeNodeItem* newNode(char character, int frequency) {
	TreeNode *r = malloc(sizeof(TreeNode)); 
	r->character = character; 
	r->frequency = frequency; 
	r->rightChild = NULL; 
	r->leftChild = NULL; 

	LinkedListTreeNodeItem *listItem = malloc(sizeof(LinkedListTreeNodeItem));
	listItem->item = r;
	listItem->next = NULL;

	return listItem; 
}

LinkedListTreeNodeItem* getMinFreq(LinkedListTreeNodeItem *start) {
	LinkedListTreeNodeItem *temp = newNode(start->item->character, start->item->frequency); 
	temp->item->leftChild = start->item->leftChild;  
	temp->item->rightChild = start->item->rightChild;  

	return temp;
}

LinkedListTreeNodeItem* orderedAppendToFreq(LinkedListTreeNodeItem *start, LinkedListTreeNodeItem *item) {
	LinkedListTreeNodeItem *prev = NULL; 
	LinkedListTreeNodeItem *firstStart = start; 

	if (item->item->frequency < start->item->frequency) {
		item->next = start; 

		return item; 
	}

	while (start->next != NULL) {
		if (item->item->frequency < start->item->frequency) {
			prev->next = item; 
			item->next = start; 

			return firstStart; 
		}

		prev = start; 
		start = start->next; 
	}

	start->next = item; 

	return firstStart;
}

LinkedListTreeNodeItem* createLinkedList(CharFreqDictionary *dict) {
	LinkedListTreeNodeItem *start = newNode(dict->charFreqs[0].character, dict->charFreqs[0].frequency); 

	LinkedListTreeNodeItem *old; 
	old = start; 
	for (int i = 1; i < dict->number_of_chars; i++)	{
		LinkedListTreeNodeItem *temp = newNode(dict->charFreqs[i].character, dict->charFreqs[i].frequency);
		old->next = temp; 
		old = temp; 
	}

	return start; 
}

LinkedListTreeNodeItem* createHuffmanTree(CharFreqDictionary *dict) {
	LinkedListTreeNodeItem *start = createLinkedList(dict); 

	do {
		TreeNode *left = getMinFreq(start)->item; 
		LinkedListTreeNodeItem *oldStart = start; 
		start = start->next; 
		free(oldStart); 

		TreeNode *right = getMinFreq(start)->item; 
		oldStart = start;  
		start = start->next; 
		free(oldStart); 

		LinkedListTreeNodeItem *top = newNode('$', left->frequency + right->frequency); 
		top->item->leftChild = left; 
		top->item->rightChild = right; 

		if (start == NULL)
			start = top; 
		else
			start = orderedAppendToFreq(start, top); 
	} while (start->next != NULL); 

	return start; 
}

int countTreeNodes(TreeNode *root) {
	if (root == NULL)
		return 0;

	return 1 + countTreeNodes(root->leftChild) + countTreeNodes(root->rightChild);
}

void encodeTree(TreeNode *root, TreeArrayItem *treeArray, int *globalIndex) {
	if (root == NULL)
		return;

	treeArray[*globalIndex].character = root->character;

	if (root->leftChild != NULL)
		SetBit(treeArray[*globalIndex].children, LEFT);
	
	if (root->rightChild != NULL)
		SetBit(treeArray[*globalIndex].children, RIGHT);

	if (root->leftChild != NULL)
		*globalIndex += 1;
	encodeTree(root->leftChild, treeArray, globalIndex);

	if (root->rightChild != NULL)
		*globalIndex += 1;
	encodeTree(root->rightChild, treeArray, globalIndex);
}

BYTE* encodeTreeToByteArray(TreeNode *root, int *byteSizeOfTree) {

	int treeSize = countTreeNodes(root);
	TreeArrayItem* treeArray = calloc(treeSize, sizeof(TreeArrayItem));
	*byteSizeOfTree = sizeof(TreeArrayItem) * treeSize;

	int globalIndex = 0;
	encodeTree(root, treeArray, &globalIndex);

	return (BYTE*)treeArray;
}

int getByteSizeOfTree(TreeNode *root) {
	return sizeof(TreeArrayItem) * countTreeNodes(root);
}

bool isNodeALeaf(TreeNode *node) {
	return node->leftChild == NULL && node->rightChild == NULL;
}

void freeTree(TreeNode *root) {
	if (root == NULL)
		return;

	freeTree(root->leftChild);
	freeTree(root->rightChild);

	free(root);
}

void freeLinkedList(LinkedListTreeNodeItem *root) {
	if (root == NULL)
		return;

	freeLinkedList(root->next);
	freeTree(root->item);
	free(root);
}

void printHuffmanTree(TreeNode *root, int depth) {
	switch (TREE_PRINT_TYPE) {
		case 0:
			print(root, depth, true);
			break;
		case 1:
			print(root, depth, false);
			break;
	}
}

void print(TreeNode *root, int depth,  bool printFreq) {
	if (root == NULL)
		return; 

	for (int i = 0; i < depth; i++)
		printf(" ");

	printFormattedChar(root->character); 

	if (printFreq)
		printf(": %d\n", root->frequency);
	else
		printf("\n");

	printHuffmanTree(root->leftChild, depth+1);	
	printHuffmanTree(root->rightChild, depth+1);
}
