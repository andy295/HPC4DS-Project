#include "include/char_freq.h"

void copy_char_Freq(CharFreq *dest, CharFreq *src) {
	dest->character = src->character;
	dest->frequency = src->frequency;
}

// maybe we could improve the function by using the multythrading for loop
void getCharFreqsFromText(CharFreqDictionary* dict, char text[], int len, int pid) {
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
}

void printCharFreqs(CharFreqDictionary* dict) {
	int i;

	int total_characters = 0;
	printf("Dictionary: \n");
	for (i = 0; i < dict->number_of_chars; i++) {
		
		#if VERBOSE <= 3
			switch (dict->charFreqs[i].character)
			{
			case '\n':
				printf("\tcharacter: \\n\tfrequence: %d\n",
					dict->charFreqs[i].frequency);
				break;
			case '\0':
				printf("\tcharacter: \\0\tfrequence: %d\n",
					dict->charFreqs[i].frequency);
				break;
			case '\t':
				printf("\tcharacter: \\t\tfrequence: %d\n",
					dict->charFreqs[i].frequency);
				break;
			case ' ':
				printf("\tcharacter: space\tfrequence: %d\n",
					dict->charFreqs[i].frequency);
				break;		
			default:
				printf("\tcharacter: %c\tfrequence: %d\n",
					dict->charFreqs[i].character,
					dict->charFreqs[i].frequency);
				break;
			}
		#endif

		total_characters += dict->charFreqs[i].frequency;
	}
}

// use a special character to indicate the end of the file
void append_to_freq(CharFreqDictionary* dict, char character, int pos) {
	if (pos == FIRST) {
		CharFreq *tmp = malloc(dict->number_of_chars * sizeof(CharFreq));
		memcpy(tmp, dict->charFreqs, dict->number_of_chars * sizeof(CharFreq));

		free(dict->charFreqs);
		++dict->number_of_chars;
		dict->charFreqs = malloc(dict->number_of_chars * sizeof(CharFreq));

		memcpy(dict->charFreqs + 1, tmp, (dict->number_of_chars) * sizeof(CharFreq));

		dict->charFreqs[0].character = character;
		dict->charFreqs[0].frequency = 1;
	}
	else {
		int frequency = dict->charFreqs[dict->number_of_chars - 1].frequency + 1;
		dict->charFreqs = realloc(dict->charFreqs, sizeof(CharFreq) * (dict->number_of_chars + 1));
		dict->charFreqs[dict->number_of_chars] = (struct CharFreq) {.character = character, .frequency = frequency};
		++dict->number_of_chars;
	}
}

void merge_char_freqs(CharFreqDictionary* dict, CharFreq* charFreqs, int size) {
	int i, j;
	for (i = 0; i < size; i++) {
		char character = charFreqs[i].character;
		int frequency = charFreqs[i].frequency;
		bool assigned = false;
		for (j = 0; j < dict->number_of_chars && !assigned; j++) {
			if (dict->charFreqs[j].character == character) {
				dict->charFreqs[j].frequency += frequency;
				assigned = true;
			}
		}

		if (!assigned) {
			append_to_freq(dict, character, LAST);
		}
	}
}

void sort_freqs(CharFreqDictionary* res) {
	int i, j;

	for (i = 0; i < res->number_of_chars; i++) {
		CharFreq minCharFreq = res->charFreqs[i];
		int indexOfMin = i;  
		for (j = i; j < res->number_of_chars; j++){
			if (minCharFreq.frequency > res->charFreqs[j].frequency) {
				minCharFreq = res->charFreqs[j]; 
				indexOfMin = j; 
			}
		}

		CharFreq temp = res->charFreqs[i]; 
		res->charFreqs[i] = minCharFreq;
		res->charFreqs[indexOfMin] = temp;
	}
}