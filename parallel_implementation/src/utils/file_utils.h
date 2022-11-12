#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> // get current directory

#include "../include/global_constants.h"

extern int get_file_size(char* fileName);
extern long read_file(const char* fileName, char** fileDest, int processId);
// extern void append_string_to_binary_file(char* string, FILE* fp, int* charIndex, char* currentChar);
extern void printPos(int processId);
