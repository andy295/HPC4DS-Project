#pragma once

#include "../include/global_constants.h"

enum OpenMode {
    READ = 0,
    WRITE = 1,
    APPEND = 2,
    WRITE_AT = 3,
    READ_B = 4,
    WRITE_B = 5,
    APPEND_B = 6,
    WRITE_B_AT = 7
};

static const int FILE_HEADER_ELEMENTS = 1;

typedef struct FileHeader {
    unsigned int arrayPosStartPos; 
} FileHeader;

extern FILE* openFile(const char *filename, int openMode, int bytePosition);
extern int getFileSize(const char *fileName);
extern long readFilePortionForProcess(const char *fileName, char **fileDest, int processId, int proc_number);
extern void writeBufferToFile(const char *filename, BYTE *buffer, int bufferSize, int openMode, int bytePosition); 

extern void printWorkDir(int processId);
