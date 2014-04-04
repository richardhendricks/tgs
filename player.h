//generic header for all game types
#ifndef PLAYER_H
#define PLAYER_H

//default value is 10, for custom value #undef and #define in game-specific header file
//#define MAX_PLAYERS 10

// playerdata_t can't contain any pointers, since we are directly providing it to the game
struct playerdata_t {
	int gamenumber;
	int playerid; //index into file with player info, password, etc
	int input[2]; 	// file descriptor for data going to game thread
	int output[2];	// file descriptor for data sent from game thread
};

#endif
