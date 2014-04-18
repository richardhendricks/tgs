//generic header for all game types
#ifndef PLAYER_H
#define PLAYER_H

//default value is 10, for custom value #undef and #define in game-specific header file
//#define MAX_PLAYERS 10

enum playerstatus_e { init=0, joining, waiting, watching, turncomplete, gameover };

struct playerstate_t {
	enum playerstatus_e status;
};
// playerdata_t can't contain any pointers used outside of the game, since we are directly providing it to the game
struct playerdata_t {
	int gamenumber;
	int playerid; 	//index into file with player info, password, etc
	int input[2]; 	// file descriptor for data going to game thread
	int output[2];	// file descriptor for data sent from game thread
	int epfd;		// epoll file descriptor
	int socket; // file pointer for socket 
	FILE *simfilein; // file pointer for sim file input
	FILE *simfileout; // file pointe rfor sim file output
	bool simmode;
	struct playerstate_t state;
};

void *player_f( void *);

#endif
