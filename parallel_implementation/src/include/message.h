#include "global_constants.h"

#include "char_freq.h"

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
    int id; // messaget type
    int size; // number of variable in message, header included
} MsgHeader;

typedef struct MsgGeneric
{
    MsgHeader header;
} MsgGeneric;

typedef struct MsgDictionary
{
    MsgHeader header;
    int charsNr;
    char *characters;
    int *frequencies;
} MsgDictionary;

extern void initMsgDictionary(MsgHeader *header);

extern void setMsg(void* dict, MsgGeneric* msg, MPI_Datatype *msgType);
extern void getMsg(void* dict, MsgGeneric* msg);

extern void buildMsgDictionary(CharFreqDictionary* dict, MsgDictionary* msgDictionary, MPI_Datatype *msgType);
extern void mergeCharFreqs(CharFreqDictionary*  dict, MsgDictionary* msg);
