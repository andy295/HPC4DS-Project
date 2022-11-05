
struct TreeNode {
	int frequency; 
	char letter; 

	struct TreeNode* leftChild; 
	struct TreeNode* rightChild; 
}; 

struct LinkedListTreeNodeItem {
	struct TreeNode* item; 
	struct LinkedListTreeNodeItem* next; 
}; 

struct LinkedListTreeNodeItem* new_node(char letter, int freq){
	struct TreeNode* r = malloc(sizeof(struct TreeNode)); 
	r->letter = letter; 
	r->frequency = freq; 
	r->rightChild = NULL; 
	r->leftChild = NULL; 

	struct LinkedListTreeNodeItem* listItem = malloc(sizeof(struct LinkedListTreeNodeItem));
	listItem->item = r;
	listItem->next = NULL;   
	return listItem; 
}

struct LinkedListTreeNodeItem* get_min_freq(struct LinkedListTreeNodeItem* start){
	struct LinkedListTreeNodeItem* temp = new_node(start->item->letter, start->item->frequency); 
	temp->item->leftChild = start->item->leftChild;  
	temp->item->rightChild = start->item->rightChild;  
	return temp;
}

struct LinkedListTreeNodeItem* ordered_append_to_freq(struct LinkedListTreeNodeItem* start, struct LinkedListTreeNodeItem* item){
	struct LinkedListTreeNodeItem* prev = NULL; 
	struct LinkedListTreeNodeItem* firstStart = start; 

	if (item->item->frequency < start->item->frequency){
		item->next = start; 
		return item; 
	}

	while(start->next != NULL){
		if (item->item->frequency < start->item->frequency){
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

struct LinkedListTreeNodeItem* create_linked_list(struct LetterFreqDictionary* res) {
	struct LinkedListTreeNodeItem* start = new_node(res->letterFreqs[0].letter, res->letterFreqs[0].frequency); 

	struct LinkedListTreeNodeItem* old; 
	old = start; 
	for (int i = 1; i < res->number_of_letters; i++)	{
		struct LinkedListTreeNodeItem* temp = new_node(res->letterFreqs[i].letter, res->letterFreqs[i].frequency); 
		old->next = temp; 
		old = temp; 
	}

	return start; 
}

struct TreeNode* create_huffman_tree(struct LetterFreqDictionary* res){
	struct LinkedListTreeNodeItem* start = create_linked_list(res); 

	do{
		struct TreeNode* left = get_min_freq(start)->item; 
		struct LinkedListTreeNodeItem* oldStart = start; 
		start = start->next; 
		free(oldStart); 

		struct TreeNode* right = get_min_freq(start)->item; 
		oldStart = start;  
		start = start->next; 
		free(oldStart); 

		struct LinkedListTreeNodeItem* top = new_node('$', left->frequency + right->frequency); 
		top->item->leftChild = left; 
		top->item->rightChild = right; 

		if (start == NULL) {
			start = top; 
		} else {
			start = ordered_append_to_freq(start, top); 
		}

	} while(start->next != NULL); 

	return start->item; 
}
