#include "include/message.h"

void initMsgHeader(MsgHeader* header, int id, int size) {
	header->id = id;
	header->size = size;
}

MsgDictionary* createMsgDictionaryFromFreqs(CharFreqDictionary* allChars){
	MsgDictionary* msgDictSnd = malloc(sizeof(MsgDictionary));
    
	initMsgHeader(&msgDictSnd->header, MSG_DICTIONARY, 0);
	msgDictSnd->charsNr = 0;
	msgDictSnd->charFreqs = NULL;

	buildMsgDictionary(allChars, msgDictSnd);

	return msgDictSnd;
}

void buildMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDict) {
	#if VERBOSE <= 2
		printMessageHeader(&msgDict->header);
	#endif

	msgDict->header.size = sizeof(MsgDictionary) + dict->number_of_chars * sizeof(CharFreq);
	msgDict->charsNr = dict->number_of_chars;
	msgDict->charFreqs = malloc(dict->number_of_chars * sizeof(CharFreq));

	// maybe we could use memcpy here
	for (int i = 0; i < dict->number_of_chars; i++) {
		msgDict->charFreqs[i].character = dict->charFreqs[i].character;
		msgDict->charFreqs[i].frequency = dict->charFreqs[i].frequency;
	}
}

void printMessageHeader(MsgDictionary* msg) {
	#if VERBOSE <= 3
		printf("Message: msg->header.size: %d\n", msg->header.size);
		printf("Message: msg->header.id: %d\n", msg->header.id);
		printf("Message: msg->charsNr: %d\n", msg->charsNr);
		printf("Message: msg->charFreqs: %p\n", msg->charFreqs);
	#endif
}

BYTE* createMessageBufferFromDict(MsgDictionary* msgDictSnd, int bufferSize){
	BYTE *buffer = malloc(sizeof(BYTE) * bufferSize);
	memcpy(buffer, &msgDictSnd->header, sizeof(MsgHeader));
	memcpy(buffer + sizeof(MsgHeader), &msgDictSnd->charsNr, sizeof(int));
	memcpy(buffer + sizeof(MsgHeader) + sizeof(int), msgDictSnd->charFreqs, sizeof(CharFreq) * msgDictSnd->charsNr);

	return buffer;
}

void createMsgDictFromByteBuffer(MsgDictionary* msgRcv, BYTE *buffer){
    initMsgHeader(&msgRcv->header, MSG_DICTIONARY, 0);
	msgRcv->charsNr = 0;
	msgRcv->charFreqs = NULL;

	// copy the data from the buffer to the message
	memcpy(&msgRcv->header, buffer, sizeof(MsgHeader));
	memcpy(&msgRcv->charsNr, buffer + sizeof(MsgHeader), sizeof(int));

	msgRcv->charFreqs = malloc(sizeof(CharFreq) * msgRcv->charsNr);
	memcpy(msgRcv->charFreqs, buffer + sizeof(MsgHeader) + sizeof(int), sizeof(CharFreq) * msgRcv->charsNr);
}
