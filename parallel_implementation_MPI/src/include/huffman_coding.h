#pragma once

#include "global_constants.h"

#include "char_freq.h"
#include "message.h"
#include "huffman_tree.h"
#include "encoding.h"

#include "../utils/file_utils.h"
#include "../utils/time_utils.h"
#include "../utils/data_logger.h"
#include "../utils/conversion_utils.h"

bool useMasterProcess(int *proc_number, bool withMaster);
bool calculateSenderReceiver(int proc_number, int pid, int *sender, int *receiver);
void recvEncodingText(EncodingText *encodingText, int sender);
void unorderedSendRecv(int proc_number, int pid, CharFreqDictionary *dict, MPI_Datatype *charFreqDictType, bool withMaster);
void semiOrderedSendRecv(int pid, EncodingText *encodingText, int sender, int receiver);
