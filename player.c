#include "game.h"


//Specific header for player

//player functions
void * player_f( void *data )
{
	struct playerdata_t *myplayerdata;
	myplayerdata = (struct playerdata_t *) data;
	int retval;

	printf( "NEW PLAYER: %d, game %d, inp read %d, inp write %d, outp read %d, outp write %d, thread id %lu\n", myplayerdata->playerid, myplayerdata->gamenumber, myplayerdata->input[READPIPE], myplayerdata->input[WRITEPIPE], myplayerdata->output[READPIPE], myplayerdata->output[WRITEPIPE], pthread_self() );

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
	//Delay a few seconds until we get while loop finished
	sleep(15);

	close( myplayerdata->input[READPIPE] );
	close( myplayerdata->input[WRITEPIPE] );
	close( myplayerdata->output[READPIPE] );
	close( myplayerdata->output[WRITEPIPE] );

	retval = EXIT_SUCCESS;
	pthread_exit( (void *)(long)retval );
}
