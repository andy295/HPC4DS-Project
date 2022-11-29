#pragma once

#include "global_constants.h"

enum Position {
    FIRST = 1,
    LAST
};

typedef struct CharFreq {
	int frequency;
	char character; 
} CharFreq; 

typedef struct CharFreqDictionary {
	int number_of_chars; 
	CharFreq *charFreqs; 
} CharFreqDictionary;

extern void copy_char_Freq(CharFreq *dest, CharFreq *src);

extern void getCharFreqsFromText(CharFreqDictionary* dict, char text[], int len, int pid);

extern void sortFreqs(CharFreqDictionary* res);
extern void append_to_freq(CharFreqDictionary* dict, char character, int pos);

// extern void append_to_freq_dict(struct LetterFreqDictionary* dict, char letter);

extern void mergeCharFreqs(CharFreqDictionary* dict, CharFreq* charFreqs, int size); 

extern void printCharFreqs(CharFreqDictionary* dict);
