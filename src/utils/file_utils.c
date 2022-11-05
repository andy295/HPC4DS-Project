

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
