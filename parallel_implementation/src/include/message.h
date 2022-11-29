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

void initMsgHeader(MsgHeader* header, int id, int size);
extern void initMsgDictionary(MsgDictionary* msg);

extern void setMsg(void* dict, MsgGeneric* msg);
// extern void getMsg(void* dict, MsgGeneric* msg);

extern void getCharFreqDicFromMsgDict(CharFreqDictionary* dict, MsgDictionary* msgDict);
extern void getMsgDictFromByteBuffer(MsgDictionary* msgRcv, BYTE *buffer);

void buildMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDict);

// one of them must be removed
// void mergeCharFreqs(CharFreqDictionary*  dict, MsgDictionary* msgDict);

void printMessageHeader(MsgDictionary* msg); 

BYTE* createMessageBufferFromDict(MsgDictionary* msg, int bufferSize);