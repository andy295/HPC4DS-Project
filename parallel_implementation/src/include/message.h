#pragma once

#include "global_constants.h"

#include "char_freq.h"
#include "encoding.h"

enum Messages {
    // message number slave --> master
    MSG_DICTIONARY = 0,
    MSG_ENCODING = 1,
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

typedef struct MsgCharEncodingDictionary {
    MsgHeader header;
    int charsNr;
    CharEncoding *charEncoding;
} MsgCharEncodingDictionary;

extern BYTE* getMessage(void *data, int msgType, int *bufferSize);
extern void setMessage(void *data, BYTE *buffer);

BYTE* serializeMsgCharFreqDictionary(CharFreqDictionary *dict, int *bufferSize);
void deserializeMsgCharFreqDictionary(CharFreqDictionary *dict, BYTE *buffer);

BYTE* serializeMsgCharEncodingDictionary(CharEncodingDictionary *dict, int *bufferSize);
void deserializeMsgCharEncodingDictionary(CharEncodingDictionary *dict, BYTE *buffer);
