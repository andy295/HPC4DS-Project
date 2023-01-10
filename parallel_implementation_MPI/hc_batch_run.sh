
#!/bin/bash
repeat_executions=5

start_num_of_processes=1
end_num_of_processes=16
step_processes=1

start_num_of_threads=1
end_num_of_threads=16
step_threads=1

echo -e "Running ${repeat_executions} executions with processes from ${start_num_of_processes} to ${end_num_of_processes} skipping ${step} each step...\n"

for ((j=1;j<=repeat_executions;j++))
do 
    echo -e "--- Execution ${j} of ${repeat_executions}...\n"
    for ((i=start_num_of_processes;i<=end_num_of_processes;i+=step_processes)) 
    do
        echo -e "--- Running with $i nodes...\n"

        for ((t=start_num_of_threads;t<=end_num_of_threads;t+=step_threads)) 
        do
            echo -e "--- Running with $t threads..."

            touch parallel_implementation_MPI/output/reference.txt

            if [ -z parallel_implementation_MPI/output/out ]; then 
                rm parallel_implementation_MPI/output/out
            fi

            qsub -q short_cpuQ -l select=$i:ncpus=$i -v "num_of_threads=$t" -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_qsub.sh

            while ! test parallel_implementation_MPI/output/out -nt parallel_implementation_MPI/output/reference.txt; do
                sleep 3; 
            done

            rm parallel_implementation_MPI/output/reference.txt

            echo -e "Output of execution:"
            cat parallel_implementation_MPI/output/out

        done
        
        echo -e "Done with $i nodes\n"
    done
done