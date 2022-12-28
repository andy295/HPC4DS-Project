#include "include/message.h"

BYTE* getMessage(void* data, int msgType, int *bufferSize) {
	switch (msgType)
	{
	case MSG_DICTIONARY:
		return serializeMsgCharFreqDictionary((CharFreqDictionary*)data, bufferSize);
		break;

	case MSG_ENCODING_DICTIONARY:
		return serializeMsgCharEncodingDictionary((CharEncodingDictionary*)data, bufferSize);
		break;

	case MSG_ENCODING_TEXT:
		return serializeMsgEncodingText((EncodingText*)data, bufferSize);
		break;

	case MSG_TEXT:
		return serializeMsgText((char*)data, bufferSize);
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

	MsgCharFreqDictionary msg = {
		.header.id = MSG_DICTIONARY,
		.header.size = *bufferSize,
		.charsNr = dict->number_of_chars,
		.charFreqs = NULL};
	memcpy(buffer, &msg, sizeof(MsgCharFreqDictionary));
	memcpy(buffer + sizeof(MsgCharFreqDictionary), dict->charFreqs, sizeof(CharFreq) * dict->number_of_chars);

	return buffer;
}

BYTE* serializeMsgCharEncodingDictionary(CharEncodingDictionary* dict, int *bufferSize) {
	*bufferSize = sizeof(MsgCharEncodingDictionary);
	BYTE *buffer = malloc(sizeof(BYTE) * (*bufferSize));

	MsgCharEncodingDictionary msg = {
		.header.id = MSG_ENCODING_DICTIONARY,
		.header.size = *bufferSize,
		.charsNr = dict->number_of_chars,
		.charEncoding = NULL};
	memcpy(buffer, &msg, sizeof(MsgCharEncodingDictionary));

	int totalStrLen = 0;
	for (int i = 0; i < dict->number_of_chars; i++) {
		*bufferSize += sizeof(CharEncoding) + (sizeof(char) * (dict->charEncoding[i].length+1));
		buffer = realloc(buffer, sizeof(BYTE) * (*bufferSize));

		int start = sizeof(MsgCharEncodingDictionary) + (sizeof(CharEncoding) * i) + (sizeof(char) * (totalStrLen));
		memcpy(buffer + start, &dict->charEncoding[i], sizeof(CharEncoding));

		start += sizeof(CharEncoding);
		memcpy(buffer + start, dict->charEncoding[i].encoding, sizeof(char) * dict->charEncoding[i].length+1);

		totalStrLen += dict->charEncoding[i].length+1;
	}

	return buffer;
}

BYTE *serializeMsgEncodingText(EncodingText *encodingText, int *bufferSize) {
	int posSize = sizeof(short) * encodingText->nr_of_dim;
	int encTextSize = sizeof(BYTE) * encodingText->nr_of_bytes;
	*bufferSize = sizeof(MsgEncodingText) + posSize + encTextSize;

	BYTE *buffer = malloc(sizeof(BYTE) * (*bufferSize));

	MsgEncodingText msg = {
		.header.id = MSG_ENCODING_TEXT,
		.header.size = *bufferSize,
		.nrOfPos = encodingText->nr_of_dim,
		.nrOfBytes = encodingText->nr_of_bytes, 
		.positions = NULL, .text = NULL};
	memcpy(buffer, &msg, sizeof(MsgEncodingText));
	memcpy(buffer + sizeof(MsgEncodingText), encodingText->dimensions, posSize);
	memcpy(buffer + sizeof(MsgEncodingText) + posSize, encodingText->encodedText, encTextSize);

	return buffer;
}

BYTE *serializeMsgText(char *text, int *bufferSize) {
	int textLen = (strlen(text) + 1);
	*bufferSize = sizeof(MsgText) + (sizeof(char) * textLen);
	BYTE *buffer = malloc(sizeof(BYTE) * (*bufferSize));

	MsgText msg = {
		.header.id = MSG_TEXT,
		.header.size = *bufferSize,
		.textLength = textLen,
		.text = NULL};
	memcpy(buffer, &msg, sizeof(MsgText));
	memcpy(buffer + sizeof(MsgText), text, sizeof(char) * textLen);

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

	case MSG_ENCODING_DICTIONARY:
		deserializeMsgCharEncodingDictionary((CharEncodingDictionary*)data, buffer);
		break;

	case MSG_ENCODING_TEXT:
		deserializeMsgEncodingText((EncodingText*)data, buffer);
		break;

	case MSG_TEXT:
		deserializeMsgText((DecodingText*)data, buffer);
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
	
	#if DEBUG
		printf("\nReceived dictionary with %d chars:\n", dict->number_of_chars);
		printCharFreqs(dict);
	#endif
}

void deserializeMsgCharEncodingDictionary(CharEncodingDictionary* dict, BYTE *buffer) {
	MsgCharEncodingDictionary msg;
	memcpy(&msg, buffer, sizeof(MsgCharEncodingDictionary));

	dict->number_of_chars = msg.charsNr;
	dict->charEncoding = malloc(sizeof(CharEncoding) * dict->number_of_chars);

	int prevStrLen = 0;
	for (int i = 0; i < dict->number_of_chars; i++) {
		int start = sizeof(MsgCharEncodingDictionary) + (sizeof(CharEncoding) * i) + (sizeof(char) * (prevStrLen));
		memcpy(&dict->charEncoding[i], buffer + start, sizeof(CharEncoding));

		dict->charEncoding[i].encoding = malloc(sizeof(char) * (dict->charEncoding->length+1));

		start += sizeof(CharEncoding);
		memcpy(dict->charEncoding[i].encoding, buffer + start, sizeof(char) * (dict->charEncoding[i].length+1));

		prevStrLen += dict->charEncoding[i].length+1;
	}

	#if DEBUG
		printf("\nReceived encoding dictionary with %d chars:\n", dict->number_of_chars);
		printEncodings(dict);
	#endif
}

void deserializeMsgEncodingText(EncodingText *encodingText, BYTE *buffer) {
	MsgEncodingText msg;
	memcpy(&msg, buffer, sizeof(MsgEncodingText));

	int posSize = sizeof(short) * msg.nrOfPos;
	int encTextSize = sizeof(BYTE) * msg.nrOfBytes;

	encodingText->nr_of_dim = msg.nrOfPos;
	encodingText->dimensions = malloc(sizeof(short) * encodingText->nr_of_dim);
	memcpy(encodingText->dimensions, buffer + sizeof(MsgEncodingText), posSize);

	encodingText->nr_of_bytes = msg.nrOfBytes;
	encodingText->encodedText = malloc(sizeof(BYTE) * encodingText->nr_of_bytes);
	memcpy(encodingText->encodedText, buffer + sizeof(MsgEncodingText) + posSize, encTextSize);
}

void deserializeMsgText(DecodingText *decodedText, BYTE *buffer) {
	MsgText msg;
	memcpy(&msg, buffer, sizeof(MsgText));

	int textLen = sizeof(char) * msg.textLength;

	decodedText->length = msg.textLength;
	decodedText->decodedText = malloc(textLen);
	memcpy(decodedText->decodedText, buffer + sizeof(MsgText), textLen);
}

char* getMsgName(int msgType) {
	switch (msgType)
	{
	case MSG_DICTIONARY:
		return "DICTIONARY";
	case MSG_ENCODING_DICTIONARY:
		return "ENCODING DICTIONARY";
	case MSG_ENCODING_TEXT:
		return "ENCODING TEXT";
	case MSG_TEXT:
		return "TEXT";
	default:
		return "UNKNOWN";
	}
}