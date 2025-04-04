#!/bin/bash

# Number of threads:
for t in 1 4
do
        /usr/bin/g++ -DNUMT=$t main.cpp -o project0 -lm -fopenmp  # Full path to g++-13
        ./project0
done
