#!/bin/bash
#PBS -l select=1:ncpus=4:mem=2gb

# set max execution time
#PBS -l walltime=0:05:00

# set execution queu
#PBS -q short_cpuQ

module load mpich-3.2

mpicc -g -Wall -o Project/huffman_coding Project/src/huffman_coding.c Project/src/utils/file_utils.c Project/src/char_freq.c

mpirun.actual -n 4 ./Project/huffman_coding
