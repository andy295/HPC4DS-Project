#include "include/char_freq.h"

// maybe we could improve the function by using the multythrading for loop
void get_chars_freqs(CharFreqDictionary* dict, char text[], int len) {
	int i, j;
	for (i = 0; i < len; i++) {
		char character = text[i]; 
		bool assigned = false; 
		for (j = 0; j < dict->number_of_chars && !assigned; j++) {

            CharFreq *p = &dict->charFreqs[j];
			if (p->character == character) {
				++p->frequency; 
				assigned = true; 
			}
		}

		if (!assigned) {
            ++dict->number_of_chars;
            dict->charFreqs = realloc(dict->charFreqs, sizeof(CharFreq) * dict->number_of_chars);

            CharFreq *p = &dict->charFreqs[dict->number_of_chars - 1];
			*p = (struct CharFreq) {.character = character, .frequency = 1};
		}
	}

	#if VERBOSE == 2
		print_dictionary(dict);
	#endif
}

void print_dictionary(CharFreqDictionary* dict) {
	int i;
	printf("dictionary:\n");
	int total_characters = 0;
	for (i = 0; i < dict->number_of_chars; i++) {
		printf("\tcharacter: %c\tfrequence: %d\n",
		dict->charFreqs[i].character,
		dict->charFreqs[i].frequency);

		total_characters += dict->charFreqs[i].frequency;
	}

	printf("number of characters: %d\n", total_characters);
}

void init_char_freq_dictionary(CharFreqDictionary* dict, int size) {
	dict->charFreqs = malloc(sizeof(CharFreq) * size); 
	dict->number_of_chars = 0; 
}

// 	// use # as a special character to indicate the end of the file (commento da copilot, complimenti)
// 	res->letterFreqs[res->number_of_letters].letter = '#'; 
// 	res->letterFreqs[res->number_of_letters].frequency = 1000; 
// 	res->number_of_letters += 1; 
// }

// void sort_freqs(struct LetterFreqDictionary* res){
// 	for (int i = 0; i < res->number_of_letters; i++) {
		
// 		struct LetterFreq minLetterFreq = res->letterFreqs[i];
// 		int indexOfMin = i;  
// 		for (int j = i; j < res->number_of_letters; j++){
// 			if (minLetterFreq.frequency > res->letterFreqs[j].frequency){
// 				minLetterFreq = res->letterFreqs[j]; 
// 				indexOfMin = j; 
// 			}
// 		}

// 		struct LetterFreq temp = res->letterFreqs[i]; 
// 		res->letterFreqs[i] = minLetterFreq;
// 		res->letterFreqs[indexOfMin] = temp;
// 	}
// }

void merge_char_freqs(CharFreqDictionary* destDict, CharFreqDictionary* srcDict) {
	int i, j;
	for (i = 0; i < srcDict->number_of_chars; i++) {
		bool found = false; 
		for (j = 0; j < destDict->number_of_chars && !found; j++) {
			if (destDict->charFreqs[j].character == srcDict->charFreqs[i].character) {
				destDict->charFreqs[j].frequency += srcDict->charFreqs[i].frequency; 
				found = true; 
			}
		}

		if (!found) {
			++destDict->number_of_chars; 
            destDict->charFreqs = realloc(destDict->charFreqs, sizeof(CharFreq) * destDict->number_of_chars);

            CharFreq *p = &destDict->charFreqs[destDict->number_of_chars - 1];
			p->character = srcDict->charFreqs[i].character;
			p->frequency = srcDict->charFreqs[i].frequency;
		}
	}
}
