#!/bin/sh

# Model 2pM38

## Values in infoAKEI2pM38.cpp used for the Neuroimage article:
# const int beginGam = 0, nGam = 4;	// begin gam = {0, 1, 2}
# const int nParam = 2;
# int Reward[240][4], Punish[240][4];


# Get information (after optmization has been completed):
# Parameter values are copied from SolFuscAkEi2pM38Min
./infoAKEI2pM38 0.100253 -0.00184356 0 RecFuscIgtAkEi InfoFuscAkEi2pM38_subj0.txt
./infoAKEI2pM38 0.00446011 0.00332389 1 RecFuscIgtAkEi InfoFuscAkEi2pM38_subj1.txt
./infoAKEI2pM38 0.0397649 0.000489738 2 RecFuscIgtAkEi InfoFuscAkEi2pM38_subj2.txt
./infoAKEI2pM38 3.5931e-05 -0.00164228 3 RecFuscIgtAkEi InfoFuscAkEi2pM38_subj3.txt
./infoAKEI2pM38 0.00191449 0.00357437 4 RecFuscIgtAkEi InfoFuscAkEi2pM38_subj4.txt
./infoAKEI2pM38 0.0264907 0.00121602 5 RecFuscIgtAkEi InfoFuscAkEi2pM38_subj5.txt
./infoAKEI2pM38 0.119637 -0.00152146 6 RecFuscIgtAkEi InfoFuscAkEi2pM38_subj6.txt
./infoAKEI2pM38 0.00431408 0.00275913 7 RecFuscIgtAkEi InfoFuscAkEi2pM38_subj7.txt
./infoAKEI2pM38 0.0324163 0.00254687 8 RecFuscIgtAkEi InfoFuscAkEi2pM38_subj8.txt
./infoAKEI2pM38 0.0280786 0.000699943 9 RecFuscIgtAkEi InfoFuscAkEi2pM38_subj9.txt

