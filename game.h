//generic header for all game types
#ifndef GAME_H
#define GAME_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pthread.h>
#include "handler.h"

#define STACK_SIZE (64 * 1024)
#define BUFFER_SIZE (512)

#define READPIPE (0)
#define WRITEPIPE (1)

struct gamedata_t {
	int gamenumber;
	int input; 	// file descriptor for data going to game thread
	int output;	// file descriptor for data sent from game thread
	pthread_mutex_t mutex;
};

struct playerdata_t {
	int gamenumber;
	int input; 	// file descriptor for data going to game thread
	int output;	// file descriptor for data sent from game thread
	char *infile;   // These must be malloc'd in the caller, or else it might get tromped on.
	char *outfile;
	pthread_mutex_t mutex;
};

typedef enum e_server_commands { stop_game=0, add_player, add_watcher, report_status} server_commands_t;

struct command_packet {
	server_commands_t command;
	int datasize;
	// actual data follows packet header
};

typedef enum e_server_results { ok = 0, error_game_full, status_report } server_results_t;

struct result_packet {
	server_results_t result;
	int datasize;
	// actual data follows packet header
};

typedef enum e_player_commands { quit_game = 0, status, move } player_commands_t;

struct player_command_packet{
	player_commands_t command;
	int datasize;
	// actual data follows packet header
};

typedef enum e_player_results { result=0, terminate } player_results_t;

struct player_results_packet{
	player_results_t result;
	int datasize;
	// actual data follows packet header
};

void *server_f( void * );

#endif
