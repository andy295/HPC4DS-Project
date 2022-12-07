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

typedef struct TreeNodeShort {
    char character; 
    unsigned char children;
} TreeNodeShort;

typedef struct MsgEncodingDictionary {
    MsgHeader header;
    unsigned int charNr;
    TreeNodeShort *treeNodes;
} MsgEncodingDictionary;

extern BYTE* getMessage(void* data, int msgType, int *bufferSize);
extern void setMessage(void *data, BYTE *buffer);

BYTE* serializeMsgCharFreqDictionary(CharFreqDictionary* dict, int *bufferSize);
void deserializeMsgCharFreqDictionary(CharFreqDictionary* dict, BYTE *buffer);

BYTE* serializeMsgEncodingDictionary(TreeNode *root, int *bufferSize);
void addNode(TreeNode *node, BYTE* msg, int *idx);
// void deserializeMsgEncodingDictionary(TreeNode *root);

void printCharFreqDictionary(CharFreqDictionary* dict);

// one of them must be removed
// void mergeCharFreqs(CharFreqDictionary*  dict, MsgCharFreqDictionary* msgDict);
// void getMsgDictionary(CharFreqDictionary* dict, MsgCharFreqDictionary* msgDict);

// // --- MsgCharFreqDictionary functions ---
// extern MsgCharFreqDictionary* createMsgCharFreqDictionaryFromFreqs(CharFreqDictionary* allChars);
// extern MsgCharFreqDictionary* createMsgCharFreqDictionaryFromByteBuffer(BYTE *buffer);
// extern BYTE* createMessageBufferFromMsgCharFreqDictionary(MsgCharFreqDictionary* msg, int bufferSize);
// void buildMsgCharFreqDictionary(CharFreqDictionary* dict, MsgCharFreqDictionary* msgDict);

// // --- MsgEncodingDictionary creation functions ---
// extern MsgEncodingDictionary* createMsgEncodingDictionaryFromFreqs(CharEncoding* allEncodings, int size);
// extern MsgEncodingDictionary* createMsgEncodingDictionaryFromByteBuffer(BYTE *buffer);
// extern BYTE* createMessageBufferFromMsgEncodingDictionary(MsgEncodingDictionary* msg, int bufferSize);

// void initMsgHeader(MsgHeader* header, int id, int size);

// extern void createMsgEncoding(TreeNode *root);
// void printMessageHeader(MsgCharFreqDictionary* msg); 
