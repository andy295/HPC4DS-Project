#!/bin/bash
#PBS -l select=1:ncpus=7:mem=2gb

# set max execution time
#PBS -l walltime=0:01:00

# set execution queu
#PBS -q short_cpuQ

touch parallel_implementation_MPI/output/reference.txt

if [ -z parallel_implementation_MPI/output/out ]; then 
	rm parallel_implementation_MPI/output/out
fi

qsub -q short_cpuQ -l walltime=0:01:00 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_qsub.sh

while ! test parallel_implementation_MPI/output/out -nt parallel_implementation_MPI/output/reference.txt; do
	sleep 3; 
done

rm parallel_implementation_MPI/output/reference.txt

cat parallel_implementation_MPI/output/out