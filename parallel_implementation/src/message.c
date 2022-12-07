#include "include/message.h"

BYTE* getMessage(void* data, int msgType, int *bufferSize) {
	switch (msgType)
	{
	case MSG_DICTIONARY:
		return serializeMsgCharFreqDictionary((CharFreqDictionary*)data, bufferSize);
		break;
	
	case MSG_ENCODING:
		return serializeMsgEncodingDictionary((TreeNode*)data, bufferSize);
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

BYTE* serializeMsgEncodingDictionary(TreeNode *root, int *bufferSize) {
	int treeSize = *bufferSize;
	*bufferSize = (sizeof(int) * 3) + (treeSize * sizeof(TreeNodeShort));
	BYTE *buffer = malloc(sizeof(BYTE) * (*bufferSize));

	int offset = 0;
	MsgHeader header = {MSG_ENCODING, (*bufferSize)};
    memcpy(buffer + (offset * sizeof(int)), &header.id, sizeof(int));
	++offset;
    
	memcpy(buffer + (offset * sizeof(int)), &header.size, sizeof(int));
	++offset;

	memcpy(buffer + (offset * sizeof(int)), &treeSize, sizeof(int));
	++offset;

	int nodeNr = 0;
	addNode(root, buffer, &nodeNr);

	return buffer;
}

void addNode(TreeNode *node, BYTE *msg, int *idx) {
	TreeNodeShort msgNode = {.character = node->character, .children = 0};

	const int pos = *idx;
	++(*idx);

	if (*idx < 4) {
		if (node->leftChild != NULL) {
			SetBit(msgNode.children, LEFT); 
			addNode(node->leftChild, msg, idx);
		}

		if (node->rightChild != NULL) {
			SetBit(msgNode.children, RIGHT); 
			addNode(node->rightChild, msg, idx), idx;
		}
	}

	memcpy(msg + (sizeof(int) * 3) + (pos * sizeof(TreeNodeShort)) , &msgNode, sizeof(TreeNodeShort));
}

void setMessage(void *data, BYTE *buffer) {
	MsgGeneric *p = (MsgGeneric*)buffer;
	int msgId = p->header.id;
	switch (msgId)
	{
	case MSG_DICTIONARY:
		deserializeMsgCharFreqDictionary((CharFreqDictionary*)data, buffer);
		break;

	case MSG_ENCODING:
		// deserializeMsgEncodingDictionary((TreeNode*)data);
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

// void deserializeMsgEncodingDictionary(TreeNode *root) {
// 	// TODO
// }

void printCharFreqDictionary(CharFreqDictionary* dict) {
	printf("number of chars: %d\n", dict->number_of_chars);

	for (int i = 0; i < dict->number_of_chars; i++) {
		printf("char: ");
		printFormattedChar(dict->charFreqs[i].character);
		printf(", freq: %d\n", dict->charFreqs[i].frequency);
	}
}

// //////////////////////////////////////////////////////////////////////////

// BYTE* serializeMsgCharFreqDictionary(CharFreqDictionary* dict, int *bufferSize) {
// 	*bufferSize = sizeof (MsgCharFreqDictionary) + (sizeof(CharFreq) * dict->number_of_chars);

// 	// *bufferSize = sizeof(int) + (sizeof(CharFreq) * dict->number_of_chars);

// 	printf("sizeof(MsgHeader): %ld\n", sizeof(MsgHeader));
// 	printf("sizeof(int): %ld\n", sizeof(int));
// 	printf("sizeof(MsgHeader) + sizeof(int): %ld\n", sizeof(MsgHeader) + sizeof(int));
// 	printf("sizeof(MsgHeader) + sizeof(int) + sizeof(CharFreq*): %ld\n", sizeof(MsgHeader) + sizeof(int) + sizeof(CharFreq*));
// 	printf("sizeof (MsgCharFreqDictionary): %ld\n", sizeof (MsgCharFreqDictionary));
// 	printf("sizeof(CharFreq*): %ld\n", sizeof(CharFreq*));
// 	printf("sizeof(CharFreq): %ld\n", sizeof(CharFreq));
// 	printf("sizeof(CharFreq) * dict->number_of_chars: %ld\n", sizeof(CharFreq) * dict->number_of_chars);
// 	printf("bufferSize: %d\n", *bufferSize);
// 	printf("sizeof(BYTE): %ld\n", sizeof(BYTE));

// 	MsgCharFreqDictionary *buffer = malloc(sizeof(BYTE) * (*bufferSize));

// 	printf("buffer->header: %ld\n", sizeof(buffer->header));
// 	printf("void: %ld\n", sizeof(void));


// 	MsgHeader header = {MSG_DICTIONARY, (*bufferSize)};
// 	printf("buffer: %p\n", buffer);
//     memcpy(buffer, &header, sizeof(MsgHeader));
//     printf("buffer->header.id: %d\n", buffer->header.id);
// 	printf("buffer->header.size: %d\n", buffer->header.size);
	
// 	printf("buffer: %p\n", buffer);
// 	printf("dict->number_of_chars: %d\n", dict->number_of_chars);
// 	memcpy(buffer + sizeof(MsgHeader) + 4, &dict->number_of_chars, sizeof(int));
// 	printf("buffer->charsNr: %d\n", buffer->charsNr);

// 	void *tmp = buffer + sizeof(MsgHeader) + 4;
// 	printf("buffer + sizeof(MsgHeader) + 4 add: %p\n", tmp);

// 	void *tmp4 = buffer;
// 	printf("tmp4 add: %p\n", tmp4);
// 	tmp4 = tmp4 + sizeof(MsgHeader);
// 	printf("tmp4 + sizeof(MsgHeader) add: %p\n", tmp4);

// 	printf("sizeof(buffer): %ld\n", sizeof(buffer));

// 	void *tmp3 = buffer + sizeof(MsgHeader);
// 	printf("tmp3 add: %p\n", tmp3);
// 	printf("buffer + sizeof(MsgHeader) add: %p\n", tmp3);

// 	printf("buffer->charsNr add: %p\n", &buffer->charsNr);
// 	printf("buffer: %p\n", buffer);
//     // memcpy(buffer + sizeof(MsgHeader) + sizeof(int), dict->charFreqs, sizeof(CharFreq) * dict->number_of_chars);

// 	// printf("Sended dictionary with %d chars:\n", dict->number_of_chars);
// 	// printCharFreqDictionary(dict);

// 	// printf("\nSended buffer with %d chars:\n", buffer->charsNr);
// 	// printf("tmp->charFreqs: %p\n", buffer->charFreqs);

// 	// printf("char: ");
// 	// printFormattedChar((tmp->charFreqs[0].character));
// 	// printf(", freq: %d\n", tmp->charFreqs[0].frequency);

// 	// for (int i = 0; i < tmp->charsNr; i++) {
// 	// 	printf("char: ");
// 	// 	printFormattedChar(tmp->charFreqs[i].character);
// 	// 	printf(", freq: %d\n", tmp->charFreqs[i].frequency);
// 	// }

// 	BYTE *tmp2 = malloc(sizeof(MsgHeader) + sizeof(int));
// 	return tmp2;
// }

// void getMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDict) {
// 	int i;

// 	printf("Message: msgDict->charsNr: %d\n", msgDict->charsNr);
// 	printf("Message: msgDict->charFreqs: %p\n", msgDict->charFreqs);


// 	printf("Message: dict->charFreqs: %p\n", dict->charFreqs);

// 	dict->number_of_chars = msgDict->charsNr;
// 	dict->charFreqs = malloc(dict->number_of_chars * sizeof(CharFreq));

// 	printf("Message: dict->number_of_chars: %d\n", dict->number_of_chars);
// 	printf("Message: dict->charFreqs: %p\n", dict->charFreqs);

// 	// // maybe we could use memcpy here
// 	// for (i = 0; i < msgDict->charsNr; i++) {
// 	// 	dict->charFreqs[i].character = msgDict->charFreqs[i].character;
// 	// 	dict->charFreqs[i].frequency = msgDict->charFreqs[i].frequency;
// 	// }

// 	// for (i = 0; i < msgDict->charsNr; i++) {
// 	// 	printf("msgDict->charFreqs[%d].character: %c\n", i, msgDict->charFreqs[i].character);
// 	// 	printf("msgDict->charFreqs[%d].frequency: %d\n", i, msgDict->charFreqs[i].frequency);
// 	// }

// 	// CharFreq *tmp = msgDict->charFreqs;
// 	// printf("charFreqs[0]\tchar: %c\tfreq:%d\n", tmp->character, tmp->frequency);
// }

// /////////////////////////////////////////////////////////////////////////////////////////

// MsgCharFreqDictionary* createMsgCharFreqDictionaryFromFreqs(CharFreqDictionary* allChars){
// 	MsgCharFreqDictionary* msgDictSnd = malloc(sizeof(MsgCharFreqDictionary));

// 	initMsgHeader(&msgDictSnd->header, MSG_DICTIONARY, 0);
// 	msgDictSnd->charsNr = 0;
// 	msgDictSnd->charFreqs = NULL;

// 	buildMsgCharFreqDictionary(allChars, msgDictSnd);

// 	return msgDictSnd;
// }

// void buildMsgCharFreqDictionary(CharFreqDictionary* dict, MsgCharFreqDictionary* msgDict) {
// 	#if VERBOSE <= 2
// 		printMessageHeader(&msgDict->header);
// 	#endif

// 	msgDict->header.size = sizeof(MsgCharFreqDictionary) + dict->number_of_chars * sizeof(CharFreq);
// 	msgDict->charsNr = dict->number_of_chars;
// 	msgDict->charFreqs = malloc(dict->number_of_chars * sizeof(CharFreq));

// 	// maybe we could use memcpy here
// 	for (int i = 0; i < dict->number_of_chars; i++) {
// 		msgDict->charFreqs[i].character = dict->charFreqs[i].character;
// 		msgDict->charFreqs[i].frequency = dict->charFreqs[i].frequency;
// 	}
// }

// BYTE* createMessageBufferFromMsgCharFreqDictionary(MsgCharFreqDictionary* msgDictSnd, int bufferSize){
// 	BYTE *buffer = malloc(sizeof(BYTE) * bufferSize);
// 	memcpy(buffer, &msgDictSnd->header, sizeof(MsgHeader));
// 	memcpy(buffer + sizeof(MsgHeader), &msgDictSnd->charsNr, sizeof(int));
// 	memcpy(buffer + sizeof(MsgHeader) + sizeof(int), msgDictSnd->charFreqs, sizeof(CharFreq) * msgDictSnd->charsNr);

// 	return buffer;
// }

// MsgCharFreqDictionary* createMsgCharFreqDictionaryFromByteBuffer(BYTE *buffer){
// 	MsgCharFreqDictionary* msgRcv = malloc(sizeof(MsgCharFreqDictionary));

//     initMsgHeader(&msgRcv->header, MSG_DICTIONARY, 0);
// 	msgRcv->charsNr = 0;
// 	msgRcv->charFreqs = NULL;

// 	// copy the data from the buffer to the message
// 	memcpy(&msgRcv->header, buffer, sizeof(MsgHeader));
// 	memcpy(&msgRcv->charsNr, buffer + sizeof(MsgHeader), sizeof(int));

// 	msgRcv->charFreqs = malloc(sizeof(CharFreq) * msgRcv->charsNr);
// 	memcpy(msgRcv->charFreqs, buffer + sizeof(MsgHeader) + sizeof(int), sizeof(CharFreq) * msgRcv->charsNr);

// 	return msgRcv;
// }

// // --- MsgEncodingDictionary funcs ---
// MsgEncodingDictionary* createMsgEncodingDictionaryFromFreqs(CharEncoding* allEncodings, int size){
// 	MsgEncodingDictionary* msgDictSnd = malloc(sizeof(MsgEncodingDictionary));
    
// 	initMsgHeader(&msgDictSnd->header, MSG_DICTIONARY, 0);
	
// 	msgDictSnd->header.size = sizeof(MsgCharFreqDictionary) + size * sizeof(CharEncoding);
// 	msgDictSnd->encodingNr = size;
// 	msgDictSnd->charEncodings = malloc(size * sizeof(CharEncoding));

// 	// maybe we could use memcpy here
// 	for (int i = 0; i < size; i++) {
// 		msgDictSnd->charEncodings[i].character = allEncodings[i].character;
// 		msgDictSnd->charEncodings[i].encoding = allEncodings[i].encoding;
// 	}

// 	return msgDictSnd;
// }

// BYTE* createMessageBufferFromMsgEncodingDictionary(MsgEncodingDictionary* msgDictSnd, int bufferSize){
// 	BYTE *buffer = malloc(sizeof(BYTE) * bufferSize);
// 	memcpy(buffer, &msgDictSnd->header, sizeof(MsgHeader));
// 	memcpy(buffer + sizeof(MsgHeader), &msgDictSnd->encodingNr, sizeof(int));
// 	memcpy(buffer + sizeof(MsgHeader) + sizeof(int), msgDictSnd->charEncodings, sizeof(CharEncoding) * msgDictSnd->encodingNr);

// 	return buffer;
// }

// MsgEncodingDictionary* createMsgEncodingDictionaryFromByteBuffer(BYTE *buffer){
// 	MsgEncodingDictionary* msgRcv = malloc(sizeof(MsgEncodingDictionary));

//     initMsgHeader(&msgRcv->header, MSG_DICTIONARY, 0);
// 	msgRcv->encodingNr = 0;
// 	msgRcv->charEncodings = NULL;

// 	// copy the data from the buffer to the message
// 	memcpy(&msgRcv->header, buffer, sizeof(MsgHeader));
// 	memcpy(&msgRcv->encodingNr, buffer + sizeof(MsgHeader), sizeof(int));

// 	msgRcv->charEncodings = malloc(sizeof(CharEncoding) * msgRcv->encodingNr);
// 	memcpy(msgRcv->charEncodings, buffer + sizeof(MsgHeader) + sizeof(int), sizeof(CharEncoding) * msgRcv->encodingNr);

// 	return msgRcv;
// }

// // TODO AC
// void printMessageHeader(MsgCharFreqDictionary* msg) {
// 	printf("Message: msg->header.size: %d\n", msg->header.size);
// 	printf("Message: msg->header.id: %d\n", msg->header.id);
// 	printf("Message: msg->charsNr: %d\n", msg->charsNr);
// 	printf("Message: msg->charFreqs: %p\n", msg->charFreqs);
// }

// // TODO AC
// void printSizeOfMessage(MsgCharFreqDictionary* msg) {
// 		#if VERBOSE <= 2
// 		printf("size of int: %lu\n", sizeof(int));
// 		printf("size of char: %lu\n", sizeof(char));
// 		printf("size of msgHeader: %lu\n", sizeof(MsgHeader));
// 		printf("size of msgDict: %lu\n", sizeof(MsgDictionary));
// 		printf("size of CharFreq: %lu\n", sizeof(CharFreq));
// 		printf("size of CharFreq * number of chars : %lu * %u = %lu\n", sizeof(CharFreq), dict->number_of_chars, sizeof(CharFreq) * dict->number_of_chars);
// 	#endif
// }

// TODO AC
// void foo() {
	// for (int i = 0; i < msgEnc.charNr; i++) {
	// 	char c[3];

	// 	if (msgEnc.treeNodes[i].character == '\0') {
	// 		c[0] = '\\';
	// 		c[1] = '0';
	// 		c[2] = '\0';
	// 	}
	// 	else if (msgEnc.treeNodes[i].character == '\n') {
	// 		c[0] = '\\';
	// 		c[1] = 'n';
	// 		c[2] = '\0';
	// 	}
	// 	else if (msgEnc.treeNodes[i].character == ' ') {
	// 		c[0] = '\\';
	// 		c[1] = 'b';
	// 		c[2] = '\0';
	// 	}
	// 	else {
	// 		c[0] = msgEnc.treeNodes[i].character;
	// 		c[1] = '\0';
	// 	}
		
	// 	printf("node %d:\n\tchar: %s\n\thas child left: %d\n\thas child right: %d\n\n", 
	// 		i,
	// 		c,
	// 		IsBit(msgEnc.treeNodes[i].childs, LEFT),
	// 		IsBit(msgEnc.treeNodes[i].childs, RIGHT));
	// }
// }
