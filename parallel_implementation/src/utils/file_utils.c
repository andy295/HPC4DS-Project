#include "file_utils.h"

int getFileSize(char* fileName) {
	FILE* fp = fopen(fileName, "r"); 
	fseek(fp, 0, SEEK_END); 
	long fSize = ftell(fp); 
	fseek(fp, 0, SEEK_SET); 

	fclose(fp); 

	return fSize * 8; 
}

void printWorkDir(int processId) {
	char cwd[2048];
   	if (getcwd(cwd, sizeof(cwd)) != NULL) {
    	printf("Process %d current working dir: %s\n", processId, cwd);
   	} else {
		perror("getcwd() error");
   	}
}

long readFilePortionForProcess(const char* fileName, char** fileDest, int processId, int proc_number) {
	FILE* fp = fopen(fileName, "r"); 
	if (fp != NULL) {
		fseek(fp, 0, SEEK_END); 
		long fSize = ftell(fp);

		int residual = fSize % proc_number; 
		fSize = fSize / proc_number;
		fseek(fp, processId * fSize, SEEK_SET);

		// quick and dirty solution 
		// but i don't have a better idea right now
		if (processId == proc_number - 1 && residual != 0)
			fSize += residual;

		*fileDest = malloc(fSize + 1); 
		fread((*fileDest), fSize, 1, fp);
		fclose(fp); 

		(*fileDest)[fSize] = '\0';

		#if VERBOSE <= 1
			printf("process %d:\n%s\n", processId, *fileDest);
			printf("total text length: %ld\n", fSize);
		#endif

		return fSize;
	}

	printf("Error opening file %s\n", fileName);
	return 0; 
}

// void append_string_to_binary_file(char* string, FILE* fp, int* charIndex, char* currentChar) {
// 	int strlength = strlen(string);
// 	for (int k = 0; k < strlength; k++) {

// 		if (string[k] == '1')
// 			*currentChar |= 1 << *charIndex;

// 		if (*charIndex == 7) {
// 			fwrite(currentChar, sizeof(char), 1, fp);
// 			*charIndex = 0;
// 			*currentChar = 0;
// 		} else
// 			*charIndex += 1;
// 	}
// }
