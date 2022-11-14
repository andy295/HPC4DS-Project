

void print_list(struct LinkedListTreeNodeItem* start){
	while(start->next != NULL){
		printf("%c: %d\n", start->item->letter, start->item->frequency); 
		start = start->next; 
	}
	printf("%c: %d\n", start->item->letter, start->item->frequency); 
}

void print_tree(struct TreeNode* root){
	if (root != NULL){
		printf("%c, %d\n",root->letter, root->frequency); 

		print_tree(root->leftChild); 
		print_tree(root->rightChild); 
	}
}

void print_binary(char c){
	for (int i = 0; i < 8; i++){
		printf("%d", (c >> i) & 1); 
	}

	printf("\n");
}
