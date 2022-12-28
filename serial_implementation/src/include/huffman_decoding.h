#pragma once

#include "global_constants.h"

#include "message.h"
#include "huffman_tree.h"
#include "encoding.h"

#include "../utils/file_utils.h"
#include "../utils/time_utils.h"

void calculateBlockRange(int nrOfBlocks, int nrOfProcs, int pid, int *start, int *end);
int roundUp(int numToRound, int multiple);

extern int huffman_decoding();
