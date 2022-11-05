

int get_file_size(char* filename){
	FILE* fp = fopen(filename, "r"); 
	fseek(fp, 0, SEEK_END); 
	long fsize = ftell(fp); 
	fseek(fp, 0, SEEK_SET); 

	fclose(fp); 

	return fsize * 8; 
}

char* read_file(char* filename){
	FILE* fp = fopen(filename, "r"); 
	fseek(fp, 0, SEEK_END); 
	long fsize = ftell(fp); 
	fseek(fp, 0, SEEK_SET); 

	char* string = malloc(fsize + 1); 
	fread(string, fsize, 1, fp); 
	fclose(fp); 

	string[fsize] = '\0'; 

	return string; 
}

void append_string_to_binary_file(char* string, FILE* fp, int* charIndex, char* currentChar){
	int strlength = strlen(string);
	for (int k = 0; k < strlength; k++) {

		if (string[k] == '1') {
			*currentChar |= 1 << *charIndex;
		} 

		if (*charIndex == 7) {
			fwrite(currentChar, sizeof(char), 1, fp);
			*charIndex = 0;
			*currentChar = 0;
		} else {
			*charIndex += 1;
		}
	}
}
