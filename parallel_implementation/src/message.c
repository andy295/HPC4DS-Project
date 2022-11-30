#include "include/message.h"

// --- generic funcs ---

void initMsgHeader(MsgHeader* header, int id, int size) {
	header->id = id;
	header->size = size;
}

void printMessageHeader(MsgCharFreqDictionary* msg) {
	#if VERBOSE <= 3
		printf("Message: msg->header.size: %d\n", msg->header.size);
		printf("Message: msg->header.id: %d\n", msg->header.id);
		printf("Message: msg->charsNr: %d\n", msg->charsNr);
		printf("Message: msg->charFreqs: %p\n", msg->charFreqs);
	#endif
}

// --- MsgCharFreqDictionary funcs ---
MsgCharFreqDictionary* createMsgCharFreqDictionaryFromFreqs(CharFreqDictionary* allChars){
	MsgCharFreqDictionary* msgDictSnd = malloc(sizeof(MsgCharFreqDictionary));
    
	initMsgHeader(&msgDictSnd->header, MSG_DICTIONARY, 0);
	msgDictSnd->charsNr = 0;
	msgDictSnd->charFreqs = NULL;

	buildMsgCharFreqDictionary(allChars, msgDictSnd);

	return msgDictSnd;
}

void buildMsgCharFreqDictionary(CharFreqDictionary* dict, MsgCharFreqDictionary* msgDict) {
	#if VERBOSE <= 2
		printMessageHeader(&msgDict->header);
	#endif

	msgDict->header.size = sizeof(MsgCharFreqDictionary) + dict->number_of_chars * sizeof(CharFreq);
	msgDict->charsNr = dict->number_of_chars;
	msgDict->charFreqs = malloc(dict->number_of_chars * sizeof(CharFreq));

	// maybe we could use memcpy here
	for (int i = 0; i < dict->number_of_chars; i++) {
		msgDict->charFreqs[i].character = dict->charFreqs[i].character;
		msgDict->charFreqs[i].frequency = dict->charFreqs[i].frequency;
	}
}

BYTE* createMessageBufferFromMsgCharFreqDictionary(MsgCharFreqDictionary* msgDictSnd, int bufferSize){
	BYTE *buffer = malloc(sizeof(BYTE) * bufferSize);
	memcpy(buffer, &msgDictSnd->header, sizeof(MsgHeader));
	memcpy(buffer + sizeof(MsgHeader), &msgDictSnd->charsNr, sizeof(int));
	memcpy(buffer + sizeof(MsgHeader) + sizeof(int), msgDictSnd->charFreqs, sizeof(CharFreq) * msgDictSnd->charsNr);

	return buffer;
}

MsgCharFreqDictionary* createMsgCharFreqDictionaryFromByteBuffer(BYTE *buffer){
	MsgCharFreqDictionary* msgRcv = malloc(sizeof(MsgCharFreqDictionary));

    initMsgHeader(&msgRcv->header, MSG_DICTIONARY, 0);
	msgRcv->charsNr = 0;
	msgRcv->charFreqs = NULL;

	// copy the data from the buffer to the message
	memcpy(&msgRcv->header, buffer, sizeof(MsgHeader));
	memcpy(&msgRcv->charsNr, buffer + sizeof(MsgHeader), sizeof(int));

	msgRcv->charFreqs = malloc(sizeof(CharFreq) * msgRcv->charsNr);
	memcpy(msgRcv->charFreqs, buffer + sizeof(MsgHeader) + sizeof(int), sizeof(CharFreq) * msgRcv->charsNr);

	return msgRcv;
}

// --- MsgEncodingDictionary funcs ---
MsgEncodingDictionary* createMsgEncodingDictionaryFromFreqs(CharEncoding* allEncodings, int size){
	MsgEncodingDictionary* msgDictSnd = malloc(sizeof(MsgEncodingDictionary));
    
	initMsgHeader(&msgDictSnd->header, MSG_DICTIONARY, 0);
	
	msgDictSnd->header.size = sizeof(MsgCharFreqDictionary) + size * sizeof(CharEncoding);
	msgDictSnd->encodingNr = size;
	msgDictSnd->charEncodings = malloc(size * sizeof(CharEncoding));

	// maybe we could use memcpy here
	for (int i = 0; i < size; i++) {
		msgDictSnd->charEncodings[i].character = allEncodings[i].character;
		msgDictSnd->charEncodings[i].encoding = allEncodings[i].encoding;
	}

	return msgDictSnd;
}

BYTE* createMessageBufferFromMsgEncodingDictionary(MsgEncodingDictionary* msgDictSnd, int bufferSize){
	BYTE *buffer = malloc(sizeof(BYTE) * bufferSize);
	memcpy(buffer, &msgDictSnd->header, sizeof(MsgHeader));
	memcpy(buffer + sizeof(MsgHeader), &msgDictSnd->encodingNr, sizeof(int));
	memcpy(buffer + sizeof(MsgHeader) + sizeof(int), msgDictSnd->charEncodings, sizeof(CharEncoding) * msgDictSnd->encodingNr);

	return buffer;
}

MsgEncodingDictionary* createMsgEncodingDictionaryFromByteBuffer(BYTE *buffer){
	MsgEncodingDictionary* msgRcv = malloc(sizeof(MsgEncodingDictionary));

    initMsgHeader(&msgRcv->header, MSG_DICTIONARY, 0);
	msgRcv->encodingNr = 0;
	msgRcv->charEncodings = NULL;

	// copy the data from the buffer to the message
	memcpy(&msgRcv->header, buffer, sizeof(MsgHeader));
	memcpy(&msgRcv->encodingNr, buffer + sizeof(MsgHeader), sizeof(int));

	msgRcv->charEncodings = malloc(sizeof(CharEncoding) * msgRcv->encodingNr);
	memcpy(msgRcv->charEncodings, buffer + sizeof(MsgHeader) + sizeof(int), sizeof(CharEncoding) * msgRcv->encodingNr);

	return msgRcv;
}

