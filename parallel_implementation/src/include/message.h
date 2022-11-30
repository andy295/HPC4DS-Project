#pragma once

#include "global_constants.h"

#include "char_freq.h"
#include "encoding.h"

enum Messages {
    // message number slave --> master
    MSG_DICTIONARY = 1

    // message number master --> slave
    // not used
};

typedef struct MsgHeader {
    int id; // messaget type
    int size; // size of the message in bytes
} MsgHeader;

// TODO: per andrea, iniziando a fare la parte di passaggio degli encoding, 
// 	 potrebbe avere senso usare i generics, ma non so se vale la pena perderci tempo
// 	 io non saprei come fare, se vuoi metterti tu, altrimenti facciamo senza
typedef struct MsgGeneric {
    MsgHeader header;
} MsgGeneric;

typedef struct MsgCharFreqDictionary {
    MsgHeader header;
    int charsNr;
    CharFreq *charFreqs;
} MsgCharFreqDictionary;

typedef struct MsgEncodingDictionary {
    MsgHeader header;
    int encodingNr;
    CharEncoding *charEncodings;
} MsgEncodingDictionary;


// --- MsgCharFreqDictionary functions ---
extern MsgCharFreqDictionary* createMsgCharFreqDictionaryFromFreqs(CharFreqDictionary* allChars);
extern MsgCharFreqDictionary* createMsgCharFreqDictionaryFromByteBuffer(BYTE *buffer);
extern BYTE* createMessageBufferFromMsgCharFreqDictionary(MsgCharFreqDictionary* msg, int bufferSize);
void buildMsgCharFreqDictionary(CharFreqDictionary* dict, MsgCharFreqDictionary* msgDict);

// --- MsgEncodingDictionary creation functions ---
extern MsgEncodingDictionary* createMsgEncodingDictionaryFromFreqs(CharEncoding* allEncodings, int size);
extern MsgEncodingDictionary* createMsgEncodingDictionaryFromByteBuffer(BYTE *buffer);
extern BYTE* createMessageBufferFromMsgEncodingDictionary(MsgEncodingDictionary* msg, int bufferSize);


void initMsgHeader(MsgHeader* header, int id, int size);
void printMessageHeader(MsgCharFreqDictionary* msg); 
