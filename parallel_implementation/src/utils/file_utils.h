#pragma once

#include "../include/global_constants.h"

extern int getFileSize(char* fileName);
extern long readFilePortionForProcess(const char* fileName, char** fileDest, int processId, int proc_number);
// extern void append_string_to_binary_file(char* string, FILE* fp, int* charIndex, char* currentChar);
extern void printWorkDir(int processId);
