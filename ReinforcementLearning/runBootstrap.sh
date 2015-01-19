#!/bin/sh

# Bootstrap model 2pM38
# Simulate decisions and restimate parameters

## Values set in optimAKEI2pM38f.cpp (f = fast version)
# const int beginGam = 0, nGam = 4;	// begin gam = {0, 1, 2}
# const int nParam = 2;
# const int nRestart = 5, nRound = 5, nStep = 500;						// set nbr of rounds and steps
# const float pTrim = 0.001;
# const double frac = .50;									// set the frac for neighbour
# const double minBound[] = {0, -0.01};					// set min bounds of x, y, z
# const double maxBound[] = {1, +0.01};					// set max bounds of x, y, z

## Run bootstrapping on subject 0

./simulAKEI2pM38 0.100253 -0.00184356 1000 SimFuscAkEi2pM38_subj0
./optimAKEI2pM38f 1000 SimFuscAkEi2pM38_subj0 BootFuscAkEi2pM38_subj0

## Uncomment below to run bootstrapping on more sujbects

#./simulAKEI2pM38 0.00446011 0.00332389 1000 SimFuscAkEi2pM38_subj1
#./optimAKEI2pM38f 1000 SimFuscAkEi2pM38_subj1 BootFuscAkEi2pM38_subj1 

#./simulAKEI2pM38 0.0397649 0.000489738 1000 SimFuscAkEi2pM38_subj2
#./optimAKEI2pM38f 1000 SimFuscAkEi2pM38_subj2 BootFuscAkEi2pM38_subj2 

#./simulAKEI2pM38 3.5931e-05 -0.00164228 1000 SimFuscAkEi2pM38_subj3
#./optimAKEI2pM38f 1000 SimFuscAkEi2pM38_subj3 BootFuscAkEi2pM38_subj3 

#./simulAKEI2pM38 0.00191449 0.00357437 1000 SimFuscAkEi2pM38_subj4
#./optimAKEI2pM38f 1000 SimFuscAkEi2pM38_subj4 BootFuscAkEi2pM38_subj4

#./simulAKEI2pM38 0.0264907 0.00121602 1000 SimFuscAkEi2pM38_subj5
#./optimAKEI2pM38f 1000 SimFuscAkEi2pM38_subj5 BootFuscAkEi2pM38_subj5 

#./simulAKEI2pM38 0.119637 -0.00152146 1000 SimFuscAkEi2pM38_subj6
#./optimAKEI2pM38f 1000 SimFuscAkEi2pM38_subj6 BootFuscAkEi2pM38_subj6 

#./simulAKEI2pM38 0.00431408 0.00275913 1000 SimFuscAkEi2pM38_subj7
#./optimAKEI2pM38f 1000 SimFuscAkEi2pM38_subj7 BootFuscAkEi2pM38_subj7 

#./simulAKEI2pM38 0.0324163 0.00254687 1000 SimFuscAkEi2pM38_subj8
#./optimAKEI2pM38f 1000 SimFuscAkEi2pM38_subj8 BootFuscAkEi2pM38_subj8 

#./simulAKEI2pM38 0.0280786 0.000699943 1000 SimFuscAkEi2pM38_subj9
#./optimAKEI2pM38f 1000 SimFuscAkEi2pM38_subj9 BootFuscAkEi2pM38_subj9 



