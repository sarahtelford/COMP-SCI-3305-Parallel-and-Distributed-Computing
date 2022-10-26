#!/bin/bash
# Configure the resources required
#SBATCH -n 1                # number of cores
#SBATCH --time=00:05:00     # time allocation, which has the format DD:HH:MM
#SBATCH --gres=gpu:1        # generic resource required (1 GPU)

# time ./parallel input.txt
# time ./parallel 10.txt
time ./parallel 100.txt
# time ./parallel 1000.txt

