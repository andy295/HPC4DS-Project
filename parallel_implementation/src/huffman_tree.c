#include "include/huffman_tree.h"

LinkedListTreeNodeItem* new_node(char character, int frequency) {
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

LinkedListTreeNodeItem* get_min_freq(LinkedListTreeNodeItem* start) {
	LinkedListTreeNodeItem* temp = new_node(start->item->character, start->item->frequency); 
	temp->item->leftChild = start->item->leftChild;  
	temp->item->rightChild = start->item->rightChild;  

	return temp;
}

LinkedListTreeNodeItem* ordered_append_to_freq(LinkedListTreeNodeItem* start, LinkedListTreeNodeItem* item) {
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

LinkedListTreeNodeItem* create_linked_list(CharFreqDictionary* dict) {
	LinkedListTreeNodeItem* start = new_node(dict->charFreqs[0].character, dict->charFreqs[0].frequency); 

	LinkedListTreeNodeItem* old; 
	old = start; 
	for (int i = 1; i < dict->number_of_chars; i++)	{
		LinkedListTreeNodeItem* temp = new_node(dict->charFreqs[i].character, dict->charFreqs[i].frequency); 
		old->next = temp; 
		old = temp; 
	}

	return start; 
}

TreeNode* create_huffman_tree(CharFreqDictionary* dict) {
	LinkedListTreeNodeItem* start = create_linked_list(dict); 

	do {
		TreeNode* left = get_min_freq(start)->item; 
		LinkedListTreeNodeItem* oldStart = start; 
		start = start->next; 
		free(oldStart); 

		TreeNode* right = get_min_freq(start)->item; 
		oldStart = start;  
		start = start->next; 
		free(oldStart); 

		LinkedListTreeNodeItem* top = new_node('$', left->frequency + right->frequency); 
		top->item->leftChild = left; 
		top->item->rightChild = right; 

		if (start == NULL)
			start = top; 
		else
			start = ordered_append_to_freq(start, top); 

	} while(start->next != NULL); 

	return start->item; 
}
