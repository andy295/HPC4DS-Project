
# qsub -q short_cpuQ -l select=2:ncpus=2 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=3:ncpus=3 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=4:ncpus=4 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=5:ncpus=5 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=6:ncpus=6 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=7:ncpus=7 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=8:ncpus=8 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=9:ncpus=9 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=10:ncpus=10 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=11:ncpus=11 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=12:ncpus=12 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=13:ncpus=13 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=14:ncpus=14 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=15:ncpus=15 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh
# qsub -q short_cpuQ -l select=16:ncpus=16 -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh


repeat_executions=1

start_num_of_processes=2
end_num_of_processes=4
step=1

echo -e "Running ${repeat_executions} executions with processes from ${start_num_of_processes} to ${end_num_of_processes} skipping ${step} each step...\n"

for ((j=1;j<=repeat_executions;j++))
do 
    echo -e "--- Execution ${j} of ${repeat_executions}...\n"
    for ((i=start_num_of_processes;i<=end_num_of_processes;i+=step)) 
    do
        echo -e "Running with $i nodes..."
        touch parallel_implementation_MPI/output/reference.txt

        if [ -z parallel_implementation_MPI/output/out ]; then 
            rm parallel_implementation_MPI/output/out
        fi

        qsub -q short_cpuQ -l select=$i:ncpus=$i -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh

        while ! test parallel_implementation_MPI/output/out -nt parallel_implementation_MPI/output/reference.txt; do
            sleep 3; 
        done

        rm parallel_implementation_MPI/output/reference.txt

        echo -e "Output of execution:"
        cat parallel_implementation_MPI/output/out
        
        echo -e "Done with $i nodes\n"
    done
done