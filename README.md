# HPC4DS-Project - Huffman Coding

High performance computing for data science project.

Andrea Cristiano, matr. 229370

Gabriele Padovani, matr. 229207

## Project Structure

The project structure is split into three main sections: 
- Serial implementation (found in the folder `serial_implementation`): which consists in a basic version of huffman encoding without using any multi-process or threading technique; 
- Parallel basic implementation (found in the folder `parallel_implementation`): which makes use of basic MPI_Send and MPI_Recv calls, as well as openMP directives.
- Parallel mpi implementation (found in the folder `parallel_implementation_MPI`): which also uses advanced MPI techniques, such as MPI_Gather and MPI_Bcast.

## Running

First of all download and move into the repository: 
```
git clone https://github.com/lelepado01/HPC4DS-Project.git 
cd HPC4DS-Project
```

To run each of the sections, two scripts are present:
 - `hc_build.sh`: which can be used to compile the relative script, the default compilation target is the **encoding** section, it is possible however to compile and run the decoding section by passing `dec` as argument; 
 - `hc_run.sh`: which can be used to run the project.
 
Here are some examples on how to build and run: 
 - Build and run the serial encoding implementation of the project: 
 ```
 ./serial_implementation/hc_build.sh
 ./serial_implementation/hc_run.sh
 ```
 - Build and run the serial decoding implementation of the project:  
 ```
 ./serial_implementation/hc_build.sh dec
 ./serial_implementation/hc_run.sh
 ```

The run script can be used to run the version which was last built. The binaries can be found in the output directory. For the previous example, the executable path would be: 
```
./serial_implementation/output/huffman_coding
```

The same folder is also used to store the output and error files, as well as any file used by the datalogger and time classes: 
```
./serial_implementation/output/out
./serial_implementation/output/err
./serial_implementation/output/data.csv
```

## Resources

- https://iq.opengenus.org/huffman-encoding/
- https://stackoverflow.com/questions/31796662/is-there-any-way-to-parallelize-huffman-encoding-implementation-on-hardware
- http://jmyers-gnu.blogspot.com/2007/02/parallelizing-huffman-coding-in-that.html

- https://www.youtube.com/watch?v=B3y0RsVCyrw
