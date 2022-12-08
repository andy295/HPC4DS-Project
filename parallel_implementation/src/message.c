#include "include/message.h"

BYTE* getMessage(void* data, int msgType, int *bufferSize) {
	switch (msgType)
	{
	case MSG_DICTIONARY:
		return serializeMsgCharFreqDictionary((CharFreqDictionary*)data, bufferSize);
		break;
	
	default:
		printf("Error: unknown message type: %d\n", msgType);
		return NULL;
		break;
	}
}

BYTE* serializeMsgCharFreqDictionary(CharFreqDictionary* dict, int *bufferSize) {
	*bufferSize = sizeof(MsgCharFreqDictionary) + (sizeof(CharFreq) * dict->number_of_chars);
	BYTE *buffer = malloc(sizeof(BYTE) * (*bufferSize));

	MsgCharFreqDictionary msg = {.header.id = MSG_DICTIONARY, .header.size = *bufferSize, .charsNr = dict->number_of_chars, .charFreqs = NULL};
	memcpy(buffer, &msg, sizeof(MsgCharFreqDictionary));
	memcpy(buffer + sizeof(MsgCharFreqDictionary), dict->charFreqs, sizeof(CharFreq) * dict->number_of_chars);

	return buffer;
}

void setMessage(void *data, BYTE *buffer) {
	MsgGeneric *p = (MsgGeneric*)buffer;
	int msgId = p->header.id;
	switch (msgId)
	{
	case MSG_DICTIONARY:
		deserializeMsgCharFreqDictionary((CharFreqDictionary*)data, buffer);
		break;

	default:
		printf("Error: unknown message type: %d\n", msgId);
		break;
	}
}

void deserializeMsgCharFreqDictionary(CharFreqDictionary* dict, BYTE *buffer) {
	MsgCharFreqDictionary msg;
	memcpy(&msg, buffer, sizeof(MsgCharFreqDictionary));

	dict->number_of_chars = msg.charsNr;
	dict->charFreqs = malloc(sizeof(CharFreq) * dict->number_of_chars);
	memcpy(dict->charFreqs, buffer + sizeof(MsgCharFreqDictionary), sizeof(CharFreq) * dict->number_of_chars);
	
	#if VERBOSE <= 2
		printf("\nReceived dictionary with %d chars:\n", dict->number_of_chars);
		printCharFreqDictionary(dict);
	#endif
}

void printCharFreqDictionary(CharFreqDictionary* dict) {
	printf("number of chars: %d\n", dict->number_of_chars);

	for (int i = 0; i < dict->number_of_chars; i++) {
		printf("char: ");
		printFormattedChar(dict->charFreqs[i].character);
		printf(", freq: %d\n", dict->charFreqs[i].frequency);
	}
}
