/*
 * client.h
 *
 * Course: Systems Software, Assignment: 06
 * Fall Semester 2013
 * University of Zurich
 *
 * Created on: 05.12.2013
 * Authors:    Raphael Matile / Remo Koch
 * License:    MIT
 */

#ifndef __ex_06__client__
#define __ex_06__client__

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <functional>
#include <errno.h>

typedef unsigned int uint;

class client {
public:
	client(std::string pPath);
	~client() {
		close(client_sock);
	}
	;
	void run();

private:
	struct sockaddr_un remote;
	int client_sock;

	size_t recv_msg_length;
	socklen_t addr_size;

	void send_msg(std::string pMessage);
	std::string recv_msg();

	// returns a string which represents the value of the given boolean
	inline std::string const boolToString(bool b) {
		return b ? "true" : "false";
	}
	// returns a bool which represents the value of the given string
	inline bool stringToBool(std::string const& s) {
		return s == "false" ? false : true;
	}

	bool hasWon;
	int has_hit_ctr;

	bool checkPlayfield(std::string pCoordinates);
	void drawPlayfield();
	void initalize_playfield();

	// row(column)
	// 0 represents shot which didn't hit
	// 1 represents boat
	// 2 represents hit
	std::vector<std::vector<int> > enemy_playfield;
	// 0 represents none
	// 1 represents shot, which didn't hit
	// 2 represents shot, which hit
	std::vector<std::vector<int> > local_playfield;

};

#endif /* defined(__ex_06__client__) */
