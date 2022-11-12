#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "global_constants.h"

typedef struct CharFreq {
	int frequency;
	char character; 
} CharFreq; 

typedef struct CharFreqDictionary {
	int number_of_chars; 
	CharFreq *charFreqs; 
} CharFreqDictionary;


void init_char_freq_dictionary(CharFreqDictionary* dict, char* text, int count);
// void append_to_freq(struct LetterFreqDictionary* dict, char letter, int freq);
// extern void append_to_freq_dict(struct LetterFreqDictionary* dict, char letter);
// extern void sort_freqs(struct LetterFreqDictionary* res);
// extern void merge_letter_freqs(struct LetterFreqDictionary* allLetters, struct LetterFreqDictionary* letterFreqs);