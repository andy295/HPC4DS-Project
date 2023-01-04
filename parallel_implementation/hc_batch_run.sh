
qsub -q short_cpuQ -l select=2:ncpus=2 -o parallel_implementation/output/out -e parallel_implementation/output/err ./parallel_implementation/hc_batch_qsub.sh
qsub -q short_cpuQ -l select=4:ncpus=4 -o parallel_implementation/output/out -e parallel_implementation/output/err ./parallel_implementation/hc_batch_qsub.sh
qsub -q short_cpuQ -l select=8:ncpus=8 -o parallel_implementation/output/out -e parallel_implementation/output/err ./parallel_implementation/hc_batch_qsub.sh
qsub -q short_cpuQ -l select=16:ncpus=16 -o parallel_implementation/output/out -e parallel_implementation/output/err ./parallel_implementation/hc_batch_qsub.sh
