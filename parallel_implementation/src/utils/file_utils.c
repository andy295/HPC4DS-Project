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

// void encodeToFile(char* text, CharEncoding* encodings, int unique_letters, int total_letters){
	
// 	FILE *fp;
// 	fp = fopen(ENCODED_FILE, "wb");

// 	int charIndex = 0; 
// 	char c = 0;
// 	for (int i = 0; i < total_letters; i++) {
// 		for (int j = 0; j < unique_letters; j++) {
// 			if (text[i] == encodings[j].letter) {
// 				append_string_to_binary_file(encodings[j].encoding, fp, &charIndex, &c); 
// 			}
// 		}
// 	}

// 	// appends and writes custom end of file character
// 	append_string_to_binary_file(encodings[unique_letters-1].encoding, fp, &charIndex, &c); 
// 	fwrite(&c, sizeof(char), 1, fp);

// 	fclose(fp);
// }

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

// void decode_from_file(struct TreeNode* root){

// 	FILE *fp2;
// 	fp2 = fopen(ENCODED_FILE, "rb");

// 	char c;
// 	char lastContinuousChar = 0;

// 	struct TreeNode* intermediateNode = root;

// 	int endReached = 0;
// 	while (fread(&c, sizeof(char), 1, fp2)) {

// 		if (endReached == 1) {
// 			break;
// 		}

// 		for (int i = 0; i < 8; i++) {
// 			if (intermediateNode->letter != '$') {

// 				if (intermediateNode->letter == '#'){
// 					endReached = 1;
// 					break;
// 				}

// 				if (VERBOSE){
// 					printf("%c", intermediateNode->letter);
// 				}

// 				intermediateNode = root;
// 			}

// 			if (c & (1 << i)) {
// 				intermediateNode = intermediateNode->rightChild;
// 			} else {
// 				intermediateNode = intermediateNode->leftChild;
// 			}
// 		}
// 	}

// 	fclose(fp2);
// }