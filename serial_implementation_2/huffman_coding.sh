# get all c file paths in src folder
c_files=$(find ./src -name "*.c")

gcc -std=gnu99 -g -Wall -o huffman_coding $c_files