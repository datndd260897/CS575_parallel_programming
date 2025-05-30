#!/bin/bash

#SBATCH -J proj07FourierTests
#SBATCH -A cs475-575
#SBATCH -p classmpitest
#SBATCH -o proj07_%j.out
#SBATCH -e proj07_%j.err
#SBATCH --mail-type=END,FAIL
#SBATCH --mail-user=nguyedin@oregonstate.edu

module load openmpi

# Elements array (number of elements)
ELEMENTS_LIST=(1048576 524288)

# Nodes to test
NODES_LIST=(1 2 4 6 8)

# Loop over number of elements and nodes
for NUMELEMENTS in "${ELEMENTS_LIST[@]}"
do
  echo "Testing NUMELEMENTS=$NUMELEMENTS"
  
  # Here we modify the source code to set NUMELEMENTS and recompile
  # We'll do a simple sed to replace the #define NUMELEMENTS line
  sed -i "s/#define NUMELEMENTS.*$/#define NUMELEMENTS\t($NUMELEMENTS)/" proj07.cpp
  
  # Compile
  mpic++ proj07.cpp -o proj07 -lm
  if [ $? -ne 0 ]; then
    echo "Compilation failed for NUMELEMENTS=$NUMELEMENTS!"
    exit 1
  fi
  
  for NNODES in "${NODES_LIST[@]}"
  do
    echo "Running with $NNODES nodes (processes)"
    
    # Run MPI with NNODES processes
    mpiexec -mca btl self,tcp -np $NNODES ./proj07
    
    echo "----------------------------------------"
  done
  
done

echo "All tests done."
