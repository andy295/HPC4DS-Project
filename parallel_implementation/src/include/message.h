#pragma once

#include "global_constants.h"

#include "char_freq.h"
#include "encoding.h"

enum Messages {
    // message number slave --> master
    MSG_DICTIONARY = 0,
    MSG_ENCODING_DICTIONARY = 1,
    MSG_ENCODING_TEXT = 2, // maybe we don't need it, and we can remove it
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

typedef struct MsgEncodingText { // maybe we don't need it, and we can remove it
    MsgHeader header;
    int BitsNr;
    BYTE *text;
} MsgEncodingText;

BYTE* serializeMsgCharFreqDictionary(CharFreqDictionary *dict, int *bufferSize);
void deserializeMsgCharFreqDictionary(CharFreqDictionary *dict, BYTE *buffer);

BYTE* serializeMsgCharEncodingDictionary(CharEncodingDictionary *dict, int *bufferSize);
void deserializeMsgCharEncodingDictionary(CharEncodingDictionary *dict, BYTE *buffer);

BYTE *serializeMsgEncodingText(EncodingText *data, int *bufferSize); // maybe we don't need it, and we can remove it
void deserializeMsgEncodingText(EncodingText *data, BYTE *buffer); // maybe we don't need it, and we can remove it

extern BYTE* getMessage(void *data, int msgType, int *bufferSize);
extern void setMessage(void *data, BYTE *buffer);
