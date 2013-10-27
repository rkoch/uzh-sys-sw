/*
 * matcher.cpp
 *
 * Course: Systems Software, Assignment: 03
 * Fall Semester 2013
 * University of Zurich
 *
 * Created on: 27.10.2013
 * Authors:    Raphael Matile / Remo Koch
 * License:    MIT
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cerrno>

using std::cerr;
using std::endl;
using std::map;
using std::string;
using std::vector;
using std::ios;
using std::ifstream;
using std::ofstream;

#ifndef _MATCHER_DEFS_
#define _MATCHER_DEFS_

#define MATCHER "matcher"

#endif

typedef unsigned int uint;

struct subSequence {
	int begin;
	int end;
	string content;
};

struct pattern {
	int length;
	string content;
	vector<subSequence> subSequences;

	bool operator<(const pattern& a) const {
		return this->content < a.content;
	}
};

struct sequence {
	int length;
	string content;
};

map<pattern, vector<int> > occurences;
vector<sequence> sequenceList;
vector<pattern> patternList;

void readPatterns(const char *pPath) {
	ifstream infile;
	infile.open(pPath);

	if (!infile.good()) {
		cerr << "An error occured during file read. File: " << pPath << ", Error: " << strerror(errno) << endl;
		infile.close();
		exit(4);
	}

	vector<string> input;
	while (!infile.eof()) {
		string tmp;
		infile >> tmp;
		input.push_back(tmp);
	}

	infile.close();
	// create pattern, input.at(0): nrOfPatterns in textfile
	for (int i = 0; i < (atoi(input.at(0).c_str())) * 2; i += 2) {
		pattern tmp;
		vector<int> tmpV;
		tmp.length = atoi(input.at(i + 1).c_str());
		tmp.content = input.at(i + 2);
		occurences[tmp] = tmpV;          // array operator creates new key, if it doesn't exist...
		patternList.push_back(tmp);
	}
}

// read Sequences
void readSequences(const char *pPath) {
	ifstream infile;
	infile.open(pPath);

	if (!infile.good()) {
		cerr << "An error occured during file read. File: " << pPath << ", Error: " << strerror(errno) << endl;
		infile.close();
		exit(4);
	}

	vector<string> input;
	while (!infile.eof()) {
		string tmp;
		infile >> tmp;
		input.push_back(tmp);
	}

	infile.close();

	for (uint i = 0; i < input.size() / 2; i++) {
		sequence tmp;
		tmp.length = atoi(input.at(i).c_str());
		tmp.content = input.at(i + 1);
		sequenceList.push_back(tmp);
	}
}

// create subSequences in each sequence depending on the length of a pattern
void createSubSequences(int pNrOfThreads) {
	for (uint i = 0; i < sequenceList.size(); i++) {
		int seqLength = sequenceList.at(i).length;

		int subSeqLength = seqLength / pNrOfThreads;

		// not more threads than min length of sequencelist.length
		if (subSeqLength == 0) {
			cerr << "More threads than the minimum length of a sequence over all entries in sequenceList are not allowed!" << endl;
			cerr << "Max. allowed threads over all sequences are: " << seqLength << endl;
			exit(5);
		}

		for (uint j = 0; j < patternList.size(); j++) {
			int pos = 0;
			for (int k = 0; k < pNrOfThreads - 1; k++) {
				subSequence tmp;
				tmp.begin = pos;
				tmp.end = pos + subSeqLength - 1 + patternList.at(j).length - 1;          // with letter at end, with overlap of pattern
				tmp.content = sequenceList.at(i).content.substr(tmp.begin, subSeqLength - 1 + patternList.at(j).length);
				patternList.at(j).subSequences.push_back(tmp);
				pos += subSeqLength;          // change starting letter to one next to the last of the previous subsequence
			}
			// last subSequence with rest in it
			subSequence tmp;
			tmp.begin = pos;
			tmp.end = tmp.begin + (seqLength - pos);
			tmp.content = sequenceList.at(i).content.substr(tmp.begin, (tmp.end - tmp.begin));
			patternList.at(j).subSequences.push_back(tmp);
		}
	}
}

// parallel execution of lookup, called for each pattern
void *exec_parallel(void *pPattern) {
	pattern *tmp = (pattern *) pPattern;
	// for each pattern go through all subSequences
	for (uint j = 0; j < tmp->subSequences.size(); j++) {
		int subSeqLength = (tmp->subSequences.at(j).end - tmp->subSequences.at(j).begin) + 1;          // TODO +1 weg, -1 in for schleife weg...
		for (int k = 0; k < subSeqLength - 1; k++) {
			// going through the whole subSequence
			if ((tmp->length - 1 < subSeqLength) && (tmp->subSequences.at(j).content.substr(k, tmp->length) == tmp->content)) {
				// report k as starting position
				occurences[*tmp].push_back(k + tmp->subSequences.at(j).begin);
			}
		}
	}
	pthread_exit(NULL);
}

// serial execution
void exec_serial(void *pPattern) {
	pattern *tmp = (pattern *) pPattern;
	// for each pattern go through all subSequences
	for (uint j = 0; j < tmp->subSequences.size(); j++) {
		int subSeqLength = (tmp->subSequences.at(j).end - tmp->subSequences.at(j).begin);
		for (int k = 0; k < subSeqLength; k++) {
			// going through the whole subSequence
			if ((tmp->length - 1 < subSeqLength) && (tmp->subSequences.at(j).content.substr(k, tmp->length) == tmp->content)) {
				// report k as starting position
				occurences[*tmp].push_back(k + tmp->subSequences.at(j).begin);
			}
		}
	}
}

// write results to results.txt
// Note: If a file named "occurences.txt" already exists, its content will be overwritten
void writeResults() {
	ofstream outfile("occurences.txt", ios::out | ios::app | ios::trunc);
	outfile.open("occurences.txt");
	if (!outfile.is_open()) {
		cerr << "There was an error writing the results file!" << endl;
		outfile.close();
		exit(6);
	}

	for (uint i = 0; i < occurences.size(); i++) {
		int size = (int) occurences[patternList.at(i)].size();
		outfile << size << endl;
		for (uint j = 0; j < occurences[patternList.at(i)].size(); j++) {
			int tmpPos = occurences[patternList.at(i)].at(j);
			outfile << tmpPos << " ";
		}
		outfile << endl;
	}

	outfile.close();
}

void writeUsage() {
	cerr << "usage: " << MATCHER << " [sequences.txt] [patterns.txt] [mode: serial=0, parallel=1] [nrOfThreads]" << endl;
	exit(EXIT_FAILURE);
}

int main(int argc, const char* argv[]) {

	// Read and validate CLI arguments
	if ((argc < 4)                                              // Less than 3 arguments
	|| ((strcmp(argv[3], "0") != 0) && (strcmp(argv[3], "1")))  // OR no valid mode selected
			|| ((strcmp(argv[3], "1") == 0)                     // OR if in parallel mode
					&& ((argc < 5) || (atoi(argv[4]) <= 0)))) { //    AND no 4th argument OR number of threads is smaller-or-equals 0 or no number
		writeUsage();
	}

	readSequences(argv[1]);
	readPatterns(argv[2]);

	// check wheter parallel or serial mode should be executed
	bool serialMode = (strcmp(argv[3], "0") == 0);
	if (serialMode) {
		// serial mode
		createSubSequences(1);
		for (uint i = 0; i < patternList.size(); i++) {
			exec_serial((void *) (&patternList.at(i)));
		}
	} else {
		// parallel mode
		createSubSequences(atoi(argv[4]));
		pthread_t p_threads[atoi(argv[4])];
		for (uint i = 0; i < patternList.size(); i++) {
			if (pthread_create(&p_threads[i], NULL, exec_parallel, (void *) (&patternList.at(i))) != 0) {
				perror("pthread_create() error");
				exit(2);
			}
		}
		for (uint i = 0; i < patternList.size(); i++) {
			if (pthread_join(p_threads[i], NULL) != 0) {
				perror("pthread_create() error");
				exit(3);
			}
		}
	}
	writeResults();

	return 0;
}
