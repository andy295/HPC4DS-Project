#!/bin/bash
#PBS -l select=1:ncpus=4:mem=2gb

# set max execution time
#PBS -l walltime=0:05:00

# set execution queu
#PBS -q short_cpuQ

touch parallel_implementation/output/reference.txt

if [ -z parallel_implementation/output/out ]; then 
	rm parallel_implementation/output/out
fi

qsub -q short_cpuQ -l walltime=0:02:00 -o parallel_implementation/output/out -e parallel_implementation/output/err ./parallel_implementation/hc_qsub.sh

while ! test parallel_implementation/output/out -nt parallel_implementation/output/reference.txt; do
	sleep 3; 
done

rm parallel_implementation/output/reference.txt

cat parallel_implementation/output/out