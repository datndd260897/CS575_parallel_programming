#!/bin/bash

# Number of threads:
for num_thread in 1 2 4 6 8
do
    for num_trial in 100 1000 10000 50000 100000
    do
            /usr/bin/g++ -DCSV=true -DNUMT=$num_thread -DNUMTRIALS=$num_trial main.cpp -o project1 -lm -fopenmp  # Full path to g++-13
            ./project1
    done
done
