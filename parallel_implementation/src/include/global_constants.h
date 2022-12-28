#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <mpi.h>
#include <unistd.h> // get current directory
#include <math.h>
#include <setjmp.h>

// enable only if 1 AND if PID == master, so only prints one time
#define DEBUG(PID) (0 && PID == 0) 

#define TREE_PRINT_WITH_FREQ 0
#define BITS_IN_BYTE 8 
#define ENDTEXT '\0'
#define CHARS_PER_BLOCK 23 // just for test

#define ENCODED_FILE "HPC4DS-Project/parallel_implementation/output/encoded_file"
#define SRC_FILE "HPC4DS-Project/parallel_implementation/text.txt"
#define TIME_LOG_FILE "HPC4DS-Project/parallel_implementation/output/data.csv"
#define DATA_LOGGER_FILE "HPC4DS-Project/parallel_implementation/output/data_log.csv"

// they accept only char because it is 8 bits long
#define IsBit(v, n)      (bool)((v >> n) & 1U)
#define SetBit(v, n)     (v |= 1UL << n)
#define ClrBit(v, n)     (v &= ~(1UL << n))

typedef unsigned char BYTE;

static inline void freeBuffer(void* buffer) {
	if (buffer != NULL)
		free(buffer);
}
