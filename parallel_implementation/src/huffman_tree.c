#include "include/huffman_tree.h"

LinkedListTreeNodeItem* newNode(char character, int frequency) {
	TreeNode* r = malloc(sizeof(TreeNode)); 
	r->character = character; 
	r->frequency = frequency; 
	r->rightChild = NULL; 
	r->leftChild = NULL; 

	LinkedListTreeNodeItem* listItem = malloc(sizeof(LinkedListTreeNodeItem));
	listItem->item = r;
	listItem->next = NULL;

	return listItem; 
}

LinkedListTreeNodeItem* getMinFreq(LinkedListTreeNodeItem* start) {
	LinkedListTreeNodeItem* temp = newNode(start->item->character, start->item->frequency); 
	temp->item->leftChild = start->item->leftChild;  
	temp->item->rightChild = start->item->rightChild;  

	return temp;
}

LinkedListTreeNodeItem* orderedAppendToFreq(LinkedListTreeNodeItem* start, LinkedListTreeNodeItem* item) {
	LinkedListTreeNodeItem* prev = NULL; 
	LinkedListTreeNodeItem* firstStart = start; 

	if (item->item->frequency < start->item->frequency) {
		item->next = start; 

		return item; 
	}

	while(start->next != NULL) {
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

LinkedListTreeNodeItem* createLinkedList(CharFreqDictionary* dict) {
	LinkedListTreeNodeItem* start = newNode(dict->charFreqs[0].character, dict->charFreqs[0].frequency); 

	LinkedListTreeNodeItem* old; 
	old = start; 
	for (int i = 1; i < dict->number_of_chars; i++)	{
		LinkedListTreeNodeItem* temp = newNode(dict->charFreqs[i].character, dict->charFreqs[i].frequency); 
		old->next = temp; 
		old = temp; 
	}

	return start; 
}

TreeNode* createHuffmanTree(CharFreqDictionary* dict, int *size) {
	LinkedListTreeNodeItem* start = createLinkedList(dict); 
	*size = 0;

	do {
		TreeNode* left = getMinFreq(start)->item; 
		LinkedListTreeNodeItem* oldStart = start; 
		start = start->next; 
		free(oldStart); 

		TreeNode* right = getMinFreq(start)->item; 
		oldStart = start;  
		start = start->next; 
		free(oldStart); 

		LinkedListTreeNodeItem* top = newNode('$', left->frequency + right->frequency); 
		top->item->leftChild = left; 
		top->item->rightChild = right; 

		if (start == NULL)
			start = top; 
		else
			start = orderedAppendToFreq(start, top); 

		++(*size);
	} while (start->next != NULL); 

	return start->item; 
}

void printHuffmanTree(TreeNode* root, int depth) {
	if (TREE_PRINT_TYPE == 0)
		print(root, depth);
	else
		print2D(root);
}

void print(TreeNode* root, int depth) {
	if (root == NULL)
		return; 

	for (int i = 0; i < depth; i++)
		printf(" ");

	printFormattedChar(root->character); 
	printf(": %d\n", root->frequency);

	printHuffmanTree(root->leftChild, depth+1);	
	printHuffmanTree(root->rightChild, depth+1);
}

// Wrapper over print2DUtil()
void print2D(TreeNode* root)
{
	// Pass initial space count as 0
	print2DUtil(root, 0);
}

// Function to print binary tree in 2D
// It does reverse inorder traversal
void print2DUtil(TreeNode* root, int space)
{
	// Base case
	if (root == NULL)
		return;

	// Increase distance between levels
	space += COUNT;

	// Process right child first
	print2DUtil(root->rightChild, space);

	// Print current node after space
	// count
	printf("\n");
	for (int i = COUNT; i < space; i++)
		printf(" ");
	printf("%c - %d\n", root->character, root->frequency);

	// Process left child
	print2DUtil(root->leftChild, space);
}