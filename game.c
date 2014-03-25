#include "game.h"


//Specific header for game

//Game functions
void * server_f( void *data )
{
	struct gamedata_t *mygamedata;
	mygamedata = (struct gamedata_t *) data;
	int retval;

	printf( "Inside the game thread\n" );
	printf( "My gamenumber is %d, my input number is %d, my output number is %d, my thread id is %lu\n", mygamedata->gamenumber, mygamedata->input, mygamedata->output, pthread_self() );

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
	pthread_exit( (void *) (long)retval );
}
