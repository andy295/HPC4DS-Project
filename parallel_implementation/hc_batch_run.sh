

qsub -q short_cpuQ -l walltime=0:02:00 -o parallel_implementation/output/out -e parallel_implementation/output/err ./parallel_implementation/hc_qsub.sh