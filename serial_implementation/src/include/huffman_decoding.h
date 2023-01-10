#pragma once

#include "global_constants.h"

#include "huffman_tree.h"
#include "encoding.h"

#include "../utils/file_utils.h"
#include "../utils/time_utils.h"
#include "../utils/conversion_utils.h"
#include "../utils/data_logger.h"

void calculateBlockRange(int nrOfBlocks, int nrOfProcs, int pid, int *start, int *end);
int roundUp(int numToRound, int multiple);
