#include "include/char_freq.h"

// maybe we could improve the function by using the multythrading for loop
void init_char_freq_dictionary(CharFreqDictionary* dict, char text[], int len) {
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

	#ifdef VERBOSE
		printf("dictionary:\n");
		int total_characters = 0;
		for (i = 0; i < dict->number_of_chars; i++) {
			printf("\tcharacter: %c\tfrequence: %d\n",
			dict->charFreqs[i].character,
			dict->charFreqs[i].frequency);

			total_characters += dict->charFreqs[i].frequency;
		}

		printf("number of characters: %d\n", total_characters);
	#endif
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

// void merge_letter_freqs(struct LetterFreqDictionary* allLetters, struct LetterFreqDictionary* letterFreqs){

// 	for (int i = 0; i < letterFreqs->number_of_letters; i++){
// 		int found = 0; 
// 		for (int j = 0; j < allLetters->number_of_letters; j++){
// 			if (allLetters->letterFreqs[j].letter == letterFreqs->letterFreqs[i].letter){
// 				allLetters->letterFreqs[j].frequency += letterFreqs->letterFreqs[i].frequency; 
// 				found = 1; 
// 				break; 
// 			}
// 		}

// 		if (found == 0){
// 			allLetters->letterFreqs[allLetters->number_of_letters].letter = letterFreqs->letterFreqs[i].letter; 
// 			allLetters->letterFreqs[allLetters->number_of_letters].frequency = letterFreqs->letterFreqs[i].frequency; 
// 			allLetters->number_of_letters++; 
// 		}
// 	}
// }
