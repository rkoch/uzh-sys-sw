/*
 * patt_rec.cpp
 *
 * Course: Systems Software, Assignment: 04
 * Fall Semester 2013
 * University of Zurich
 *
 * Created on: 03.11.2013
 * Authors:    Raphael Matile / Remo Koch
 * License:    MIT
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>
#include "math.h"
#include <omp.h>
#include <iomanip> // setprecision

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;
using std::strcmp;
using std::getline;
using std::vector;
using std::ifstream;
using std::ofstream;
using std::sort;
using std::ios;

#ifndef _PATTREC_DEFS_
#define _PATTREC_DEFS_ 1

#define PATT_REC "patt_rec"

#endif


//	y
//  | (0/0) (1/0)
//  | (1/0) (1/1)
//  | (2/0) (2/1)
//	--------------x
struct occurence {
	int xPos;
	int yPos;
	float goodness;
};

vector<occurence> occurenceList; // list with all occurences in it
vector<vector<int> > data; // content of data-file
vector<vector<int> > pattern; // content of pattern-file

double startTime = 0.0;
double endTime = 0.0;

const int MAX_DIFFERENCE = 10;
const int MAX_MEDIAN_DIFFERENCE = 3;

vector<int> &split(const string &s, char delim, vector<int> &elems) {
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim)) {
		elems.push_back(atoi(item.c_str()));
	}
	return elems;
}

vector<int> split(const string &s, char delim) {
	vector<int> elems;
	split(s, delim, elems);
	return elems;
}

// read File
vector<vector<int> > readFile(const char *pPath) {
	ifstream infile(pPath);
	if (!infile.good()) {
		perror("I/O Error occured.");
		infile.close();
		exit(EXIT_FAILURE);
	}
	int rows;
	int cols;
	infile >> rows; // zeile
	infile >> cols; // spalte

	int lineNumber = 0; // current line number
	vector<vector<int> > tmp(rows, vector<int>(cols));
	string elem;

	while (getline(infile, elem) && (lineNumber < tmp.size())) {
		if (elem != "") {
			tmp.at(lineNumber) = split(elem, ' ');
			++lineNumber;
		}
	}
	infile.close();
	return tmp;
}

// calculate Median of two vectors and check condition 2 according to the exercise
bool checkMedian(vector<int> a, vector<int> b) {
	sort(a.begin(), a.end());
	sort(b.begin(), b.end());

	double aMedian;
	double bMedian;
	if ((a.size() % 2) == 0) {
		// even size
		aMedian = ((a.at(a.size() / 2)) + (a.at((a.size() / 2) - 1))) / 2.0;
	} else {
		// odd size
		aMedian = a.at(a.size() / 2);
	}

	if ((b.size() % 2) == 0) {
		// even size
		bMedian = ((b.at(b.size() / 2)) + (b.at((b.size() / 2) - 1))) / 2.0;
	} else {
		// odd size
		bMedian = b.at(b.size() / 2);
	}

	if (fabs(aMedian - bMedian) < MAX_MEDIAN_DIFFERENCE) {
		return true;
	}

	return false;
}

float calculateGoodnes(vector<int> a, vector<int> b) {
	// a and b have the same size
	int sum = 0;
	for (int i = 0; i < a.size(); i++) {
		sum += abs(a.at(i) - b.at(i));
	}
	unsigned long n = a.size() * a.size();
	if (n == 0) {
		cerr << "An error occured during calculation of goodness: Divide by zero is not allowed!" << endl;
		exit(EXIT_FAILURE);
	}
	float goodness = ((1.0f / n) * sum);
	return goodness;
}

// find occurence in a parallel way using OpenMP
void parallelExec(int num_of_threads) {
	startTime = omp_get_wtime();
	unsigned long tmpPatternHeightWidth = pattern.size(); // Patterns are always squares according the excercise
	// going through all sequences
	unsigned long tmpDataSchemeWidth = data.at(0).size(); // width is equal over the whole dataScheme
	unsigned long tmpDataSchemeHeight = data.size();

	// going through the whole data-scheme
#pragma omp parallel for shared(occurenceList) schedule(static) num_threads(num_of_threads)
	for (int k = 0; k < tmpDataSchemeHeight - tmpPatternHeightWidth + 1; k++) {
		for (int l = 0; l < tmpDataSchemeWidth - tmpPatternHeightWidth + 1; l++) {
			// compare each element of the pattern with the corresponding one of the dataScheme according to the rules described later
			bool cond1 = false;
			bool cond2 = false;
			vector<int> tmpListForMedianData;
			vector<int> tmpListForMedianPattern;
			occurence tmp;
			tmp.xPos = l;
			tmp.yPos = k;

			for (int m = 0; m < tmpPatternHeightWidth; m++) {
				for (int n = 0; n < tmpPatternHeightWidth; n++) {
					// condition 1: each pair must have a difference of at most 10
					// condition 2: the median over all values of the temporary part
					if ((data.at(k + m).at(l + n) - pattern.at(m).at(n)) < MAX_DIFFERENCE) {
						cond1 = true;
					}
					tmpListForMedianData.push_back(data.at(k + m).at(l + n));
					tmpListForMedianPattern.push_back(pattern.at(m).at(n));
				}
			}
			// check both conditions; if the first condition is false, it doesn't matter, if the second is true
			if (cond1) {
				// calculate median
				cond2 = checkMedian(tmpListForMedianData, tmpListForMedianPattern);
				if (cond2) {
					// calculate goodness
					tmp.goodness = calculateGoodnes(tmpListForMedianData, tmpListForMedianPattern);
					// report l,k as coordinates for a found pattern
					occurenceList.push_back(tmp);
				}
			}
		}
	}
	endTime = omp_get_wtime();

}

// find occurences in a serial way
void serialExec() {
	startTime = omp_get_wtime();
	unsigned long tmpPatternHeightWidth = pattern.size(); // Patterns are always squares according the excercise
	unsigned long tmpDataSchemeWidth = data.at(0).size(); // width is equal over the whole dataScheme
	unsigned long tmpDataSchemeHeight = data.size();
	// going through the whole data-scheme
	for (int k = 0; k < tmpDataSchemeHeight - tmpPatternHeightWidth + 1; k++) {
		for (int l = 0; l < tmpDataSchemeWidth - tmpPatternHeightWidth + 1; l++) {
			// compare each element of the pattern with the corresponding one of the dataScheme according to the rules described later
			bool cond1 = false;
			bool cond2 = false;
			vector<int> tmpListForMedianData;
			vector<int> tmpListForMedianPattern;
			occurence tmp;
			tmp.xPos = l;
			tmp.yPos = k;

			// std::cout << "current element: " << dataList.at(j).at(k).at(l) << std::endl;

			for (int m = 0; m < tmpPatternHeightWidth; m++) {
				for (int n = 0; n < tmpPatternHeightWidth; n++) {
					// condition 1: each pair must have a difference of at most 10
					// condition 2: the median over all values of the temporary part
					// std::cout << patternList.at(i).at(m).at(n) << std::endl;
					if ((data.at(k + m).at(l + n) - pattern.at(m).at(n)) < MAX_DIFFERENCE) {
						cond1 = true;
					}
					tmpListForMedianData.push_back(data.at(k + m).at(l + n));
					tmpListForMedianPattern.push_back(pattern.at(m).at(n));
				}
			}
			// check both conditions; if the first condition is false, it doesn't matter, if the second is true
			if (cond1) {
				// calculate median
				cond2 = checkMedian(tmpListForMedianData, tmpListForMedianPattern);
				if (cond2) {
					// calculate goodness
					tmp.goodness = calculateGoodnes(tmpListForMedianData, tmpListForMedianPattern);
					// report l,k as coordinates for a found pattern
					occurenceList.push_back(tmp);
				}
			}
		}
	}
	endTime = omp_get_wtime();
}

// computes the min, average, max value of goodness over all occurences
// v[0]: min
// v[1]: avg
// v[2]: max
vector<float> computeMinAvgMaxGoodness() {
	vector<float> result;
	unsigned long size = occurenceList.size();

	if (size == 0) {
		cerr << "Failure during average-goodness-computation. Divide by zero is not allowed." << endl;
		exit(EXIT_FAILURE);
	}

	// min, max goodness
	float min = occurenceList.at(0).goodness;
	float max = occurenceList.at(0).goodness;
	float sum = 0.0;
	for (int i = 0; i < size; i++) {
		if (occurenceList.at(i).goodness < min) {
			min = occurenceList.at(i).goodness;
		}
		if (occurenceList.at(i).goodness > max) {
			max = occurenceList.at(i).goodness;
		}
		sum += occurenceList.at(i).goodness;
	}

	float avg = sum / size;

	result.push_back(min);
	result.push_back(avg);
	result.push_back(max);

	return result;
}

// writes min, avg, max goodness on std::cout
// @pre res[0]: min, res[1]: avg, res[2]: max
void writeGoodness(const vector<float> &res) {
	cout << "min goodness: " << res.at(0) << "\n";
	cout << "avg goodness: " << res.at(1) << "\n";
	cout << "max goodness: " << res.at(2) << endl; // only flush at the end
}

void writeExecTime() {
	cout << "execution time: " << (endTime - startTime) << "s" << endl;
}

// writes each occurence to the file "occurences.txt".
// Note: If a file name "occurences.txt" already exits, it will be overwritten
void writeResults() {
	ofstream outfile("occurences.txt", ios::out | ios::app | ios::trunc);
	outfile.open("occurences.txt");
	if (!outfile.is_open()) {
		cerr << "There was an error writing the results file!" << endl;
		outfile.close();
		exit(EXIT_FAILURE);
	}

	for (int i = 0; i < occurenceList.size(); i++) {
		outfile << occurenceList.at(i).xPos << " " << occurenceList.at(i).yPos << endl;
	}

	outfile.close();
}

void writeUsage() {
	cerr << "usage: " << PATT_REC << " [data_matrix.txt] [pattern_matrix.txt] [mode: serial=0, parallel=1] [nrOfThreads]" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, const char * argv[]) {
	// check cli-arguments
	if ((argc < 4) // Less than 3 arguments
	|| ((strcmp(argv[3], "0") != 0) && (strcmp(argv[3], "1"))) // OR no valid mode selected
			|| ((strcmp(argv[3], "1") == 0) // OR if in parallel mode
			&& ((argc < 5) || (atoi(argv[4]) <= 0)))) { //    AND no 4th argument OR number of threads is smaller-or-equals 0 or no number
		writeUsage();
	}

	// read data and pattern into a 2D structure
	data = readFile(argv[1]);
	pattern = readFile(argv[2]);

	bool serialMode = (strcmp(argv[3], "0") == 0);
	if (serialMode) {
		serialExec();
	} else {
		parallelExec(atoi(argv[4]));
	}

	// writes found occurences to a textfile
	writeResults();
	// writes execution time to cout
	writeExecTime();
	// writes min, avg, max goodness to std::cout
	writeGoodness(computeMinAvgMaxGoodness());

	return 0;
}

