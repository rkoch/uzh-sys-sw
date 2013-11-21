/*
 * food_stand.cpp
 *
 * Course: Systems Software, Assignment: 05
 * Fall Semester 2013
 * University of Zurich
 *
 * Created on: 20.11.2013
 * Authors:    Remo Koch / Raphael Matile
 * License:    MIT
 */

#include <iostream>
#include <time.h>
#include <pthread.h>
#include <vector>

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

#ifndef _FOOD_STAND_DEFS_
#define _FOOD_STAND_DEFS_ 1

#define FOOD_STAND "food_stand"
#define MS_S_MULTIPLIER  1000       // 1 sec = 1'000 millisec
#define NS_MS_MULTIPLIER 1000000    // 1 millisec = 1'000'000 nanosec
#define NS_S_MULTIPLIER  1000000000 // 1 sec = 1'000'000'000 nanosec
#endif

struct food_stand {
	int num_of_dishes;
	pthread_mutex_t *mutex;
};

struct attendant_data {
	int interval_ms;
	food_stand *stand;
};

struct visitor_data {
	int id;
	int min_sleep_ms;
	int max_sleep_ms;
	food_stand *stand;
};

// Globals to save locally defined data such that they are not automatically destroyed
pthread_mutex_t glob_mutex;
vector<pthread_t> thread_store;
bool shutdown = false;
int* eat_count;

void *run_attendant(void *attr) {
	attendant_data *data = (attendant_data*) attr;
	food_stand *stand = data->stand;
	pthread_mutex_t *mutex = stand->mutex;

	timespec delay;
	delay.tv_sec = data->interval_ms / MS_S_MULTIPLIER;
	delay.tv_nsec = (long) (data->interval_ms - (delay.tv_sec * MS_S_MULTIPLIER)) * NS_MS_MULTIPLIER;

	while (!shutdown) {
		pthread_mutex_lock(mutex);

		// fill up again
		stand->num_of_dishes = 10;

		cout << "Attendant is stocking up to 10 dishes." << endl;

		pthread_mutex_unlock(mutex);
		nanosleep(&delay, NULL);
	}

	delete data;
	pthread_exit(NULL);
}

void init_attendant(food_stand *stand, long interval_time_ms) {
	attendant_data *data = new attendant_data();
	data->interval_ms = interval_time_ms;
	data->stand = stand;

	pthread_t attendant;
	if (pthread_create(&attendant, NULL, run_attendant, data) != 0) {
		perror("error in pthread_create() for attendant thread");
		exit(EXIT_FAILURE);
	}
	thread_store.push_back(attendant);
}

void *run_visitor(void *attr) {
	visitor_data *data = (visitor_data*) attr;
	food_stand *stand = data->stand;
	pthread_mutex_t *mutex = stand->mutex;

	while (!shutdown) {
		// check if there is any food, otherwise wait again until there is some food (and do not block the program flow)
		if (stand->num_of_dishes > 0) {
			pthread_mutex_lock(mutex);

			if (stand->num_of_dishes > 0) { // obviously check again if there is food
				stand->num_of_dishes--;
				eat_count[data->id - 1] = eat_count[data->id - 1] + 1;
				cout << "Visitor " << data->id << " just took a food sample (" << stand->num_of_dishes << " samples left)" << endl;
			}

			pthread_mutex_unlock(mutex);
		}

		timespec delay;
		unsigned int diff = data->max_sleep_ms - data->min_sleep_ms;
		int delay_ms = data->min_sleep_ms;
		if (diff != 0) {
			delay_ms += rand() % diff;
		}
		delay.tv_sec = delay_ms / MS_S_MULTIPLIER;
		delay.tv_nsec = (delay_ms - (delay.tv_sec * MS_S_MULTIPLIER)) * NS_MS_MULTIPLIER;
		nanosleep(&delay, NULL);
	}

	delete data;
	pthread_exit(NULL);
}

void init_visitors(food_stand *stand, int num_visitors, long min_sleep_ms, long max_sleep_ms) {

	for (int i = 0; i < num_visitors; i++) {
		// new visitor data for each thread but share the foodstand
		visitor_data *data = new visitor_data();
		data->id = i + 1;
		data->min_sleep_ms = min_sleep_ms;
		data->max_sleep_ms = max_sleep_ms;
		data->stand = stand;

		pthread_t visitor;
		if (pthread_create(&visitor, NULL, run_visitor, data) != 0) {
			perror("error in pthread_create() for visitor thread");
			exit(EXIT_FAILURE);
		}
		thread_store.push_back(visitor);
	}
}

void write_usage() {
	cout << "usage: " << FOOD_STAND << " [num_of_visitors] [stand_attendant_delay_ms] [visitor_sleep_min_ms] [visitor_sleep_max_ms]" << endl;
}

int main(int argc, const char **argv) {
	// Read and validate CLI arguments
	if ((argc < 4) // too few arguments OR
	|| (atoi(argv[1]) <= 0) // no int or smaller-eq zero visitors
			|| (atoi(argv[2]) < 0) // not int or smaller than zero ms delay
			|| (atoi(argv[3]) < 0) // no int or smaller than zero ms min delay
			|| (atoi(argv[4]) <= 0) // no int or smaller-eq ms max delay
			|| (atoi(argv[4]) < atoi(argv[3]))) { // max smaller than min delay
		write_usage();
		return EXIT_FAILURE;
	}

	int num_of_visitors = atoi(argv[1]);
	int attendant_delay = atoi(argv[2]);
	int visitor_min_delay = atoi(argv[3]);
	int visitor_max_delay = atoi(argv[4]);

	// init stand
	food_stand *stand = new food_stand();
	stand->num_of_dishes = 0;
	pthread_mutex_init(&glob_mutex, NULL);
	stand->mutex = &glob_mutex;

	// init stand attendant
	init_attendant(stand, attendant_delay);

	// init visitors
	eat_count = new int[num_of_visitors];
	for (int i = 0; i < num_of_visitors; i++) {
		eat_count[i] = 0; // set every entry to 0
	}
	init_visitors(stand, num_of_visitors, visitor_min_delay, visitor_max_delay);

	// wait for keyboard entry
	cin.ignore();
	shutdown = true;

	// exit and wait for threads
	for (unsigned i = 0; i < thread_store.size(); i++) {
		if (pthread_join(thread_store[i], NULL) != 0) {
			perror("error in pthread_join()");
			exit(EXIT_FAILURE);
		}
	}

	// write results
	cout << "\n" << "Food Stats:" << "\n";
	for (int i = 0; i < num_of_visitors; i++) {
		cout << "Visitor " << i + 1 << " ate " << eat_count[i] << " samples." << "\n";
	}
	cout << endl;

	delete[] eat_count;
	delete stand;

	return 0;
}
