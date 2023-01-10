#include "include/message.h"

#ifdef BYTE_TYPE_VERSION
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
#endif

#ifdef MPI_TYPE_VERSION
void buildDatatype(int msgId, MPI_Datatype *type) {
	switch (msgId)
	{
	case MSG_DICTIONARY:
		buildCharFreqDictionaryType(type);
		break;

	case MSG_ENCODING_DICTIONARY:
		buildCharEncodingDictionaryType(type);
		break;

	default:
		fprintf(stderr, "Error: in build type unknown message id: %d\n", msgId);
		break;
	}
}

void buildCharFreqDictionaryType(MPI_Datatype *charFrecDictType) {
	int blockLengths[] = {1, 1};
	MPI_Datatype types[] = {MPI_CHAR, MPI_INT};
	MPI_Aint offsets[MSG_DICTIONARY_SIZE];

	offsets[0] = offsetof(CharFreq, character);
	offsets[1] = offsetof(CharFreq, frequency);

	MPI_Type_create_struct(MSG_DICTIONARY_SIZE, blockLengths, offsets, types, charFrecDictType);
	MPI_Type_commit(charFrecDictType);
}

void buildCharEncodingDictionaryType(MPI_Datatype *charEncDictType) {
	int blockLengths[] = {1, 1};
    MPI_Datatype types[] = {MPI_CHAR, MPI_INT};
    MPI_Aint offsets[MSG_ENCODING_DICTIONARY_SIZE];

    offsets[0] = offsetof(CharEncoding, character);
    offsets[1] = offsetof(CharEncoding, length);

    MPI_Type_create_struct(MSG_ENCODING_DICTIONARY_SIZE, blockLengths, offsets, types, charEncDictType);
    MPI_Type_commit(charEncDictType);
}

BYTE* getMessage(MsgHeader *header, void *data) {
	switch (header->id)
	{
	case MSG_DICTIONARY:
		return serializeMsgCharFreqDictionary(header, (CharFreqDictionary*)data);
		break;

	case MSG_ENCODING_TEXT:
		return serializeMsgEncodingText(header, (EncodingText*)data);
		break;
	
	case MSG_ENCODING_DICTIONARY:
		return serializeMsgCharEncodingDictionary(header, (CharEncodingDictionary*)data);
		break;

	case MSG_TEXT:
		return serializeMsgText(header, (char*)data);
		break;
	
	default:
		fprintf(stderr, "Error: in get message unknown message id: %d\n", header->id);
		return NULL;
	}

	return NULL;
}

BYTE* serializeMsgCharFreqDictionary(MsgHeader *header, CharFreqDictionary *dict) {
	header->size = sizeof(int) + (sizeof(CharFreq) * dict->number_of_chars);

	BYTE *buffer = calloc((header->size), sizeof(BYTE));
	header->position = 0;

	MPI_Pack(&dict->number_of_chars, 1, MPI_INT, buffer, header->size, &header->position, MPI_COMM_WORLD);
    MPI_Pack(dict->charFreqs, dict->number_of_chars, *header->type, buffer, header->size, &header->position, MPI_COMM_WORLD);

	return buffer;
}

BYTE* serializeMsgEncodingText(MsgHeader *header, EncodingText *encodedText) {
	int dimSize = sizeof(unsigned short) * encodedText->nr_of_dim;
	int encTextSize = sizeof(BYTE) * encodedText->nr_of_bytes;
	header->size = (MSG_ENCODING_TEXT_SIZE * sizeof(unsigned int)) + dimSize + encTextSize;

	BYTE *buffer = calloc((header->size), sizeof(BYTE));
	header->position = 0;

	MPI_Pack(&encodedText->nr_of_dim, 1, MPI_UNSIGNED, buffer, header->size, &header->position, MPI_COMM_WORLD);
    MPI_Pack(&encodedText->nr_of_bytes, 1, MPI_UNSIGNED, buffer, header->size, &header->position, MPI_COMM_WORLD);
    MPI_Pack(encodedText->dimensions, encodedText->nr_of_dim, MPI_UNSIGNED_SHORT, buffer, header->size, &header->position, MPI_COMM_WORLD);
    MPI_Pack(encodedText->encodedText, encodedText->nr_of_bytes, MPI_BYTE, buffer, header->size, &header->position, MPI_COMM_WORLD);

	return buffer;
}

BYTE* serializeMsgCharEncodingDictionary(MsgHeader *header, CharEncodingDictionary *dict) {
	header->size = sizeof(int);
	BYTE *buffer = malloc(sizeof(BYTE) * (header->size));

	MPI_Pack(&dict->number_of_chars, 1, MPI_INT, buffer, header->size, &header->position, MPI_COMM_WORLD);

	for (int i = 0; i < dict->number_of_chars; i++) {
		header->size += sizeof(char) + sizeof(int) + (sizeof(char) * (dict->charEncoding[i].length+1));
		buffer = realloc(buffer, header->size);

		MPI_Pack(&dict->charEncoding[i], 1, *header->type, buffer, header->size, &header->position, MPI_COMM_WORLD);
		MPI_Pack(dict->charEncoding[i].encoding, (dict->charEncoding[i].length+1), MPI_CHAR, buffer, header->size, &header->position, MPI_COMM_WORLD);
	}

	return buffer;
}

BYTE* serializeMsgText(MsgHeader *header, char *text) {
	int textLen = strlen(text) + 1;
	header->size = (sizeof(int) * MSG_TEXT_SIZE) + (sizeof(char) * textLen);
	
	BYTE *buffer = malloc(sizeof(BYTE) * (header->size));
	header->position = 0;

	MPI_Pack(&textLen, 1, MPI_INT, buffer, header->size, &header->position, MPI_COMM_WORLD);
	MPI_Pack(text, textLen, MPI_CHAR, buffer, header->size, &header->position, MPI_COMM_WORLD);

	return buffer;
}
#endif

#ifdef BYTE_TYPE_VERSION
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
#endif

#ifdef MPI_TYPE_VERSION
void setMessage(MsgHeader *header, void *data, BYTE *buffer) {
	switch (header->id)
	{
	case MSG_DICTIONARY:
		deserializeMsgCharFreqDictionary(header, (CharFreqDictionary*)data, buffer);
		break;

	case MSG_ENCODING_TEXT:
		deserializeMsgEncodingText(header, (EncodingText*)data, buffer);
		break;

	case MSG_ENCODING_DICTIONARY:
		deserializeMsgCharEncodingDictionary(header, (CharEncodingDictionary*)data, buffer);
		break;

	case MSG_TEXT:
		deserializeMsgText(header, (DecodingText*)data, buffer);
		break;

	default:
		fprintf(stderr, "Error: in set message unknown message id: %d\n", header->id);
		break;
	}
}

void deserializeMsgCharFreqDictionary(MsgHeader *header, CharFreqDictionary *dict, BYTE *buffer) {
	MPI_Unpack(buffer, header->size, &header->position, &dict->number_of_chars, 1, MPI_INT, MPI_COMM_WORLD);

	dict->charFreqs = calloc(dict->number_of_chars, sizeof(CharFreq));
    MPI_Unpack(buffer, header->size, &header->position, dict->charFreqs, dict->number_of_chars, *header->type, MPI_COMM_WORLD);
}

void deserializeMsgEncodingText(MsgHeader *header, EncodingText *encodedText, BYTE *buffer) {
	MPI_Unpack(buffer, header->size, &header->position, &encodedText->nr_of_dim, 1, MPI_UNSIGNED, MPI_COMM_WORLD);
	MPI_Unpack(buffer, header->size, &header->position, &encodedText->nr_of_bytes, 1, MPI_UNSIGNED, MPI_COMM_WORLD);

	encodedText->dimensions = calloc(encodedText->nr_of_dim, sizeof(unsigned short));
	MPI_Unpack(buffer, header->size, &header->position, encodedText->dimensions, encodedText->nr_of_dim, MPI_UNSIGNED_SHORT, MPI_COMM_WORLD);

	encodedText->encodedText = calloc(encodedText->nr_of_bytes, sizeof(BYTE));
	MPI_Unpack(buffer, header->size, &header->position, encodedText->encodedText, encodedText->nr_of_bytes, MPI_CHAR, MPI_COMM_WORLD);
}

void deserializeMsgCharEncodingDictionary(MsgHeader *header, CharEncodingDictionary *dict, BYTE *buffer) {
	MPI_Unpack(buffer, header->size, &header->position, &dict->number_of_chars, 1, MPI_INT, MPI_COMM_WORLD);
	dict->charEncoding = malloc(sizeof(CharEncoding) * dict->number_of_chars);

	for (int i = 0; i < dict->number_of_chars; i++) {
		MPI_Unpack(buffer, header->size, &header->position, &dict->charEncoding[i], 1, *header->type, MPI_COMM_WORLD);

		dict->charEncoding[i].encoding = malloc(sizeof(char) * (dict->charEncoding[i].length+1));
		MPI_Unpack(buffer, header->size, &header->position, dict->charEncoding[i].encoding, (dict->charEncoding[i].length+1), MPI_CHAR, MPI_COMM_WORLD);
	}
}

void deserializeMsgText(MsgHeader *header, DecodingText *decodedText, BYTE *buffer) {
	MPI_Unpack(buffer, header->size, &header->position, &decodedText->length, 1, MPI_INT, MPI_COMM_WORLD);

	decodedText->decodedText = calloc(decodedText->length, sizeof(char));
    MPI_Unpack(buffer, header->size, &header->position, decodedText->decodedText, decodedText->length, MPI_CHAR, MPI_COMM_WORLD);
}
#endif

BYTE* prepareForReceive(MsgProbe *probe, MPI_Status *status) {
	MPI_Probe(probe->pid, probe->tag, MPI_COMM_WORLD, status);

	// when probe returns, the status object has the size and other
	// attributes of the incoming message
	// get the message size
	MPI_Get_count(status, MPI_BYTE, &probe->header.size);

	// now receive the message with the allocated buffer
	BYTE *buffer = calloc(probe->header.size, sizeof(BYTE));

	return buffer;
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
