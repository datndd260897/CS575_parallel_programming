#!/bin/bash

# Make sure the OpenCL library path is correct for your system
OPENCL_LIB=/usr/local/apps/cuda/10.1/lib64/libOpenCL.so.1

for DATASIZE in 4096 16384 65536 262144 1048576 4194304
do
    for LOCALSIZE in 8 32 64 128 256
    do
        echo "Running with DATASIZE=$DATASIZE and LOCALSIZE=$LOCALSIZE"

        g++ -DDATASIZE=$DATASIZE -DLOCALSIZE=$LOCALSIZE -o proj06 proj06.cpp $OPENCL_LIB -lm -fopenmp
        if [ $? -ne 0 ]; then
            echo "Compilation failed for DATASIZE=$DATASIZE LOCALSIZE=$LOCALSIZE"
            exit 1
        fi

        ./proj06
    done
done
