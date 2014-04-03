#include "game.h"


//Specific header for game

// Need to add a little extra to account for packets
#define COMMAND_SIZE ( BUFFER_SIZE + sizeof (struct command_packet ) )

//RAH STOP HERE
//void process_server_command( 


//Game functions
void * game_f( void *data )
{
	struct gamedata_t *mygamedata;
	mygamedata = (struct gamedata_t *) data;
	int retval;
	bool run=true;

	int epfd;

	uint8_t commandbuffer[ COMMAND_SIZE ];

	printf( "NEW GAME: gamenumber %d, input %d, output %d, thread id %lu\n", mygamedata->gamenumber, mygamedata->input, mygamedata->output, pthread_self() );

	// Maximum file descriptors we will watch is  (MAX_PLAYERS+1) because of the extra pipe to talk to the server
	epfd = epoll_create ( MAX_PLAYERS + 1 );
	while( run )
	{
		struct command_packet *cmd;
		uint8_t *data;

		if ( -1 == read( mygamedata->output, commandbuffer, COMMAND_SIZE ) ) {
			//REMOVE THIS IF STATEMENT WHEN EPOLL IMPLEMENTED
			if( errno == EAGAIN ) {
				sleep(1);
				continue;
			}
				
			char buf[ BUFFER_SIZE ];
			sprintf( buf, "Error reading in server command packet for game %d\n", mygamedata->gamenumber );
			perror( buf );
			exit( EXIT_SUCCESS );
		}

		printf( "command received for game %d\n", mygamedata->gamenumber );
		cmd = (struct command_packet *)commandbuffer;

		switch( cmd->command )
		{
		case stop_game:
			printf( "Quitting game %d\n", mygamedata->gamenumber );
			run = false;
			break;
		case add_player:
			break;
		case report_status:
			break;
		default:
			break;
		}
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
	pthread_exit( (void *) (long)retval );
}
