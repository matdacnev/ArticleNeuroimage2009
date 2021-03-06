// Mathieu d'Acremont
// Swiss Centre for Affective  Sciences
// University of  Geneva
// Switzerland

// Optimization
// 2 param model


#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring> // cstring replace string
#include <algorithm>

#include <gsl/gsl_math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

using namespace std;

// the header
void printTab(const char *name, int *tab, int size); // added const before char
void printTab(const char *name, double *tab, int size); // added const before char
void loadReinforcement();
int getNbrSubj(char *nameFile);
void loadResponse(char *nameFile, int *response);
void saveArray(char *nameFileSol, double **tab, int nRow, int nCol);
int * selSubj(int *record, int subj);
inline double logLHPE3D(double *param, int *y);
inline bool isPositive(double val);
inline bool isNotFinite(double val);
void theSequence(double *treshold, int *y);
void tah3D(double **Solution, int *y, double *treshold, int rowInSol);
inline void neighbourNormal3D(double *point);


// global variable
const int beginGam = 0, nGam = 4;	                    // begin gam = {0, 1, 2}
const int nParam = 2;
int Reward[240][4], Punish[240][4];

gsl_rng *r = gsl_rng_alloc(gsl_rng_taus); 	// init random generator

const int nRestart = 30, nRound = 10, nStep = 2000;					// set nbr of rounds and steps
const float pTrim = 0.001;
const double frac = .50;									// set the frac for neighbour

const double minBound[] = {0, -0.01};					// set min bounds of x, y, z
const double maxBound[] = {1, +0.01};					// set max bounds of x, y, z
const double dBound[] = {maxBound[0] - minBound[0], maxBound[1] - minBound[1]};


int main(int argc, char *argv[]) {
	// program arg1 arg2 arg3
	//	arg1 = max nbr of subj to analyse
	//	arg2 = file of responses to load
	//	arg3 = file to save sol
	
	if (argc != 4) {
		cerr << "ERROR. The number of param is wrong. Exit" << endl; exit(1);
	}
	
	int maxSubj = atoi(argv[1]);

	char nameFileResponse[60+1], nameFileSol[60+1], nameFileMinSol[60+1];
	strcpy(nameFileResponse, "Data/");
	strcat(nameFileResponse, argv[2]);		// Data/Fileresponse
	
	strcpy(nameFileSol, "Result/");
	strcat(nameFileSol, argv[3]);			// Result/Filesave

	strcpy(nameFileMinSol, nameFileSol);
	strcat(nameFileMinSol, "Min");			// Result/FilesaveMin
	
	// cout << "*****************************************" << endl;
	// cout << "load=" << nameFileResponse << ", save=" << nameFileSol << endl;
	

	long time0, time1;
	double ll, minLl;
	int minRowInSol = -1;
	
	loadReinforcement();
	gsl_rng_set(r, time(0)); 					// random seed
	
	// load data (simulated or real)
	const int nR = nGam*100;
	const int nSubj = getNbrSubj(nameFileResponse);
	// cout << "nSubj=" << nSubj << " nR=" << nR << endl;
	if (maxSubj > nSubj) {maxSubj = nSubj;}
	int response[nSubj*nR];					// static
	loadResponse(nameFileResponse, response);			// response <- loadResponse()
		
	double **Solution = new double* [maxSubj*nRestart];
	for (int i = 0; i < maxSubj*nRestart; i++) {Solution[i] = new double[nParam+1];}	// params and LL
	
	double *minSol[maxSubj];
	double *treshold = new double[nRound];			// array containing the treshold
	
	
	int rowInSol = 0;
	for (int subj = 0; subj < maxSubj; subj++) {
		time0 = time(0);
		cout << "load=" << nameFileResponse << ", subj=" << subj << "/" << maxSubj << "/" << nSubj << endl;
		int *y = selSubj(response, subj);		// y <- select one subject
		// printTab("y", y, nR);

		// create the sequence
		theSequence(treshold, y);					// ON-OFF, treshold <- theSequence
		// OFF printTab("treshold", treshold, nRound);
	
		// search solution
		minLl = GSL_POSINF;
		for (int restart = 0; restart < nRestart; restart++) {
			// create the sequence
			// theSequence(treshold, y);				// ON-OFF, treshold <- theSequence
			// OFF printTab("treshold", treshold, nRound);

			tah3D(Solution, y, treshold, rowInSol);		// Solution[rowInSol] <- tah3D
			printTab("solution", Solution[rowInSol], nParam+1);
			
			ll = Solution[rowInSol][nParam];			// col where LL is save
			if (ll < minLl) {
				minLl = ll;
				minRowInSol = rowInSol;
			}
			// cout << "minLl=" << minLl << endl;			
			rowInSol++;
		}
		minSol[subj] = Solution[minRowInSol];			// keep min solution
		printTab("solution", Solution[minRowInSol], nParam+1);
				
		time1 = time(0);
		// OFF cout << "Time for TA = " << time1 - time0 << " sec" << endl;
	}	
	saveArray(nameFileSol, Solution, maxSubj*nRestart, nParam+1);		// save solution
	saveArray(nameFileMinSol, minSol, maxSubj, nParam+1);			// save min solution
	
	cout << "Finished " << nameFileSol << endl;
	
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
	
	cout << "Reinforcement loaded." << endl;
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

int * selSubj(int *response, int subj) {
	return (response + subj*nGam*100);
}

double logLHPE3D(double *param, int *y) {
	static double p[4];									// init ok
	static double meanVal[4], varVal[4], x[4];
	static double rewVal, pE, pEVar, pEScale, pEScale2, sumX, sumLog;
	static int first, last, firstCard, lastCard;		// init ok
	static int cardInDeck[4];							// init ok
	static int deck, card; 								// deck selected, position in deck
	
	sumLog = 0;
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
			deck = y[k]; 						// select the deck
			if (!gsl_finite(p[deck])) {				// p = nan, -inf, or + inf
					return(GSL_NAN);
			}
			sumLog += log(p[deck]);					// add the log of the prob to the total
			card = cardInDeck[deck]; 				// position in the deck
			cardInDeck[deck]++;					// one card further in that deck

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
			for (int i = 0; i < 4; i++) {p[i] = x[i] / sumX;}
		}
	}
	return(-sumLog);
}

inline bool isPositive(double val) {
	return val > 0;
}

inline bool isNotFinite(double val) {
	return !gsl_finite(val);
}

void theSequence(double *treshold, int *y) {
	static double point[nParam];
	static double f0, f1, u;
	
	// generate random points and neighbours
	static int nPoint = min(nRound*nStep, 5000);				// 5000 rand points at max			
	static double *distF = new double[nPoint];			
	
	for (int i = 0; i < nPoint; i++) {					// generate nPoint random points and neighbours
		for (int j = 0; j < nParam; j++) {
			u = gsl_rng_uniform(r);					
			point[j] = minBound[j] + dBound[j]*u;		// init random point
		}
		f0 = logLHPE3D(point, y);
		neighbourNormal3D(point); 				// replace point by its neighbour
		f1 = logLHPE3D(point, y);
		distF[i] = f1 - f0;			
	}

	
	// create a sequence of thresholds
	int nTrim = (int)(nPoint*pTrim);							// not sure !!
	
	double *p = remove_if(distF, distF + nPoint, isNotFinite);	// put nan at the end, nan start at p	
	sort(distF, p);							// sort distances (nan excluded)
	// double *firstDist, *lastDist;
	double *firstDist = find_if(distF, p, isPositive);					// where positive values start (nan exluded)
	double *lastDist = p - nTrim;								// pos of last value (trim removed)
	int nPosDist = lastDist - firstDist;						// nbr pos dist retained
	int oneMove = nPosDist / nRound	;							// nbr of dist for one move
	
	double *pTreshold;
	for (int n = 1; n < nRound; n++) {
		pTreshold = lastDist - n*oneMove;
		treshold[n-1] = *pTreshold;							// copy the treshold in the array
	}
	treshold[nRound-1] = 0;									// last treshold is null	
	// printTab("treshold", treshold, nRound);
}


void tah3D(double **Solution, int *y, double *treshold, int rowInSol) {
	static double point[nParam];
	static double f0, f1, u;

	
	// init a starting point
	for (int i = 0; i < nParam; i++) {
		u = gsl_rng_uniform(r);							// get unifrom [0, 1[
		point[i] = minBound[i] + dBound[i]*u;					// init starting point
	}
	f0 = logLHPE3D(point, y);							// f of the starting point
	
	double neighbourPoint[nParam];
	for (int i = 0; i < nParam; i++) {
			neighbourPoint[i] = point[i];					// copy the point to leave it untouch
	}

	for (int iRound = 0; iRound < nRound; iRound++) {
		for (int iStep = 0; iStep < nStep; iStep++) {
			neighbourNormal3D(neighbourPoint);				// change to neighbour
			f1 = logLHPE3D(neighbourPoint, y);
			if (f1 <= f0 + treshold[iRound]) {				// the neighbour is accepted
				for (int i = 0; i < nParam; i++) {
					point[i] = neighbourPoint[i];			// the neighbour become the new point
				}
				f0 = f1;
			}
			else {						 		// the neighbour is rejected
				for (int i = 0; i < nParam; i++) {
					neighbourPoint[i] = point[i];			// back to the point
				}
			}
		}
	}
	// cout << "rowInSol=" << rowInSol << endl;
	for (int i = 0; i < nParam; i++) {
		Solution[rowInSol][i] = point[i];					// copy to solution
	}
	Solution[rowInSol][nParam] = f0;							// copy maximum likelihood
	
	
}

inline void neighbourNormal3D(double *point) {						// change point to a neighbour
	static int j;
	static double u;
	
	j = gsl_rng_get(r) % nParam;							// rand int, 0, or 1
	u = gsl_ran_gaussian(r, 1);						// normal distrib
	point[j] = point[j] + u*dBound[j]/2 * frac;
	point[j] = min(maxBound[j], max(minBound[j], point[j]));
}


void printTab(const char *name, int *tab, int size) { // added const before char
	cout.setf(ios::left, ios::adjustfield);
	cout << setw(4) << name << "=";
	cout.setf(ios::right, ios::adjustfield);
	for (int i = 0; i < size; i++) {
		cout << setw(3) << tab[i];
	}
	cout << endl;
}

void printTab(const char *name, double *tab, int size) { // added const before char
	cout.setf(ios::left, ios::adjustfield);
	cout << setw(8) << name << " =";
	cout.setf(ios::right, ios::adjustfield);
	for (int i = 0; i < size-1; i++) {
		cout << setw(10) << setprecision(3) << tab[i];
	}
	cout << setw(12) << setprecision(8) << tab[size-1];
	cout << endl;
}



