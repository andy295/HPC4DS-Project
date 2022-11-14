
char* substring(char* string, int position, int length) {
	char* pointer;
	int c;

	pointer = malloc(length + 1);

	if (pointer == NULL) {
		printf("Unable to allocate memory.\n");
		exit(1);
	}

	for (c = 0; c < length; c++) {
		*(pointer + c) = *(string + position - 1);
		string++;
	}

	*(pointer + c) = '\0';

	return pointer;
}

