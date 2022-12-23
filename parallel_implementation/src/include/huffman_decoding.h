#pragma once

#include "global_constants.h"

#include "message.h"
#include "huffman_tree.h"
#include "encoding.h"

#include "../utils/file_utils.h"

void calculateBlockRange(int nrOfBlocks, int nrOfProcs, int pid, int *start, int *end);
