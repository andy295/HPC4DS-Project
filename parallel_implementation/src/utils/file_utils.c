#include "file_utils.h"

int getFileSize(char* fileName) {
	FILE* fp = fopen(fileName, "r"); 
	fseek(fp, 0, SEEK_END); 
	long fSize = ftell(fp); 
	fseek(fp, 0, SEEK_SET); 

	fclose(fp); 

	return fSize; 
}

void printWorkDir(int processId) {
	char cwd[2048];
   	if (getcwd(cwd, sizeof(cwd)) != NULL) {
    	printf("Process %d current working dir: %s\n", processId, cwd);
   	} else
		perror("getcwd() error");
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

		(*fileDest)[fSize] = ENDTEXT;

		#if VERBOSE <= 1
			printf("process %d:\n%s\n", processId, *fileDest);
			printf("total text length: %ld\n", fSize);
		#endif

		return fSize;
	}

	printf("Error opening file %s\n", fileName);
	return 0; 
}

// function to write byte buffer to file
void writeBufferToFile(char *filename, BYTE *buffer, int bufferSize, int openMode, int bytePosition) {
	FILE *file; 

	switch (openMode) {
		case WRITE:
			file = fopen(filename, "wb"); 
			break;
		
		case APPEND:
			file = fopen(filename, "ab+");
			break;

		case WRITE_AT:
			file = fopen(filename, "ab+");
			fseek(file, bytePosition, SEEK_SET);
			break;
		
		default:
			printf("Error while opening file %s, unknown open mode: %d\n", filename, openMode);
			return;
	}

	if (file == NULL) {
		printf("Error while opening file %s\n", filename);
		return;
	}

	fwrite(buffer, sizeof(BYTE), bufferSize, file);
	fclose(file);
}
