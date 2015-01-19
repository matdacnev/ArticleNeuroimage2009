// Mathieu d'Acremont
// Swiss Centre for Affective  Sciences
// University of  Geneva
// Switzerland

// Simulated decisions for a set of parameters

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <algorithm>

#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace std;

// the header
void printTab(const char *name, int *tab, int size);
void printTab(const char *name, double *tab, int size);
void loadReinforcement();
void saveArray(char *nameFileSol, double **tab, int nRow, int nCol);
void saveArray(char *nameFileSol, int *tab, int nRow, int nCol);
void learnPE(double *param, int *y);
inline int selectDeck(double *p);


// global variable
const int beginGam = 0, nGam = 4;	// begin gam = {0, 1, 2}
const int nParam = 2;
int Reward[240][4], Punish[240][4];
gsl_rng *r = gsl_rng_alloc(gsl_rng_taus); 	// init random generator

int main(int argc, char *argv[]) {
	// pogram arg1 arg2 arg3
	//	arg1 = beta
	//	arg2 = omega
	//	arg3 = nbr subj (nbr of simulations)
	//	arg4 = name of file to save response

	// cout << "argc=" << argc << endl;
	
	if (argc != nParam+3) {
		cerr << "ERROR. The number of param is wrong. Exit" << endl; exit(1);
	}
	
	double param[nParam];
	for (int i = 0; i < nParam; i++) {
		param[i] = atof(argv[i+1]);			// set param
	}

	const int nR = nGam*100;
	const int nSubj = atoi(argv[nParam+1]);			// arg2, nbr subj
	
	char nameFileSave[60+1];
	strcpy(nameFileSave, "Result/");
	strcat(nameFileSave, argv[nParam+2]);				// arg3, Data/Filename	
	
	int response[nSubj*nR];					// all responses in one line

	loadReinforcement();
	gsl_rng_set(r, time(0)); 				// random seed
	
	// simulation
	for (int subj = 0; subj < nSubj; subj++) {
		learnPE(param, &response[subj*nR]); 		// response <- simulate decisions		
		// printTab("y", &response[subj*nR], nR);
	}
	
	saveArray(nameFileSave, response, nSubj, nR); 		// WARNING, one subj per line
	
	return 0;
}


void loadReinforcement() {	// load Reward and Punishment
	int deck1, deck2, deck3, deck4;
	ifstream RewardFileIn("Data/RewardAkEi.txt"), PunishFileIn("Data/PunishAkEi.txt");

	if (!RewardFileIn ) {
		cerr << "The file RewardAkEi.txt could not be opened. Exit" << endl; exit(1);
	}

	if (!PunishFileIn) {
		cerr << "The file PunishAkEi.txt could not be opened. Exit" << endl; exit(1);
	}

	int i = 0;
	while (RewardFileIn >> deck1 >> deck2 >> deck3 >> deck4) {
		Reward[i][0] = deck1; Reward[i][1] = deck2; Reward[i][2] = deck3; Reward[i][3] = deck4;
		i++;
	}

	i = 0;
	while (PunishFileIn >> deck1 >> deck2 >> deck3 >> deck4) {
		Punish[i][0] = deck1; Punish[i][1] = deck2; Punish[i][2] = deck3; Punish[i][3] = deck4;
		i++;
	}

	RewardFileIn.close(); PunishFileIn.close();
	
	// cout << "Reinforcement loaded." << endl;
}

void saveArray(char *nameFileSol, double **tab, int nRow, int nCol) { // WARNING, unchecked
	ofstream FileOut(nameFileSol);
	
	for (int i = 0; i < nRow; i++) {
		for (int j = 0; j < nCol-1; j++) {
			FileOut << tab[i][j] << " ";
		}
		FileOut << tab[i][nCol-1];		
		FileOut << endl;
	}
	FileOut.close();
}

void saveArray(char *nameFileSol, int *tab, int nRow, int nCol) {
	ofstream FileOut(nameFileSol);
	
	int pos = 0;
	for (int i = 0; i < nRow; i++) {
		for (int j = 0; j < nCol-1; j++) {
			FileOut << tab[pos] << " ";
			pos++;
		}
		FileOut << tab[pos];		
		FileOut << endl;
		pos++;
	}
	FileOut.close();
}


void learnPE(double *param, int *y) {	// work on y
	double p[4], meanVal[4], varVal[4], x[4];
	double rewVal, pE, pEVar, pEScale, sumX;
	int first, last, firstCard, lastCard;
	int cardInDeck[4];
	int deck, card; // deck selected, position in deck


	for (int gam = 0; gam < nGam; gam++) {	// go through games
		first = gam*100; last=first+99;
		firstCard = (gam+beginGam)*60; lastCard = firstCard+59;

		for (int i = 0; i < 4; i++) {
			p[i] = 0.25;	// Initialize probability
			meanVal[i] = 0;	// Initialize action value
			varVal[i] = 1;	// Initialize action value
			cardInDeck[i] = firstCard;	// first card of the game
		}

		for (int k = first; k <= last; k++) {
			deck = selectDeck(p); 			// select the deck according to p
			y[k] = deck;				// save selected deck
			card = cardInDeck[deck]; 		// position in the deck
			cardInDeck[deck]++;				// one card further in that deck

			rewVal = Reward[card][deck] + Punish[card][deck];
			
			pE = rewVal - meanVal[deck];
			if(card == firstCard & param[0] > 0) {varVal[deck] = pE*pE;}
			pEScale = pE / sqrt(varVal[deck]);			// / sqrt(varVal[deck]);
			pEVar = (pE*pE) - varVal[deck];
			// update
			meanVal[deck] += param[0]*pEScale;			// second update
			varVal[deck] += param[0]*pEVar;
			
			if (card == lastCard) {meanVal[deck] = GSL_NEGINF;}

			sumX = 0;
			for (int i = 0; i < 4; i++) {
				x[i] = exp(meanVal[i] + param[1] * sqrt(varVal[i]));
				sumX += x[i];
			}
			for (int i = 0; i < 4; i++) {
				p[i] = x[i] / sumX;
			}
		}
	}
}


inline int selectDeck(double *p) {		// select a deck according to p
	int deck;
	double cumulF[4];
	double sumP = 0;
	for (int i = 0; i <= 3; i++) {
		sumP += p[i];
		cumulF[i] = sumP;
	}
	double u = gsl_rng_uniform(r);	// get unifrom [0, 1[
	// cout << "u=" << u << endl;
	// printTab("cumulF", cumulF, 4);
	for (int i = 0; i <= 3; i++) {
		if (u < cumulF[i]) {
			deck = i;
			break;
		}
	}
	return deck;
}


void printTab(const char *name, int *tab, int size) {
	cout.setf(ios::left, ios::adjustfield);
	cout << setw(4) << name << "=";
	cout.setf(ios::right, ios::adjustfield);
	for (int i = 0; i < size; i++) {
		cout << setw(3) << tab[i];
	}
	cout << endl;
}

void printTab(const char *name, double *tab, int size) {
	cout.setf(ios::left, ios::adjustfield);
	cout << setw(8) << name << " =";
	cout.setf(ios::right, ios::adjustfield);
	for (int i = 0; i < size-1; i++) {
		cout << setw(10) << setprecision(3) << tab[i];
	}
	cout << setw(12) << setprecision(8) << tab[size-1];
	cout << endl;
}



