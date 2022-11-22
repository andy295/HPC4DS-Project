#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <mpi.h>
#include <unistd.h> // get current directory

#define VERBOSE 3
#define MAX_UNIQUE_LETTERS 100
#define ENCODED_FILE "HPC4DS-Project/parallel_implementation/output/encoded_file"
#define SRC_FILE "HPC4DS-Project/parallel_implementation/text.txt"

// just for testing, we should identify a better value
// max number of elements in the dictionary
#define MAX_MSG_LEN 50
