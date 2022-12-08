#include "include/message.h"

BYTE* getMessage(void* data, int msgType, int *bufferSize) {
	switch (msgType)
	{
	case MSG_DICTIONARY:
		return serializeMsgCharFreqDictionary((CharFreqDictionary*)data, bufferSize);
		break;
	
	// case MSG_ENCODING:
	// 	return serializeMsgEncodingDictionary((TreeNode*)data, bufferSize);
	// 	break;

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

// BYTE* serializeMsgEncodingDictionary(TreeNode *root, int *bufferSize) {
// 	MsgEncodingDictionary msg = {.header.id = MSG_ENCODING, .header.size = 0, .charNr = *bufferSize, .nodes = NULL};

// 	*bufferSize = sizeof(MsgEncodingDictionary) + (msg.charNr * sizeof(ArrayNode));
// 	BYTE *buffer = malloc(sizeof(BYTE) * (*bufferSize));

// 	msg.header.size = *bufferSize;

// 	memcpy(buffer, &msg, sizeof(MsgEncodingDictionary));

// 	int idx = 0;
// 	addNode(root, buffer, &idx);

// 	return buffer;
// }

// void addNode(TreeNode *node, BYTE *msg, int *idx) {
// 	ArrayNode msgNode = {.frequency = node->frequency, .character = node->character, .children = 0};

// 	const int pos = *idx;
// 	++(*idx);

// 	if (node->leftChild != NULL) {
// 		SetBit(msgNode.children, LEFT); 
// 		addNode(node->leftChild, msg, idx);
// 	}

// 	if (node->rightChild != NULL) {
// 		SetBit(msgNode.children, RIGHT); 
// 		addNode(node->rightChild, msg, idx), idx;
// 	}

// 	memcpy(msg + sizeof(MsgEncodingDictionary) + (pos * sizeof(ArrayNode)) , &msgNode, sizeof(ArrayNode));
// }

void setMessage(void *data, BYTE *buffer) {
	MsgGeneric *p = (MsgGeneric*)buffer;
	int msgId = p->header.id;
	switch (msgId)
	{
	case MSG_DICTIONARY:
		deserializeMsgCharFreqDictionary((CharFreqDictionary*)data, buffer);
		break;

	// case MSG_ENCODING:
	// 	deserializeMsgEncodingDictionary((TreeNode*)data, buffer);
	// 	break;

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

// void deserializeMsgEncodingDictionary(TreeNode **root, BYTE *buffer) {
// 	MsgEncodingDictionary msg;
// 	memcpy(&msg, buffer, sizeof(MsgEncodingDictionary));
// 	msg.nodes = malloc(sizeof(ArrayNode) * msg.charNr);
// 	memcpy(msg.nodes, buffer + sizeof(MsgEncodingDictionary), sizeof(ArrayNode) * msg.charNr);

// 	#if VERBOSE <= 2
// 		for (int i = 0; i < msg.charNr; i++) {
// 			printf("char: ");
// 			printFormattedChar(msg.nodes[i].character);
// 			printf(", freq: %d, children: %d\n", msg.nodes[i].frequency, msg.nodes[i].children);
// 		}
// 	#endif

// 	extractNodes(root, &msg.nodes[0], 0);
// }

// int extractNodes(TreeNode **root, ArrayNode *node, int idx) {
// 	*root = malloc(sizeof(TreeNode));
// 	(*root)->frequency = node->frequency;
// 	(*root)->character = node->character;

// 	if (IsBit(node->children, LEFT)) {
// 		++idx;
// 		idx = extractNodes((*root)->leftChild, node + 1, idx);
// 	}

// 	if (IsBit(node->children, RIGHT))
// 		idx += extractNodes((*root)->rightChild, node + idx, 1);

// 	return idx;
// }

void printCharFreqDictionary(CharFreqDictionary* dict) {
	printf("number of chars: %d\n", dict->number_of_chars);

	for (int i = 0; i < dict->number_of_chars; i++) {
		printf("char: ");
		printFormattedChar(dict->charFreqs[i].character);
		printf(", freq: %d\n", dict->charFreqs[i].frequency);
	}
}
