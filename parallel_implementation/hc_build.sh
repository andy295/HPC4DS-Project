#!/bin/bash
#PBS -l select=1:ncpus=4:mem=2gb

# set max execution time
#PBS -l walltime=0:01:00

# set execution queu
#PBS -q short_cpuQ

module load mpich-3.2

# get all c file paths in src folder
c_files=$(find ./parallel_implementation/src -name "*.c")

# parallel_implementation/src/huffman_coding.c parallel_implementation/src/utils/file_utils.c parallel_implementation/src/char_freq.c parallel_implementation/src/message.c parallel_implementation/src/huffman_tree.c

mpicc -std=gnu99  -g -Wall -o parallel_implementation/output/huffman_coding $c_files