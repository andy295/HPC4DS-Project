#pragma once

#include "global_constants.h"

#include "char_freq.h"
#include "encoding.h"

enum Messages {
    // message number slave --> master
    MSG_DICTIONARY = 0,
    MSG_ENCODING_DICTIONARY = 1,
    MSG_ENCODING_TEXT = 2,
    MSG_TEXT = 3,
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

typedef struct MsgEncodingText {
    MsgHeader header;
    int nrOfPos;
    int nrOfBytes;
    short *positions;
    BYTE *text;
} MsgEncodingText;

typedef struct MsgText {
    MsgHeader header;
    int textLength;
    BYTE *text;
} MsgText;

BYTE* serializeMsgCharFreqDictionary(CharFreqDictionary *dict, int *bufferSize);
void deserializeMsgCharFreqDictionary(CharFreqDictionary *dict, BYTE *buffer);

BYTE* serializeMsgCharEncodingDictionary(CharEncodingDictionary *dict, int *bufferSize);
void deserializeMsgCharEncodingDictionary(CharEncodingDictionary *dict, BYTE *buffer);

BYTE *serializeMsgEncodingText(EncodingText *data, int *bufferSize);
void deserializeMsgEncodingText(EncodingText *data, BYTE *buffer);

BYTE *serializeMsgText(char *data, int *bufferSize);
void deserializeMsgText(DecodingText *decodedText, BYTE *buffer);

extern BYTE* prepareForReceive(MPI_Status *status, int *bufferSize, int pid, int tag);

extern BYTE* getMessage(void *data, int msgType, int *bufferSize);
extern void setMessage(void *data, BYTE *buffer);

extern char* getMsgName(int msgType);
