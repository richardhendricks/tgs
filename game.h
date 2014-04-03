//generic header for all game types
#ifndef GAME_H
#define GAME_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <errno.h> 

#include <sys/epoll.h>
#include <sys/types.h>

#include "player.h"
#include "handler.h"
#include "packet.h"

#define STACK_SIZE (64 * 1024)
#define BUFFER_SIZE (512)

#define READPIPE (0)
#define WRITEPIPE (1)

//default value is 10, for custom value #undef and #define in game-specific header file
#define MAX_PLAYERS 10

struct gamedata_t {
	int gamenumber;
	int input; 	// file descriptor for data going to game thread
	int output;	// file descriptor for data sent from game thread
	char gamename[BUFFER_SIZE];
	int players[MAX_PLAYERS]; //reference to array of players on this game
	pthread_mutex_t mutex;
};

void *game_f( void * );

#endif
