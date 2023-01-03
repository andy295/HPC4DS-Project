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
    int id; // message id
    int size; // size of the message in bytes
} MsgHeader;

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
    unsigned int nrOfPos;
    unsigned int nrOfBytes;
    short *positions;
    BYTE *text;
} MsgEncodingText;

typedef struct MsgText {
    MsgHeader header;
    int textLength;
    BYTE *text;
} MsgText;

BYTE* serializeMsgCharFreqDictionary(MsgHeader *header, CharFreqDictionary *dict);
void deserializeMsgCharFreqDictionary(MsgHeader *header, CharFreqDictionary *dict, BYTE *buffer);

BYTE* serializeMsgCharEncodingDictionary(MsgHeader *header, CharEncodingDictionary *dict);
void deserializeMsgCharEncodingDictionary(MsgHeader *header, CharEncodingDictionary *dict, BYTE *buffer);

BYTE *serializeMsgEncodingText(MsgHeader *header, EncodingText *encodingText);
void deserializeMsgEncodingText(MsgHeader *header, EncodingText *encodingText, BYTE *buffer);

BYTE *serializeMsgText(MsgHeader *header, char *text);
void deserializeMsgText(MsgHeader *header, DecodingText *decodedText, BYTE *buffer);

extern BYTE* getMessage(MsgHeader *header, void *data);
extern void setMessage(MsgHeader *header, void *data, BYTE *buffer);

extern char* getMsgName(int msgType);
