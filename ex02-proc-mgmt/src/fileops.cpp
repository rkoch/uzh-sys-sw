/*
 * fileops.cpp
 *
 * Course: Systems Software, Assignment: 02
 * Fall Semester 2013
 * University of Zurich
 *
 * Created on: 07.10.2013
 * Authors:    Remo Koch / Raphael Matile
 * License:    MIT
 */

#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::string;

#ifndef _FILEOPS_GREP_CMD_
#define _FILEOPS_GREP_CMD_

#define WC "/usr/bin/wc"
#define GREP "/usr/bin/grep"
#define FILEOPS "fileops"

#endif

int main(int pArgc, const char* pArgv[]) {
	int ret = 0;
	string inputFile;

	// Read and validate CLI arguments
	if (pArgc < 2) {
		cerr << "usage: " << FILEOPS << " file ..." << endl;
		exit(EXIT_FAILURE);
	}
	inputFile = pArgv[1];

	// Ask user for exec type
	cout << "Which operation should be performed on the given input file?" << endl;
	cout << "(set input file: " << inputFile << ")" << endl;
	cout << endl;

	cout << "1 )  Count the number of lines in the file" << endl;
	cout << "2 )  Search for a specific string in the file" << endl;
	cout << endl;

	string type;
	string searchPattern;

	bool valid = false;
	while (!valid) {
		cout << "Choose one of the options above: ";
		cin >> type;

		if ((type != "1") && (type != "2")) {
			cout << "Your input was not recognized. Please try again." << endl;
			cout << endl;
		} else {
			valid = true;
		}
	}

	// Ask user for search pattern (grep exec type)
	if (type == "2") {
		valid = false;
		while (!valid) {
			cout << endl;
			cout << "Please enter the pattern to search for: ";
			cin >> searchPattern;

			if (searchPattern.length() > 0) {
				valid = true;
			} else {
				cout << "Your input was not recognized. Please try again." << endl;
			}
		}
	}

	cout << endl;
	cout << "Execution result:" << endl;
	cout << endl;

	// Create a new process to execute
	pid_t pid = vfork();

	if (pid < 0) {
		// failed to fork
		perror("error creating child process");
		exit(EXIT_FAILURE);
	} else if (pid == 0) {
		// child process
		if (type == "1") {          // wc
			execl(WC, WC, "-l", inputFile.c_str(), (char*) 0);
		} else {                    // grep
			execl(GREP, GREP, "-r", searchPattern.c_str(), inputFile.c_str(), (char*) 0);
		}

		perror("error in child execl()");
		exit(EXIT_FAILURE);
	} else {
		// parent process
		int status;

		// blocks and waits for returning of child process
		wait(&status);

		if (WIFEXITED(status)) {
			// get retcode after exit
			int crc = WEXITSTATUS(status);
			if (type == "1") {
				if (crc > 0) {
					// wc rc is 1 on error
					ret = 1;
				}
			} else {
				if (crc == 1) {
					// grep rc is 1 on string not found
					ret = 2;
				} else if (crc > 1) {
					// grep returns > 1 if another error happened
					ret = 3;
				}
			}
		}
	}

	// return exit code
	return ret;

}
