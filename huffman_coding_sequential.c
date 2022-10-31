
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char text[] = "Life is a dream " 
"A gift we receive"
"To live and to love"
"We forge the path"

"Our nightmare in birth"
"Our struggle for worth"
"In vain we carry on"
"Our mission to become"

"Adapt to this world"
"This chance we must take"
"We\'ll sing our song"
"We\'ll play our hand"

"Why allow (broken world)"
"This crooked fate? (Retaliate)"
"The tunnel\'s light (accept defeat)"
"Is unmoving (don\'t be afraid)"
"It burns your eyes (obscures the path)"
"But you persevere"
"You must adapt because it\'s there"

"Climb, find your way"
"Scale high, don\'t look back"
"With hope you will find"
"The life you seek"

"Judgement from birth"
"Criticize our worth"
"We struggled to uphold"
"Struck down, will we fold?"

"Climb, find your way"
"Scale high, don\'t look back"
"With hope you will find"
"The life you seek"

"I wish I could be some one"
"Be the moon and be the sun"
"Ambition burned my wings ablaze"
"Shed a tear in these dark days"
"Tearing down the walls of love"
"Soaring through the fire above"
"Pride exists to be reclaimed"
"Roll the dice don\'t lose the game"
"Carry the weight of the world"
"The summit says you\'ll prove my worth"
"Let me claim what\'s rightfully"
"Yours to take and mine to keep"
"Stars begin to swarm around"
"The path of fate this tired ground"
"Challenge met but all the while"
"A mountain build on greed and guile"; 

struct LetterFreq {
	int frequency; 
	char letter; 
}; 

struct LetterFreqDict {
	int number_of_letters; 
	struct LetterFreq* letterFreqs; 
}; 

int MAX_UNIQUE_LETTERS = 100; 

void get_freqs_from(char data[], int count, struct LetterFreqDict* res) {
	int letterFreqSize = 0; 
	
	for (int i = 0; i < count; i++){
		char letter = data[i]; 

		int assigned = 0; 
		for (int j = 0; j < letterFreqSize; j++) {
			if (res->letterFreqs[j].letter == letter){
				res->letterFreqs[j].frequency += 1; 
				assigned = 1; 
			}
		}

		if (!assigned) {
			res->letterFreqs[letterFreqSize].letter = letter; 
			res->letterFreqs[letterFreqSize].frequency = 1; 
			letterFreqSize += 1; 
		}
	}

	res->number_of_letters = letterFreqSize; 
}


int main() {
	int count = sizeof(text) / sizeof(text[0]);
	
	struct LetterFreqDict res;
	res.number_of_letters = 0; 
	res.letterFreqs = malloc(sizeof(struct LetterFreq) * MAX_UNIQUE_LETTERS); 

	get_freqs_from(text, count, &res); 

	for (int i = 0; i < res.number_of_letters; i++) {
		printf("%c: %d\n", res.letterFreqs[i].letter, res.letterFreqs[i].frequency); 
	}
	
	return 0;
}
