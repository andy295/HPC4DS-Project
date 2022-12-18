#include "file_utils.h"

int getFileSize(const char* fileName) {
	FILE* fp = openFile(fileName, READ_CHARS, 0); 
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
	FILE* fp = openFile(fileName, READ_CHARS, 0); 
	if (fp != NULL) {
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
void writeBufferToFile(const char *filename, BYTE *buffer, int bufferSize, int openMode, int bytePosition) {
	FILE *file = openFile(filename, openMode, bytePosition);
	fwrite(buffer, sizeof(BYTE), bufferSize, file);
	fclose(file);
}


FILE* openFile(const char* filename, int openMode, int bytePosition) {
	FILE* file; 

	switch (openMode) {
		case READ_CHARS:
			file = fopen(filename, "r"); 
			fseek(file, bytePosition, SEEK_END); 
			break;
		case READ_BYTES:
			file = fopen(filename, "rb"); 
			fseek(file, bytePosition, SEEK_END); 
			break;
		
		case WRITE:
			file = fopen(filename, "wb"); 
			break;
		
		case APPEND:
			file = fopen(filename, "ab+");
			break;

		case WRITE_AT:
			file = fopen(filename, "r+b");
			fseek(file, bytePosition, SEEK_SET);
			break;
		
		default:
			printf("Error while opening file %s, unknown open mode: %d\n", filename, openMode);
			return NULL;
	}

	if (file == NULL) {
		printf("Error while opening file %s\n", filename);
		return NULL;
	}

	return file;
}