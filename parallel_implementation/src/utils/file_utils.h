#pragma once

#include "../include/global_constants.h"

enum OpenMode {
    READ = 0,
    WRITE = 1,
    APPEND = 2,
    WRITE_AT = 3
};

static const int FILE_HEADER_ELEMENTS = 1;

typedef struct FileHeader {
    unsigned int arrayPosStartPos; 
} FileHeader;

extern int getFileSize(char* fileName);
extern long readFilePortionForProcess(const char* fileName, char** fileDest, int processId, int proc_number);
extern void writeBufferToFile(char *filename, BYTE *buffer, int bufferSize, int openMode, int bytePosition);

extern void printWorkDir(int processId);

// extern void append_string_to_binary_file(char* string, FILE* fp, int* charIndex, char* currentChar);
