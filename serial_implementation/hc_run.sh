#!/bin/bash
#PBS -l select=1:ncpus=1:mem=2gb

# set max execution time
#PBS -l walltime=0:01:00

# set execution queu
#PBS -q short_cpuQ


repetitions=10

echo "Running $repetitions repetitions"

for i in $(seq 1 $repetitions); do

	echo "Repetition $i of $repetitions"
	
	touch serial_implementation/output/reference.txt

	if [ -z serial_implementation/output/out ]; then 
		rm serial_implementation/output/out
	fi

	qsub -q short_cpuQ -l walltime=0:05:00 -o serial_implementation/output/out -e serial_implementation/output/err ./serial_implementation/hc_qsub.sh

	while ! test serial_implementation/output/out -nt serial_implementation/output/reference.txt; do
		sleep 3; 
	done

	rm serial_implementation/output/reference.txt

	cat serial_implementation/output/out

	echo -e "Repetition $i of $repetitions done\n "

done