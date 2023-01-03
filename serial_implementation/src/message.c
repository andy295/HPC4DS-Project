#include "include/message.h"

BYTE* getMessage(MsgHeader *header, void *data) {
	switch (header->id)
	{
	case MSG_DICTIONARY:
		return serializeMsgCharFreqDictionary(header, (CharFreqDictionary*)data);
		break;

	case MSG_ENCODING_DICTIONARY:
		return serializeMsgCharEncodingDictionary(header, (CharEncodingDictionary*)data);
		break;

	case MSG_ENCODING_TEXT:
		return serializeMsgEncodingText(header, (EncodingText*)data);
		break;

	case MSG_TEXT:
		return serializeMsgText(header, (char*)data);
		break;
	
	default:
		fprintf(stderr, "Error: in get message unknown message type: %d\n", header->id);
		return NULL;
		break;
	}
}

BYTE* serializeMsgCharFreqDictionary(MsgHeader *header, CharFreqDictionary *dict) {
	header->size = sizeof(MsgCharFreqDictionary) + (sizeof(CharFreq) * dict->number_of_chars);
	BYTE *buffer = malloc(sizeof(BYTE) * (header->size));

	MsgCharFreqDictionary msg = {
		.header.id = header->id,
		.header.size = header->size,
		.charsNr = dict->number_of_chars,
		.charFreqs = NULL};
	memcpy(buffer, &msg, sizeof(MsgCharFreqDictionary));
	memcpy(buffer + sizeof(MsgCharFreqDictionary), dict->charFreqs, sizeof(CharFreq) * dict->number_of_chars);

	return buffer;
}

BYTE* serializeMsgCharEncodingDictionary(MsgHeader *header, CharEncodingDictionary *dict) {
	header->size = sizeof(MsgCharEncodingDictionary);
	BYTE *buffer = malloc(sizeof(BYTE) * (header->size));

	MsgCharEncodingDictionary msg = {
		.header.id = header->id,
		.header.size = header->size,
		.charsNr = dict->number_of_chars,
		.charEncoding = NULL};
	memcpy(buffer, &msg, sizeof(MsgCharEncodingDictionary));

	int totalStrLen = 0;
	for (int i = 0; i < dict->number_of_chars; i++) {
		header->size += sizeof(CharEncoding) + (sizeof(char) * (dict->charEncoding[i].length+1));
		buffer = realloc(buffer, sizeof(BYTE) * (header->size));

		int start = sizeof(MsgCharEncodingDictionary) + (sizeof(CharEncoding) * i) + (sizeof(char) * (totalStrLen));
		memcpy(buffer + start, &dict->charEncoding[i], sizeof(CharEncoding));

		start += sizeof(CharEncoding);
		memcpy(buffer + start, dict->charEncoding[i].encoding, sizeof(char) * (dict->charEncoding[i].length+1));

		totalStrLen += (dict->charEncoding[i].length + 1);
	}

	return buffer;
}

BYTE *serializeMsgEncodingText(MsgHeader *header, EncodingText *encodingText) {
	int posSize = sizeof(short) * encodingText->nr_of_dim;
	int encTextSize = sizeof(BYTE) * encodingText->nr_of_bytes;
	header->size = sizeof(MsgEncodingText) + posSize + encTextSize;

	BYTE *buffer = malloc(sizeof(BYTE) * (header->size));

	MsgEncodingText msg = {
		.header.id = header->id,
		.header.size = header->size,
		.nrOfPos = encodingText->nr_of_dim,
		.nrOfBytes = encodingText->nr_of_bytes, 
		.positions = NULL, .text = NULL};
	memcpy(buffer, &msg, sizeof(MsgEncodingText));
	memcpy(buffer + sizeof(MsgEncodingText), encodingText->dimensions, posSize);
	memcpy(buffer + sizeof(MsgEncodingText) + posSize, encodingText->encodedText, encTextSize);

	return buffer;
}

BYTE *serializeMsgText(MsgHeader *header, char *text) {
	int textLen = strlen(text) + 1;
	header->size = sizeof(MsgText) + (sizeof(char) * textLen);
	BYTE *buffer = malloc(sizeof(BYTE) * (header->size));

	MsgText msg = {
		.header.id = header->id,
		.header.size = header->size,
		.textLength = textLen,
		.text = NULL};
	memcpy(buffer, &msg, sizeof(MsgText));
	memcpy(buffer + sizeof(MsgText), text, sizeof(char) * textLen);

	return buffer;
}

void setMessage(MsgHeader *header, void *data, BYTE *buffer) {
	MsgHeader *p = (MsgHeader*)buffer;
	switch (p->id)
	{
	case MSG_DICTIONARY:
		deserializeMsgCharFreqDictionary(header, (CharFreqDictionary*)data, buffer);
		break;

	case MSG_ENCODING_DICTIONARY:
		deserializeMsgCharEncodingDictionary(header, (CharEncodingDictionary*)data, buffer);
		break;

	case MSG_ENCODING_TEXT:
		deserializeMsgEncodingText(header, (EncodingText*)data, buffer);
		break;

	case MSG_TEXT:
		deserializeMsgText(header, (DecodingText*)data, buffer);
		break;

	default:
		fprintf(stderr, "Error: in set message unknown message id: %d\n", p->id);
		break;
	}
}

void deserializeMsgCharFreqDictionary(MsgHeader *header, CharFreqDictionary *dict, BYTE *buffer) {
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

void deserializeMsgCharEncodingDictionary(MsgHeader *header, CharEncodingDictionary *dict, BYTE *buffer) {
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

		prevStrLen += (dict->charEncoding[i].length + 1);
	}

	#if DEBUG
		printf("\nReceived encoding dictionary with %d chars:\n", dict->number_of_chars);
		printEncodings(dict);
	#endif
}

void deserializeMsgEncodingText(MsgHeader *header, EncodingText *encodingText, BYTE *buffer) {
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

void deserializeMsgText(MsgHeader *header, DecodingText *decodedText, BYTE *buffer) {
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
