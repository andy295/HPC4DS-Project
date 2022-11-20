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
