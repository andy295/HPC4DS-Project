#pragma once

#include "global_constants.h"

#include "message.h"
#include "huffman_tree.h"
#include "encoding.h"

#include "../utils/file_utils.h"

BYTE* prepareForReceive(MPI_Status *status, int *bufferSize, int pid, int tag);
void calculateBlockRange(int nrOfBlocks, int nrOfProcs, int pid, int *start, int *end);
int roundUp(int numToRound, int multiple);