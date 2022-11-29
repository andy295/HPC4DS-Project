#pragma once

#include "global_constants.h"

#include "char_freq.h"

enum Messages {
    // message number slave --> master
    MSG_DICTIONARY = 1

    // message number master --> slave
    // not used
};

typedef struct MsgHeader
{
    int id; // messaget type
    int size; // size of the message in bytes
} MsgHeader;

typedef struct MsgGeneric
{
    MsgHeader header;
} MsgGeneric;

typedef struct MsgDictionary
{
    MsgHeader header;
    int charsNr;
    CharFreq *charFreqs;
} MsgDictionary;

extern MsgDictionary* createMsgDictionaryFromFreqs(CharFreqDictionary* allChars);
extern void createMsgDictFromByteBuffer(MsgDictionary* msgRcv, BYTE *buffer);
extern BYTE* createMessageBufferFromDict(MsgDictionary* msg, int bufferSize);

void initMsgHeader(MsgHeader* header, int id, int size);
void buildMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDict);

void printMessageHeader(MsgDictionary* msg); 
