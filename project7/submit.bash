#!/bin/bash
# Define variables for SLURM job and MPI configuration

NUM_NODES=8
TOTAL_MPI_TASKS=8
MPI_PROCESSES_PER_NODE=4

#SBATCH -J proj07Fourier
#SBATCH -A cs475-575
#SBATCH -p classmpitest
#SBATCH -N $NUM_NODES           # Number of nodes requested
#SBATCH -n $TOTAL_MPI_TASKS     # Total number of MPI tasks/processes
#SBATCH -o proj07.out           # Standard output file
#SBATCH -e proj07.err           # Standard error file
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=nguyedin@oregonstate.edu

module load openmpi

# Compile your project
mpic++ proj07.cpp -o proj07 -lm

# Run the program with specified MPI processes per node
mpiexec -mca btl self,tcp -np $TOTAL_MPI_TASKS ./proj07
