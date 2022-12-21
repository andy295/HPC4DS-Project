#pragma once

#include "global_constants.h"

#include "char_freq.h"
#include "message.h"
#include "huffman_tree.h"
#include "encoding.h"

#include "../utils/file_utils.h"
#include "../utils/time_utils.h"
// #include "../utils/print_utils.c"

BYTE* prepareForReceive(MPI_Status *status, int *bufferSize, int pid, int tag);
