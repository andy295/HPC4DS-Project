#pragma once

#include "global_constants.h"

#include "../utils/print_utils.h"

enum Position {
    FIRST = 0,
    LAST,
	LAST_R
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
extern void mergeCharFreqs(CharFreqDictionary *dst, CharFreqDictionary *src, int pos);
extern void printCharFreqs(CharFreqDictionary *dict);
