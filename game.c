#include "game.h"


//Specific header for game

// Need to add a little extra to account for packets
#define COMMAND_SIZE ( BUFFER_SIZE + sizeof (struct command_packet ) )

//RAH STOP HERE
//int process_server_command( 


//int process_player_command( 

//Game functions
void * game_f( void *data )
{
	struct gamedata_t *mygamedata;
	mygamedata = (struct gamedata_t *) data;
	int retval; //we return just a success/failure value
	bool run=true;

	// (MAX_PLAYERS+1) because of the extra pipe to talk to the server
	int epfd;
	struct epoll_event events[MAX_PLAYERS + 1 ];

	uint8_t commandbuffer[ COMMAND_SIZE ];

	printf( "NEW GAME: %d, inp read %d, inp write %d, outp read %d, outp write %d, thread id %lu\n", mygamedata->gamenumber, mygamedata->input[READPIPE], mygamedata->input[WRITEPIPE], mygamedata->output[READPIPE], mygamedata->output[WRITEPIPE], pthread_self() );

	epfd = epoll_create ( MAX_PLAYERS + 1 );

	//Setting bit 63 == this is a server command
	events[0].data.u64 = 1UL << 63;
	if ( -1 == epoll_ctl( epfd, EPOLL_CTL_ADD, mygamedata->input[READPIPE], &events[0] ) ) {
		retval = errno;
		printf( "Error adding server fd to epoll list\n" );
		pthread_exit( (void *) (long)retval );
	}

	while( run )
	{
		struct command_packet *cmd;
		uint8_t *data;

		if ( -1 == read( mygamedata->input[READPIPE], commandbuffer, COMMAND_SIZE ) ) {
			//REMOVE THIS IF STATEMENT WHEN EPOLL IMPLEMENTED
			if( errno == EAGAIN ) {
				errno = 0;
				sleep(1);
				continue;
			}
				
			retval = errno;
			printf( "Error reading in server command packet for game %d\n", mygamedata->gamenumber );
			pthread_exit( (void *) (long)retval );
		}

		printf( "command received for game %d\n", mygamedata->gamenumber );
		cmd = (struct command_packet *)commandbuffer;

		switch( cmd->command )
		{
		case stop_game:
			printf( "Stopping game %d\n", mygamedata->gamenumber );
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

	close( epfd );
	close( mygamedata->input[READPIPE] );
	close( mygamedata->input[WRITEPIPE] );
	close( mygamedata->output[READPIPE] );
	close( mygamedata->output[WRITEPIPE] );

	retval = EXIT_SUCCESS;
	pthread_exit( (void *) (long)retval );
}
