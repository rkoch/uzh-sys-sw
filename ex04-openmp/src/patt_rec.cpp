/*
 * patt_rec.cpp
 *
 * Course: Systems Software, Assignment: 04
 * Fall Semester 2013
 * University of Zurich
 *
 * Created on: 03.11.2013
 * Updated on: 27.11.2013
 * Authors:    Raphael Matile / Remo Koch
 * License:    MIT
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <cstring>
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <iomanip> // setprecision
using std::abs;
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

typedef unsigned long ulong;
typedef unsigned int uint;

//	y
//  | (0/0) (1/0)
//  | (1/0) (1/1)
//  | (2/0) (2/1)
//	--------------x
struct occurence {
	int x_pos;
	int y_pos;
	float goodness;
};

vector<occurence> occurence_list; // list with all occurences in it
vector<vector<int> > data; // content of data-file
vector<vector<int> > pattern; // content of pattern-file

double start_time = 0.0;
double end_time = 0.0;

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
vector<vector<int> > read_file(const char *pPath) {
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

	uint lineNumber = 0; // current line number
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
bool check_median(vector<int> a, vector<int> b) {
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

float calculate_goodness(vector<int> data_entries, vector<int> pattern_entries, int pattern_width) {
	// a and b have the same size
	int sum = 0;
	for (uint i = 0; i < data_entries.size(); i++) {
		sum += abs(data_entries.at(i) - pattern_entries.at(i));
	}
	unsigned long denominator = pattern_width * pattern_width;
	if (denominator == 0) {
		cerr << "An error occured during calculation of goodness: Divide by zero is not allowed!" << endl;
		exit(EXIT_FAILURE);
	}
	float goodness = (1.0f / denominator) * sum;

	return goodness;
}

// Computing algorithm
inline void compute(int num_of_threads) {
	// data_width = the edge width of the given pattern
	ulong data_width = data.at(0).size(); // width is equal over the whole dataScheme
	// data_height = the edge height of the given pattern
	ulong data_height = data.size();
	// pattern_width = the edge length of the given pattern
	ulong pattern_width = pattern.size(); // Patterns are always squares according the excercise

	start_time = omp_get_wtime();

	// serial mode if num_of_threads <= 0
#pragma omp parallel for if (num_of_threads > 0) shared(occurence_list) schedule(static) num_threads(num_of_threads)
	for (uint y = 0; y <= data_height - pattern_width; y++) {
		for (uint x = 0; x <= data_width - pattern_width; x++) {
			// compare each element of the pattern with the corresponding one of the dataScheme according to the rules described later
			bool max_not_exceeding = true;
			bool median_not_exceeding = false;
			vector<int> data_entries;
			vector<int> pattern_entries;

			for (ulong patt_y = 0; patt_y < pattern_width; patt_y++) {
				for (ulong patt_x = 0; patt_x < pattern_width; patt_x++) {
					int cur_data = data.at(y + patt_y).at(x + patt_x);
					int cur_patt = pattern.at(patt_y).at(patt_x);

					// condition 1: each pair must have a difference which is smaller than 10
					if (abs(cur_data - cur_patt) >= MAX_DIFFERENCE) {
						max_not_exceeding = false;
					}

					data_entries.push_back(cur_data);
					pattern_entries.push_back(cur_patt);
				}
			}

			// check second condition only if first does match
			if (max_not_exceeding) {
				// condition 2: the median over all values of the temporary part (returns true if diff is smaller than MAX_MEDIAN_DIFFERENCE)
				median_not_exceeding = check_median(data_entries, pattern_entries);
				if (median_not_exceeding) {
					occurence occ;
					occ.x_pos = x;
					occ.y_pos = y;
					// calculate goodness
					occ.goodness = calculate_goodness(data_entries, pattern_entries, pattern_width);

					// ENTERING critical section: Writing to shared list
#pragma omp critical
					occurence_list.push_back(occ); // report found pattern
					// LEAVING critical section
				}
			}
		}
	}

	end_time = omp_get_wtime();
}

// computes the min, average, max value of goodness over all occurences
// v[0]: min
// v[1]: avg
// v[2]: max
vector<float> compute_min_max_avg_goodness() {
	vector<float> result;
	uint size = occurence_list.size();

	if (size == 0) {
		cerr << "Failure during average-goodness-computation. Divide by zero is not allowed." << endl;
		exit(EXIT_FAILURE);
	}

	// min, max goodness
	float min = occurence_list.at(0).goodness;
	float max = occurence_list.at(0).goodness;
	float sum = 0.0;
	for (uint i = 0; i < size; i++) {
		if (occurence_list.at(i).goodness < min) {
			min = occurence_list.at(i).goodness;
		}
		if (occurence_list.at(i).goodness > max) {
			max = occurence_list.at(i).goodness;
		}
		sum += occurence_list.at(i).goodness;
	}

	float avg = sum / size;

	result.push_back(min);
	result.push_back(avg);
	result.push_back(max);

	return result;
}

// writes min, avg, max goodness on std::cout
// @pre res[0]: min, res[1]: avg, res[2]: max
void write_goodness(const vector<float> &res) {
	cout << "min goodness: " << res.at(0) << "\n";
	cout << "avg goodness: " << res.at(1) << "\n";
	cout << "max goodness: " << res.at(2) << endl; // only flush at the end
}

void write_exec_time() {
	cout << "execution time: " << (end_time - start_time) << "s" << endl;
}

// writes each occurence to the file "occurences.txt".
// Note: If a file name "occurences.txt" already exits, it will be overwritten
void write_results() {
	ofstream outfile("occurences.txt", ios::out | ios::app | ios::trunc);
	outfile.open("occurences.txt");
	if (!outfile.is_open()) {
		cerr << "There was an error writing the results file!" << endl;
		outfile.close();
		exit(EXIT_FAILURE);
	}

	for (uint i = 0; i < occurence_list.size(); i++) {
		outfile << occurence_list.at(i).x_pos << " " << occurence_list.at(i).y_pos << endl;
	}

	outfile.close();
}

void write_usage() {
	cerr << "usage: " << PATT_REC << " [data_matrix.txt] [pattern_matrix.txt] [mode: serial=0, parallel=1] [nrOfThreads]" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, const char * argv[]) {
	// check cli-arguments
	if ((argc < 4) // Less than 3 arguments
	|| ((strcmp(argv[3], "0") != 0) && (strcmp(argv[3], "1"))) // OR no valid mode selected
			|| ((strcmp(argv[3], "1") == 0) // OR if in parallel mode
			&& ((argc < 5) || (atoi(argv[4]) <= 0)))) { //    AND no 4th argument OR number of threads is smaller-or-equals 0 or no number
		write_usage();
	}

	// read data and pattern into a 2D structure
	data = read_file(argv[1]);
	pattern = read_file(argv[2]);

	bool serial_mode = (strcmp(argv[3], "0") == 0);
	int num_of_threads = 0;
	if (!serial_mode) {
		num_of_threads = atoi(argv[4]);
	}
	compute(num_of_threads);

	// writes found occurences to a textfile
	write_results();
	// writes execution time to cout
	write_exec_time();
	// writes min, avg, max goodness to std::cout
	write_goodness(compute_min_max_avg_goodness());

	return 0;
}

