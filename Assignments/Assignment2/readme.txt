This program reads an input file and conducts the traveling salesman algorithm on the data to determine the fastest route between nodes. 

Running sequential 
Compile
clang tsp_sequential.c -o tsp_sequential.out

Run with log file
./tsp_sequential.out input_file.txt log

Run without log file
./tsp_sequential.out input_file.txt

Speedup time
time ./tsp_sequential.out

Running parallel (must be run on cluster) 
Compile
mpicc -g -Wall -o tsp_parallel tsp_parallel.c

Run with log file
mpiexec -np ** ./tsp_parallel input_file log

Run without log file
mpiexec -np ** ./tsp_parallel input_file

** indicates number of processes (change to number)

Speedup time
time ./tsp_parallel