#pragma once

#include "../include/global_constants.h"

#include "../include/huffman_tree.h"

enum OpenMode {
    READ = 0,
    READ_ALL,
    WRITE,
    APPEND,
    WRITE_AT,
    READ_B,
    READ_ALL_B,
    WRITE_B,
    APPEND_B,
    WRITE_B_AT
};

static const int FILE_HEADER_ELEMENTS = 1;

typedef struct FileHeader {
    unsigned int byteStartOfDimensionArray; 
} FileHeader;

extern FILE* openFile(const char *filename, int openMode, int bytePosition);
extern int getFileSize(const char *fileName);
extern long readFilePortionForProcess(const char *fileName, char **fileDest, int processId, int proc_number);
extern void writeBufferToFile(const char *filename, BYTE *buffer, int bufferSize, int openMode, int bytePosition); 
extern void parseHeader(FileHeader *header, FILE *fp);
extern unsigned short* parseBlockLengths(FILE* fp, int numberOfBlocks);

extern int readEncodedFile(const char *fileName, FILE *fp, TreeNode *root, unsigned short *blockLengths);

extern void printWorkDir(int processId);
