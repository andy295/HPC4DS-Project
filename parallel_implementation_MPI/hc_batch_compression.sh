
#!/bin/bash
repeat_executions=1

start_num_of_blocks=300
end_num_of_blocks=600
step=10

echo -e "Running ${repeat_executions} executions with blocks from ${start_num_of_blocks} to ${end_num_of_blocks} skipping ${step} each step...\n"

for ((j=1;j<=repeat_executions;j++))
do 
    echo -e "--- Execution ${j} of ${repeat_executions}...\n"
    for ((i=start_num_of_blocks;i<=end_num_of_blocks;i+=step)) 
    do
        echo -e "--- Running with $i blocks...\n"

        touch parallel_implementation_MPI/output/reference.txt

        if [ -z parallel_implementation_MPI/output/out ]; then 
            rm parallel_implementation_MPI/output/out
        fi

        qsub -q short_cpuQ -l select=4:ncpus=4 -v "num_of_blocks=$i" -o parallel_implementation_MPI/output/out -e parallel_implementation_MPI/output/err ./parallel_implementation_MPI/hc_batch_compression_qsub.sh

        while ! test parallel_implementation_MPI/output/out -nt parallel_implementation_MPI/output/reference.txt; do
            sleep 3; 
        done

        rm parallel_implementation_MPI/output/reference.txt

        echo -e "Output of execution:"
        cat parallel_implementation_MPI/output/out
        
        echo -e "Done with $i blocks\n"
    done
done