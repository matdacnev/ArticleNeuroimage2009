#!/bin/sh

## Model 2pM38 (1 step, k, l)

## Values in optimAKEI2pM38.cpp used for the Neuroimage article:
# const int beginGam = 0, nGam = 4;	// begin gam = {0, 1, 2}
# const int nParam = 2;
# const int nRestart = 30, nRound = 10, nStep = 2000;				// set nbr of rounds and steps
# const float pTrim = 0.001;
# const double frac = .50;									// set the frac for neighbour
# const double minBound[] = {0, -0.01};					// set min bounds of x, y, z
# const double maxBound[] = {1, +0.01};					// set max bounds of x, y, z

# Run optimization on 10 subjects (8 are selected for the Neuroimage article):
./optimAKEI2pM38 10 RecFuscIgtAkEi SolFuscAkEi2pM38

