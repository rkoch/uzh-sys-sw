/*
 * server.h
 *
 * Course: Systems Software, Assignment: 06
 * Fall Semester 2013
 * University of Zurich
 *
 * Created on: 05.12.2013
 * Authors:    Raphael Matile / Remo Koch
 * License:    MIT
 */

#ifndef __ex_06__server__
#define __ex_06__server__

#include <iostream>
#include <vector>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <functional>
#include <errno.h>
#include <stdlib.h>

typedef unsigned int uint;

class server {
public:
	server(std::string pPath);
	~server() {
		close(server_sock);
		close(client_sock);
		unlink(local.sun_path);
	}

	void run();

private:
	int server_sock;
	int client_sock;

	sockaddr_un local;
	sockaddr_un remote;

	size_t recv_msg_length; // length of received message
	socklen_t addr_size; // length of local socket path

	void accept_connect();
	std::string recv_msg();
	void send_msg(std::string pMessage);

	// returns a string which represents the value of the given boolean
	inline std::string const boolToString(bool b) {
		return b ? "true" : "false";
	}
	// returns a bool which represents the value of the given string
	inline bool stringToBool(std::string const& s) {
		return s == "false" ? false : true;
	}

	bool hasWon;
	int has_hit_ctr; // counts the number of hits of the local player

	void initalize_playfield();
	bool checkPlayfield(std::string pCoordinates);
	void drawPlayfield();

	// row(column)
	// 0 represents hit which didn't hit
	// 1 represents boat
	// 2 represents hit
	std::vector<std::vector<int> > enemy_playfield;
	// 0 represents none
	// 1 represents hit, which didn't hit
	// 2 represents hit, which hit
	std::vector<std::vector<int> > local_playfield;
};

#endif /* defined(__ex_06__server__) */
