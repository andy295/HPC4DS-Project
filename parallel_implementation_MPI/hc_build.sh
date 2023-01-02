#!/bin/bash
#PBS -l select=1:ncpus=7:mem=2gb

# set max execution time
#PBS -l walltime=0:01:00

# set execution queu
#PBS -q short_cpuQ

module load mpich-3.2

# get all c file paths in src folder
# default is encoding build
c_files=$(find ./parallel_implementation_MPI/src -name "*.c"  ! -name "huffman_decoding.*")

if [ $1 ]; then
    if [ $1 == "dec" ] || [ $1 == "d" ]; then
        c_files=$(find ./parallel_implementation_MPI/src -name "*.c" ! -name "huffman_coding.*")
    fi
fi

mpicc -std=gnu99 -g -Wall -fopenmp -o parallel_implementation_MPI/output/huffman_coding $c_files