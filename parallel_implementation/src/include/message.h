#pragma once

#include "global_constants.h"

#include "char_freq.h"
#include "encoding.h"

enum Messages {
    // message number slave --> master
    MSG_DICTIONARY = 0,
    MSG_ENCODING = 1
};

enum Child {
    LEFT = 0,
    RIGHT = 1
};

typedef struct MsgHeader {
    int id; // messaget type
    int size; // size of the message in bytes
} MsgHeader;

typedef struct MsgGeneric {
    MsgHeader header;
} MsgGeneric;

typedef struct MsgCharFreqDictionary {
    MsgHeader header;
    int charsNr;
    CharFreq *charFreqs;
} MsgCharFreqDictionary;

// typedef struct MsgEncodingDictionary {
//     MsgHeader header;
//     unsigned int charNr;
//     ArrayNode *nodes;
// } MsgEncodingDictionary;

extern BYTE* getMessage(void *data, int msgType, int *bufferSize);
extern void setMessage(void *data, BYTE *buffer);

BYTE* serializeMsgCharFreqDictionary(CharFreqDictionary* dict, int *bufferSize);
void deserializeMsgCharFreqDictionary(CharFreqDictionary* dict, BYTE *buffer);

// BYTE* serializeMsgEncodingDictionary(TreeNode *root, int *bufferSize);
// void addNode(TreeNode *node, BYTE* msg, int *idx);
// void deserializeMsgEncodingDictionary(TreeNode **root, BYTE *buffer);
// int extractNodes(TreeNode **root, ArrayNode *node, int idx);

void printCharFreqDictionary(CharFreqDictionary *dict);
