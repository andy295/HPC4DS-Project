#pragma once

#include "../include/global_constants.h"

enum OpenMode {
    READ_CHARS = 0,
    READ_BYTES = 1,
    WRITE = 2,
    APPEND = 3,
    WRITE_AT = 4
};

static const int FILE_HEADER_ELEMENTS = 1;

typedef struct FileHeader {
    unsigned int byteSizeOfPositionArray; 
} FileHeader;

FILE* openFile(const char* filename, int openMode, int bytePosition);

extern int getFileSize(const char* fileName);
extern long readFilePortionForProcess(const char* fileName, char** fileDest, int processId, int proc_number);
extern void writeBufferToFile(const char *filename, BYTE *buffer, int bufferSize, int openMode, int bytePosition); 

extern void printWorkDir(int processId);

// extern void append_string_to_binary_file(char* string, FILE* fp, int* charIndex, char* currentChar);
