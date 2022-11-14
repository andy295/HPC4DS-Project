#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <mpi.h>

#include "global_constants.h"
#include "char_freq.h"
// #include "huffman_tree.c"
// #include "letter_encoding.c"

#include "../utils/file_utils.h"
#include "../utils/string_utils.h"
// #include utils/time_utils.c"
// #include utils/print_utils.c"

const char *fileName = "Project/src/text.txt";

enum Messages {
    // message number slave --> master
    MSG_DICTIONARY = 1

    // message number master --> slave
    // not used
};

typedef struct MsgHeader
{
    int id;          // messaget type
    int version;     // version of the message
    int size;        // size of the message in bytes
} MsgHeader;

typedef struct MsgDictionary
{
    MsgHeader header;
    int charsNr;            // messaget type
    CharFreq *charFreqs;    // version
} MsgDictionary;

extern void fillMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDict);
extern void getMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDict);