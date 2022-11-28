#include "include/message.h"

void initMsgHeader(MsgHeader* header, int id, int size)
{
	header->id = id;
	header->size = size;
}

void initMsgDictionary(MsgDictionary* msg) {
    initMsgHeader(&msg->header, MSG_DICTIONARY, 0);

	msg->charsNr = 0;
	msg->charFreqs = NULL;
}

void setMsg(void* str, MsgGeneric* msg) {
	switch (msg->header.id)
	{
	case MSG_DICTIONARY:
		buildMsgDictionary((CharFreqDictionary*)str, (MsgDictionary*)msg);
		break;
	
	default:
		printf("Error: unknown message type: %d\n", msg->header.id);
		break;
	}
}

void buildMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDict) {
	int i;

	#if VERBOSE <= 2
		printf("size of int: %lu\n", sizeof(int));
		printf("size of char: %lu\n", sizeof(char));
		printf("size of msgHeader: %lu\n", sizeof(MsgHeader));
		printf("size of msgDict: %lu\n", sizeof(MsgDictionary));
		printf("size of CharFreq: %lu\n", sizeof(CharFreq));
		printf("size of CharFreq * number of chars : %lu * %u = %lu\n", sizeof(CharFreq), dict->number_of_chars, sizeof(CharFreq) * dict->number_of_chars);
	#endif

	printf("message: sizeof(MsgDictionary): %d\n", sizeof(MsgDictionary));
	printf("message: size of CharFreq array: %d\n", dict->number_of_chars * sizeof(CharFreq));

	msgDict->header.size = sizeof(MsgDictionary) + dict->number_of_chars * sizeof(CharFreq);

	printf("message: msgDict->header.size: %d\n", msgDict->header.size);

	msgDict->charsNr = dict->number_of_chars;
	msgDict->charFreqs = malloc(dict->number_of_chars * sizeof(CharFreq));

	// maybe we could use memcpy here
	for (i = 0; i < dict->number_of_chars; i++) {
		msgDict->charFreqs[i].character = dict->charFreqs[i].character;
		msgDict->charFreqs[i].frequency = dict->charFreqs[i].frequency;
	}

	#if VERBOSE <= 1
		for (i = 0; i < msgDict->charsNr; i++)
			printf("charFreqs[%d]\tchar: %c\tfreq:%d\n", i, msgDict->charFreqs[i].character, msgDict->charFreqs[i].frequency);
	#endif	
}

void getMsg(void* str, MsgGeneric* msg) {
	switch (msg->header.id)
	{
	case MSG_DICTIONARY:
		printf("message: i'm here\n");
		getMsgDictionary((CharFreqDictionary*)str, (MsgDictionary*)msg);
		// mergeCharFreqs((CharFreqDictionary*)str, (MsgDictionary*)msg);
		break;
	default:
		printf("Error: unknown message type: %d\n", msg->header.id);
		break;
	}
}

void mergeCharFreqs(CharFreqDictionary* dict, MsgDictionary* msg) {
	// int i, j;
	// for (i = 0; i < msg->charsNr; i++) {
	// 	bool found = false; 
	// 	for (j = 0; j < dict->number_of_chars && !found; j++) {
	// 		if (dict->charFreqs[j].character == msg->characters[i]) {
	// 			dict->charFreqs[j].frequency += msg->frequencies[i]; 
	// 			found = true; 
	// 		}
	// 	}

	// 	if (!found) {
	// 		++dict->number_of_chars; 
    //         dict->charFreqs = realloc(dict->charFreqs, sizeof(CharFreq) * dict->number_of_chars);

    //         CharFreq *p = &dict->charFreqs[dict->number_of_chars - 1];
	// 		p->character = msg->characters[i];
	// 		p->frequency = msg->frequencies[i];
	// 	}
	// }
}

//////////////////////////////////////////////////////////////////////////

void getMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDict) {
	int i;

	printf("Message: msgDict->charsNr: %d\n", msgDict->charsNr);
	printf("Message: msgDict->charFreqs: %p\n", msgDict->charFreqs);


	printf("Message: dict->charFreqs: %p\n", dict->charFreqs);

	dict->number_of_chars = msgDict->charsNr;
	dict->charFreqs = malloc(dict->number_of_chars * sizeof(CharFreq));

	printf("Message: dict->number_of_chars: %d\n", dict->number_of_chars);
	printf("Message: dict->charFreqs: %p\n", dict->charFreqs);

	// // maybe we could use memcpy here
	// for (i = 0; i < msgDict->charsNr; i++) {
	// 	dict->charFreqs[i].character = msgDict->charFreqs[i].character;
	// 	dict->charFreqs[i].frequency = msgDict->charFreqs[i].frequency;
	// }

	// for (i = 0; i < msgDict->charsNr; i++) {
	// 	printf("msgDict->charFreqs[%d].character: %c\n", i, msgDict->charFreqs[i].character);
	// 	printf("msgDict->charFreqs[%d].frequency: %d\n", i, msgDict->charFreqs[i].frequency);
	// }

	// CharFreq *tmp = msgDict->charFreqs;
	// printf("charFreqs[0]\tchar: %c\tfreq:%d\n", tmp->character, tmp->frequency);
}
