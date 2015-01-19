// Mathieu d'Acremont
// Swiss Centre for Affective  Sciences
// University of  Geneva
// Switzerland

// Get info on decisions simulated by simul
// 2 param, beta and alpha

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
void learnPE(double *param, int *y, double **Information);
int getNbrSubj(char * nameFile);
void loadResponse(char *nameFile, int *response);

// global variable
const int beginGam = 0, nGam = 4;	// begin gam = {0, 1, 2}
const int nParam = 2;
int Reward[240][4], Punish[240][4];

int main(int argc, char *argv[]) {
	// pogram arg1 arg2 arg3 arg4
	//	arg1 = beta
	//	arg2 = selSubj, the first subj is 0
	//	arg3 = name of file to load
	//	arg4 = name of file to save
	
	if (argc != nParam+4) {
		cerr << "ERROR. The number of param is wrong. Exit" << endl; exit(1);
	}
	
	double param[nParam];
	for (int i = 0; i < nParam; i++) {
		param[i] = atof(argv[i+1]);			// set param
	}
	
	const int selSubj = atoi(argv[nParam+1]);

	char nameFileResponse[60+1];
	strcpy(nameFileResponse, "Data/");
	strcat(nameFileResponse, argv[nParam+2]);		// Data/Filename	
	
	char nameFileSave[60+1];
	strcpy(nameFileSave, "Result/");
	strcat(nameFileSave, argv[nParam+3]);		// Result/Filename	
	
	loadReinforcement();

	const int nR = nGam*100;
	const int nSubj = getNbrSubj(nameFileResponse);
	if (selSubj >= nSubj) {
		cerr << "ERROR. The selected subject is too high. Exit" << endl; exit(1);
	}
	
	// int response[nSubj*nR];						// static
	int response[nSubj*nR];					// static
	loadResponse(nameFileResponse, response);		// response <- loadResponse()
	// printTab("response=", response, nR);				// print first 300 decisions
	
	double **Information = new double* [nR];
	for (int i = 0; i < nR; i++) {Information[i] = new double[29];}

	
	// simulation
	learnPE(param, &response[selSubj*nR], Information); 			// response <- simulate decisions		
	// printTab("y", &response[selSubj*nR], nR);

	saveArray(nameFileSave, Information, nR, 29);
	
	return 0;
}


void loadReinforcement() {	// load Reward and Punishment
	int deck1, deck2, deck3, deck4;
	ifstream RewardFileIn("Data/RewardAkEi.txt"), PunishFileIn("Data/PunishAkEi.txt");

	if (!RewardFileIn ) {
		cerr << "ERROR. The file RewardAkEi.txt could not be opened. Exit" << endl; exit(1);
	}

	if (!PunishFileIn) {
		cerr << "ERROR. The file PunishAkEi.txt could not be opened. Exit" << endl; exit(1);
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

int getNbrSubj(char *nameFile) {
	int choice, nSubj;
	ifstream FileIn(nameFile);

	if (!FileIn) {
		cerr << "The file of responses could not be opened. Exit" << endl; exit(1);
	}
	
	int i = 0;
	while (FileIn >> choice) {
		i++;
	}
	FileIn.close();
	nSubj = i/(nGam*100);
	return(nSubj);
}



void loadResponse(char *nameFile, int *response) {
	int choice;
	ifstream FileIn(nameFile);

	int i = 0;
	while (FileIn >> choice) {
		response[i] = choice;			// all data on 1 line
		i++;
	}
	FileIn.close();
}



void saveArray(char *nameFileSol, double **tab, int nRow, int nCol) {
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

void learnPE(double *param, int *y, double **Information) {	// work on y
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
			deck = y[k]; 				// get the deck
			card = cardInDeck[deck]; 		// position in the deck
			cardInDeck[deck]++;			// one card further in that deck
			
			rewVal = Reward[card][deck] + Punish[card][deck];
			
			pE = rewVal - meanVal[deck];
			if(card == firstCard & param[0] > 0) {varVal[deck] = pE*pE;}
			pEScale = pE / sqrt(varVal[deck]);			// / sqrt(varVal[deck]);
			pEVar = (pE*pE) - varVal[deck];
			// update
			meanVal[deck] += param[0]*pEScale;		// second update
			varVal[deck] += param[0]*pEVar;
						
			if (card == lastCard) {meanVal[deck] = GSL_NEGINF;}
				
			// cout << "----------------" << endl;
			// cout << "k= " << k << " deck=" << deck << " card=" << card <<  endl;
			// cout << "rewVal= " << rewVal << " rewVal2= " << rewVal2 << endl;

			Information[k][0] = deck;				// save the selected deck
			Information[k][1] = rewVal;				// save the varVal
			Information[k][2] = p[deck];				// save the prob
			Information[k][3] = meanVal[deck];			// save the meanVal
			Information[k][4] = varVal[deck];			// save the varVal
			for (int i = 0; i < 4; i++) {				// WARNING, this is out of date
				Information[k][i+5] = p[i];			// save the prob
				Information[k][i+9] = meanVal[i];		// save the meanVal
				Information[k][i+13] = varVal[i];		// save the varVal
				if (deck == i) {
					Information[k][i+17] = pE;			// save pE
					Information[k][i+21] = pEVar;			// save pEV
					Information[k][i+25] = pEScale;		// save pEScale
				}
				else {
					Information[k][i+17] = GSL_NAN;			// save pE
					Information[k][i+21] = GSL_NAN;			// save pEV
					Information[k][i+25] = GSL_NAN;		// save pEScale
				}
			}

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



