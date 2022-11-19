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
// #include "../utils/string_utils.h"
// #include utils/time_utils.c"
// #include utils/print_utils.c"

const char *fileName = "parallel_implementation/src/text.txt";

enum Messages {
    // message number slave --> master
    MSG_DICTIONARY = 1

    // message number master --> slave
    // not used
};

enum MessageSize {
    ZERO = 0,
    ONE = 1,
    TWO = 2,
    THREE = 3,
    FOUR = 4,
    FIVE = 5
};

typedef struct MsgHeader
{
    int id;         // messaget type
    int size;       // number of variable in message, header included
} MsgHeader;

typedef struct MsgDictionary
{
    MsgHeader header;
    int charsNr;
    char *characters;
    int *frequencies;
} MsgDictionary;

extern void fillMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDictionary, MPI_Datatype* newType);
extern void getMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDict);

extern void freeBuffer(void* buffer);