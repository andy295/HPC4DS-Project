#pragma once

#include "global_constants.h"

#include "../utils/print_utils.h"

enum Position {
    FIRST = 0,
    LAST,
};

typedef struct CharFreq {
	int frequency;
	char character; 
} CharFreq;

typedef struct CharFreqDictionary {
	int number_of_chars; 
	CharFreq *charFreqs; 
} CharFreqDictionary;

extern void getCharFreqsFromText(CharFreqDictionary *dict, char text[], long len, int pid);
extern void sortCharFreqs(CharFreqDictionary *res);
extern void appendToCharFreqs(CharFreqDictionary *dict, CharFreq *charFreq, int pos);
extern void mergeCharFreqs(CharFreqDictionary *dict, CharFreq *charFreqs, int size);
extern void printCharFreqs(CharFreqDictionary *dict);
