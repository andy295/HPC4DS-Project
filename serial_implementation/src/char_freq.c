#include "include/char_freq.h"

void getCharFreqsFromText(CharFreqDictionary *dict, char text[], long len, int pid) {
	for (int i = 0; i < len; i++) {
		char character = text[i]; 
		bool assigned = false; 
		for (int j = 0; j < dict->number_of_chars && !assigned; j++) {

            CharFreq *p = &dict->charFreqs[j];
			if (p->character == character) {
				++p->frequency; 
				assigned = true; 
			}
		}

		if (!assigned) {
            ++dict->number_of_chars;
            dict->charFreqs = realloc(dict->charFreqs, sizeof(CharFreq) * dict->number_of_chars);

            CharFreq *p = &dict->charFreqs[dict->number_of_chars-1];
			*p = (struct CharFreq) {.character = character, .frequency = 1};
		}
	}
}

void oddEvenSort(CharFreqDictionary *res) {
    int phase;
    int i;
    CharFreq tmp = {.character = 0, .frequency = 0};

    for (phase = 0; phase < res->number_of_chars; phase++) {
        if (phase % 2 == 0) {
            for (i = 1; i < res->number_of_chars; i += 2) {
                if (res->charFreqs[i-1].frequency > res->charFreqs[i].frequency) {
                    tmp = res->charFreqs[i-1]; 
                    res->charFreqs[i-1] = res->charFreqs[i];
                    res->charFreqs[i] = tmp;
                }
            }
        } else {
             for (i = 1; i < res->number_of_chars - 1; i += 2) {
                if (res->charFreqs[i].frequency > res->charFreqs[i+1].frequency) {
                    tmp = res->charFreqs[i+1]; 
                    res->charFreqs[i+1] = res->charFreqs[i];
                    res->charFreqs[i] = tmp;
                }
            }
        }
    }
}

void appendToCharFreqs(CharFreqDictionary *dict, CharFreq *charFreq, int pos) {
	if (pos == FIRST) {
		CharFreq *tmp = malloc(dict->number_of_chars * sizeof(CharFreq));
		memcpy(tmp, dict->charFreqs, dict->number_of_chars * sizeof(CharFreq));

		free(dict->charFreqs);
		int oldSize = dict->number_of_chars;
		++dict->number_of_chars;
		dict->charFreqs = malloc(dict->number_of_chars * sizeof(CharFreq));

		memcpy(dict->charFreqs+1, tmp, oldSize * sizeof(CharFreq));

		dict->charFreqs[0].character = charFreq->character;
		dict->charFreqs[0].frequency = 1;
	} else if (pos == LAST) {
		dict->charFreqs[dict->number_of_chars] = (struct CharFreq) {.character = charFreq->character, .frequency = charFreq->frequency};
		++dict->number_of_chars;
	} else if (pos == LAST_R) {
   		dict->charFreqs = realloc(dict->charFreqs, sizeof(CharFreq) * (dict->number_of_chars+1));
		dict->charFreqs[dict->number_of_chars] = (struct CharFreq) {.character = charFreq->character, .frequency = charFreq->frequency};
		++dict->number_of_chars;
    }
}

void mergeCharFreqs(CharFreqDictionary *dst, CharFreqDictionary *src, int pos) {
	for (int i = 0; i < src->number_of_chars; i++) {
		char character = src->charFreqs[i].character;
		int frequency = src->charFreqs[i].frequency;
		bool assigned = false;
		for (int j = 0; j < dst->number_of_chars && !assigned; j++) {
			if (dst->charFreqs[j].character == character) {
				dst->charFreqs[j].frequency += frequency;
				assigned = true;
			}
		}

		if (!assigned)
			appendToCharFreqs(dst, &src->charFreqs[i], pos);
	}
}

void printCharFreqs(CharFreqDictionary *dict) {
	printf("Dictionary: \n");
	for (int i = 0; i < dict->number_of_chars; i++) {
			printf("\t%d.\tcharacter: ", i); 
			printFormattedChar(dict->charFreqs[i].character);
			printf("\tfrequence: %d\n", dict->charFreqs[i].frequency); 
	}
}
