# get all c file paths in src folder
c_files=$(find ./serial_implementation/src -name "*.c" ! -name "huffman_decoding.*")


if [ $1 ]; then
    if [ $1 == "dec" ] || [ $1 == "d" ]; then
        c_files=$(find ./serial_implementation/src -name "*.c" ! -name "huffman_coding.*")
    fi
fi

module load mpich-3.2

mpicc -std=gnu99 -g -Wall -fopenmp -o serial_implementation/output/huffman_coding $c_files