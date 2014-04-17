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

// Need to add a little extra to account for packets
#define COMMAND_SIZE ( BUFFER_SIZE + sizeof (struct command_packet ) )


#define READPIPE (0)
#define WRITEPIPE (1)

//default value is 10, for custom value #undef and #define in game-specific header file, this includes spectators
#define MAX_PLAYERS 10

//Currently we only allow for 1 event at a time.
#define MAX_EVENTS 1

#define SRV_CMD (1UL << 63)

struct gamedata_t {
	int gamenumber;
	int input[2]; 	// pipe file descriptor for data to game thread
	int output[2];	// pipe file descriptor for data from game thread
	int epfd; 		// file descriptor used for epoll
	int numplayers;
	pthread_mutex_t mutex;
};

void *game_f( void * );

#endif
