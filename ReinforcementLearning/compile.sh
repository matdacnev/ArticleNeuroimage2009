#!/bin/sh

# Compile cpp source files
# The Gnu Scientific library is needed (gsl)

## simulation

g++ -c simulAKEI2pM38.cpp
g++ -static simulAKEI2pM38.o -o simulAKEI2pM38 -lgsl -lgslcblas -lm

## information

g++ -c infoAKEI2pM38.cpp
g++ -static infoAKEI2pM38.o -o infoAKEI2pM38 -lgsl -lgslcblas -lm

## optimization

g++ -O3 -c optimAKEI2pM38.cpp
g++ -static -O3 optimAKEI2pM38.o -o optimAKEI2pM38 -lgsl -lgslcblas -lm

## fast optimization (run after simulation to bootstrap)

g++ -O3 -c optimAKEI2pM38f.cpp
g++ -static -O3 optimAKEI2pM38f.o -o optimAKEI2pM38f -lgsl -lgslcblas -lm


