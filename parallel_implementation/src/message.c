#include "include/message.h"

void initMsgDictionary(MsgHeader *header) {
    header->id = MSG_DICTIONARY;
    header->size = FIVE;
}

void setMsg(void* str, MsgGeneric* msg, MPI_Datatype *msgType) {
	switch (msg->header.id)
	{
	case MSG_DICTIONARY:
		buildMsgDictionary((CharFreqDictionary*)str, (MsgDictionary*)msg, msgType);
		break;
	
	default:
		printf("Error: unknown message type: %d\n", msg->header.id);
		break;
	}
}

void buildMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDictionary, MPI_Datatype* msgType) {
	int i;
	MPI_Aint idAddr, sizeAddr, charsNrAddr, charsAddr, freqsAddr;

	// fill in the message
	msgDictionary->charsNr = dict->number_of_chars;

	msgDictionary->characters = malloc(sizeof(char) * dict->number_of_chars);
	msgDictionary->frequencies = malloc(sizeof(int) * dict->number_of_chars);

	// maybe we can use multithreading here 
	for (i = 0; i < dict->number_of_chars; i++) {
		msgDictionary->characters[i] = dict->charFreqs[i].character;
		msgDictionary->frequencies[i] = dict->charFreqs[i].frequency;
	}

	// create the MPI type
	int block_length[FIVE] = {1, 1, 1, dict->number_of_chars, dict->number_of_chars};
	
	MPI_Aint displacement[FIVE];
	
	MPI_Get_address(&msgDictionary->header.id, &idAddr);
	MPI_Get_address(&msgDictionary->header.size, &sizeAddr);
	MPI_Get_address(&msgDictionary->charsNr, &charsNrAddr);
	MPI_Get_address(msgDictionary->characters, &charsAddr);
	MPI_Get_address(msgDictionary->frequencies, &freqsAddr);

	displacement[ONE] = sizeAddr - idAddr;
	displacement[TWO] = charsNrAddr - idAddr;
	displacement[THREE] = charsAddr - idAddr;
	displacement[FOUR] = freqsAddr - idAddr;
	
	MPI_Datatype types[FIVE] = {MPI_INT, MPI_INT, MPI_INT, MPI_CHAR, MPI_INT};

	MPI_Type_create_struct(FIVE, block_length, displacement, types, msgType);
	MPI_Type_commit(msgType);
}

void getMsg(void* str, MsgGeneric* msg) {
	switch (msg->header.id)
	{
	case MSG_DICTIONARY:
		mergeCharFreqs((CharFreqDictionary*)str, (MsgDictionary*)msg);
		break;
	
	default:
		printf("Error: unknown message type: %d\n", msg->header.id);
		break;
	}
}

void mergeCharFreqs(CharFreqDictionary* dict, MsgDictionary* msg) {
	int i, j;
	for (i = 0; i < msg->charsNr; i++) {
		bool found = false; 
		for (j = 0; j < dict->number_of_chars && !found; j++) {
			if (dict->charFreqs[j].character == msg->characters[i]) {
				dict->charFreqs[j].frequency += msg->frequencies[i]; 
				found = true; 
			}
		}

		if (!found) {
			++dict->number_of_chars; 
            dict->charFreqs = realloc(dict->charFreqs, sizeof(CharFreq) * dict->number_of_chars);

            CharFreq *p = &dict->charFreqs[dict->number_of_chars - 1];
			p->character = msg->characters[i];
			p->frequency = msg->frequencies[i];
		}
	}
}
