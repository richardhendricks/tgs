#include "game.h"


//Specific header for player

//player functions
void * player_f( void *data )
{
	struct playerdata_t *myplayerdata;
	myplayerdata = (struct playerdata_t *) data;
	int retval;

	printf( "NEW PLAYER: gamenumber %d, playerid %d, input %d, output %d, thread id %lu\n", myplayerdata->gamenumber, myplayerdata->playerid, myplayerdata->input, myplayerdata->output, pthread_self() );

	//while( 1 )
	{
		//Read the pipe, wait until there is data for us
		//
		//Check the command packet
		//switch based on the command
		//
		//quit game = notify player threads and terminate
		//
		//add player = Use provided pipe to talk to a player thread
		//
		//report status from game thread and print

	}
	retval = EXIT_SUCCESS;
	sleep(15);
	pthread_exit( (void *)(long)retval );
}
