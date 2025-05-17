#!/bin/bash
SIZE=1024
LIMIT=9000000
echo 'SIZE, NUMT, Mul Pnon-sse, Mul Psse, Mul S, MulOMP Psse, MulOMP S, MulSum Pnon-sse, MulSum Psse, MulSum S, MulSumOMP Psse, MulSumOMP S'
for t in 1 2 4 6
do
    while [ $SIZE -le $LIMIT ]
    do
        g++ main.cpp -DNUMT=$t -DARRAYSIZE=$SIZE -o project4 -lm -fopenmp
        ./project4
        SIZE=$((SIZE * 2))
    done
    SIZE=1024
done
