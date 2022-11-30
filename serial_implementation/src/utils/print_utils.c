

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

void printCharFreqs(struct LetterFreqDictionary* dict) {
	// int total_characters = 0;
	printf("Dictionary: \n");
	for (int i = 0; i < dict->number_of_letters; i++) {
		
		#if VERBOSE <= 3
			switch (dict->letterFreqs[i].letter)
			{
			case '\n':
				printf("\tcharacter: \\n\tfrequence: %d\n",
					dict->letterFreqs[i].frequency);
				break;
			case '\0':
				printf("\tcharacter: \\0\tfrequence: %d\n",
					dict->letterFreqs[i].frequency);
				break;
			case '\t':
				printf("\tcharacter: \\t\tfrequence: %d\n",
					dict->letterFreqs[i].frequency);
				break;
			case ' ':
				printf("\tcharacter: space\tfrequence: %d\n",
					dict->letterFreqs[i].frequency);
				break;		
			default:
				printf("\tcharacter: %c\tfrequence: %d\n",
					dict->letterFreqs[i].letter,
					dict->letterFreqs[i].frequency);
				break;
			}
		#endif
		// total_characters += dict->charFreqs[i].frequency;
	}
}