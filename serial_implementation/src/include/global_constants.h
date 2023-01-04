#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <mpi.h>
#include <omp.h>
#include <unistd.h> // get current directory
#include <math.h>
#include <setjmp.h>
#include <stddef.h>

// enable only if DEBUG_ENABLED AND if PID == master, so only prints one time
#define DEBUG_ENABLED 0
#define DEBUG(PID) (DEBUG_ENABLED && PID == 0)

#define TREE_PRINT_WITH_FREQ 0
#define BITS_IN_BYTE 8 
#define ENDTEXT '\0'
#define CHARS_PER_BLOCK 15
#define MAX_DIGITS 10

#define ENCODED_FILE "HPC4DS-Project/serial_implementation/output/encoded_file"
#define SRC_FILE "HPC4DS-Project/serial_implementation/text.txt"
#define LOG_FILE "HPC4DS-Project/serial_implementation/output/data.csv"
#define DATA_LOGGER_FILE "HPC4DS-Project/serial_implementation/output/data_log.csv"

// only chars accepted
#define IsBit(v, n)      (bool)((v >> n) & 1U)
#define SetBit(v, n)     (v |= 1UL << n)
#define ClrBit(v, n)     (v &= ~(1UL << n))

typedef unsigned char BYTE;

static inline void freeBuffer(void* buffer) {
	if (buffer != NULL)
		free(buffer);
}
