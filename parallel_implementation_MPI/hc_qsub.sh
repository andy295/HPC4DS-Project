
#!/bin/bash
#PBS -l select=4:ncpus=7:mem=2gb -l place=scatter:excl
#PBS -l walltime=0:01:00
#PBS -q short_cpuQ

module load mpich-3.2
mpirun.actual -n 1 ./HPC4DS-Project/parallel_implementation_MPI/output/huffman_coding 4
mpirun.actual -n 2 ./HPC4DS-Project/parallel_implementation_MPI/output/huffman_coding 4
mpirun.actual -n 4 ./HPC4DS-Project/parallel_implementation_MPI/output/huffman_coding 4
mpirun.actual -n 8 ./HPC4DS-Project/parallel_implementation_MPI/output/huffman_coding 4
mpirun.actual -n 16 ./HPC4DS-Project/parallel_implementation_MPI/output/huffman_coding 4