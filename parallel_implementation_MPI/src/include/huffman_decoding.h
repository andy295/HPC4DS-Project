#pragma once

#include "global_constants.h"

#include "message.h"
#include "huffman_tree.h"
#include "encoding.h"

#include "../utils/file_utils.h"
#include "../utils/time_utils.h"
#include "../utils/conversion_utils.h"
#include "../utils/data_logger.h"
#include "../utils/message_utils.h"

int calculatePrevTextSize(unsigned short *dimensions, int nrOfBlocks);
void calculateBlockRange(int nrOfBlocks, int nrOfProcs, int pid, int *start, int *end);
void recvDecodingText(DecodingText *decodingText, int sender);
void semiOrderedDecTextSendRecv(int pid, DecodingText *decodingText, int sender, int receiver);
