#!/bin/bash
for t in 1 2 4 6 8
do
  for n in 2 3 4 5 10 15 20 30 40 50
  do
     g++   main.cpp -DCSV=true -DNUMT=$t -DNUMCAPITALS=$n  -o project3  -lm  -fopenmp
    ./project3
  done
done