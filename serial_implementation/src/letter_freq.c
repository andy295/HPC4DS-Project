
struct LetterFreq {
	int frequency; 
	char letter; 
}; 

struct LetterFreqDictionary {
	int number_of_letters; 
	struct LetterFreq* letterFreqs; 
}; 

void append_to_freq_dict(struct LetterFreqDictionary* dict, char letter){
	int assigned = 0; 
	for (int i = 0; i < dict->number_of_letters; i++){
		if (dict->letterFreqs[i].letter == letter){
			dict->letterFreqs[i].frequency++; 
			assigned = 1; 
			break; 
		}
	}
	if (assigned == 0){
		dict->letterFreqs[dict->number_of_letters].letter = letter; 
		dict->letterFreqs[dict->number_of_letters].frequency = 1; 
		dict->number_of_letters++; 
	}
}

void append_to_freq(struct LetterFreqDictionary* dict, char letter, int freq){
	res->letterFreqs[res->number_of_letters].letter = letter; 
	res->letterFreqs[res->number_of_letters].frequency = freq; 
	res->number_of_letters += 1; 
}

void get_freqs_from(char data[], int count, struct LetterFreqDictionary* res) {	
	for (int i = 0; i < count; i++){
		append_to_freq_dict(res, data[i]);
	}

	// use # as a special character to indicate the end of the file (commento da copilot, complimenti)
	append_to_freq(res, '#', 1000);
}

void sort_freqs(struct LetterFreqDictionary* res){
	for (int i = 0; i < res->number_of_letters; i++) {
		
		struct LetterFreq minLetterFreq = res->letterFreqs[i];
		int indexOfMin = i;  
		for (int j = i; j < res->number_of_letters; j++){
			if (minLetterFreq.frequency > res->letterFreqs[j].frequency){
				minLetterFreq = res->letterFreqs[j]; 
				indexOfMin = j; 
			}
		}

		struct LetterFreq temp = res->letterFreqs[i]; 
		res->letterFreqs[i] = minLetterFreq;
		res->letterFreqs[indexOfMin] = temp;
	}
}

void merge_letter_freqs(struct LetterFreqDictionary* allLetters, struct LetterFreqDictionary* letterFreqs){

	for (int i = 0; i < letterFreqs->number_of_letters; i++){
		int found = 0; 
		for (int j = 0; j < allLetters->number_of_letters; j++){
			if (allLetters->letterFreqs[j].letter == letterFreqs->letterFreqs[i].letter){
				allLetters->letterFreqs[j].frequency += letterFreqs->letterFreqs[i].frequency; 
				found = 1; 
				break; 
			}
		}

		if (found == 0){
			allLetters->letterFreqs[allLetters->number_of_letters].letter = letterFreqs->letterFreqs[i].letter; 
			allLetters->letterFreqs[allLetters->number_of_letters].frequency = letterFreqs->letterFreqs[i].frequency; 
			allLetters->number_of_letters++; 
		}
	}
}

struct LetterFreqDictionary* init_letter_freq_dictionary(int size){
	struct LetterFreqDictionary* res = malloc(sizeof(struct LetterFreqDictionary)); 
	res->letterFreqs = malloc(sizeof(struct LetterFreq) * size); 
	res->number_of_letters = 0; 
	return res; 
}

struct LetterFreqDictionary* get_letter_freqs(char* text, int count){
	struct LetterFreqDictionary* res = init_letter_freq_dictionary(MAX_UNIQUE_LETTERS);
	
	get_freqs_from(text, count, res); 

	return res; 
}