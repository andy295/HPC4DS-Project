#!/bin/bash
#PBS -l select=1:ncpus=4:mem=2gb

# set max execution time
#PBS -l walltime=0:05:00

# set execution queu
#PBS -q short_cpuQ

module load mpich-3.2

mpicc -std=gnu99  -g -Wall -o parallel_implementation/output/huffman_coding parallel_implementation/src/huffman_coding.c parallel_implementation/src/utils/file_utils.c parallel_implementation/src/char_freq.c