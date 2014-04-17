#include "game.h"


//Specific header for game

int process_player_command( struct gamedata_t *mygamedata, struct playerdata_t *myplayers )
{
	uint8_t commandbuffer[ COMMAND_SIZE ];
	struct player_command_packet *cmd;
	int readsize;
	int currentpacket=0;
	bool moredata = true;

	// read() is not buffered, so it is possible to receive multiple
	// packets of data if we don't get scheduled soon enough.
	// currentpacket points to the start of the current packet in commandbuffer
	// This implementation fails if read returns a partial packet.
	readsize =  read( mygamedata->input[READPIPE], commandbuffer, COMMAND_SIZE );
	if ( -1 == readsize ) {
		printf( "Error %d reading in server command packet for game %d\n", errno,  mygamedata->gamenumber );
		return false;
	}

	while( moredata )
	{
		cmd = (struct player_command_packet *)&commandbuffer[ currentpacket ];

		switch( cmd->command )
		{
		case quit_game:
			printf( "Player  is quitting game %d\n", mygamedata->gamenumber );
			break;

		case status:
			printf( "Reporting status to player \n "  );
			break;

		case join:
			printf( "Player joining game %d\n", mygamedata->gamenumber );
			break;

		case watch:
			printf( "Player watching game %d\n", mygamedata->gamenumber );
			break;

		case gamecommand:
			printf( "Player sending command to game %d\n", mygamedata->gamenumber );
			break;

		default:
			printf( "Unknown command for game %d\n", mygamedata->gamenumber );
			return false;
			break;
		}

		//Advance to the next packet read
		currentpacket += cmd->datasize + sizeof( struct command_packet );
		if( currentpacket >= readsize ) {
			moredata = false;
		}
	}
	return true;
}

//RAH Will need to add smarter control over adding/removing players
//    Right now all it does is use an array, and it doesn't recover empty
//    or deleted players
int process_server_command( struct gamedata_t *mygamedata, struct playerdata_t *myplayers )
{
	uint8_t commandbuffer[ COMMAND_SIZE ];
	struct command_packet *cmd;
	int readsize;
	int currentpacket=0;
	bool moredata = true;
	struct epoll_event event_setup;

	// read() is not buffered, so it is possible to receive multiple
	// packets of data if we don't get scheduled soon enough.
	// currentpacket points to the start of the current packet in commandbuffer
	// This implementation fails if read returns a partial packet.
	readsize =  read( mygamedata->input[READPIPE], commandbuffer, COMMAND_SIZE );
	if ( -1 == readsize ) {
		printf( "Error %d reading in server command packet for game %d\n", errno,  mygamedata->gamenumber );
		return false;
	}

	while( moredata )
	{
		cmd = (struct command_packet *)&commandbuffer[ currentpacket ];

		switch( cmd->command )
		{
		case stop_game:
			printf( "Stopping game %d\n", mygamedata->gamenumber );
			return false;
			break;

		case add_player:
			if( mygamedata->numplayers == MAX_PLAYERS ) {
				printf( "Error, game %d is full.\n", mygamedata->gamenumber );
				return false;
			}
			memcpy( (void*) &myplayers[mygamedata->numplayers], (void*) &commandbuffer[ sizeof(struct command_packet) + currentpacket ], cmd->datasize );

			printf( "Game %d adding player %d\n", mygamedata->gamenumber, myplayers[mygamedata->numplayers].playerid );

/////RAH STOP HERE
			event_setup.data.u64 = ;
			event_setup.events = EPOLLIN;
			if ( -1 == epoll_ctl( mygamedata->epfd, EPOLL_CTL_ADD, xxxmygamedata->input[READPIPE], &event_setup ) ) {
		retval = errno; //we return just a success/failure value
		printf( "Error adding server fd to epoll list\n" );
		pthread_exit( (void *) (long)retval );
	}


			mygamedata->numplayers++;
			break;

		case report_status:
			printf( "Reporting status for game %d\n", mygamedata->gamenumber );
			printf( "Number of players: %d\n", mygamedata->numplayers );
			int i;
			for( i = 0; i < mygamedata->numplayers; i++ ) {
				printf(" Player %d id %d\n", i, myplayers[ i ].playerid );
			}
			break;

		default:
			printf( "Unknown command for game %d\n", mygamedata->gamenumber );
			return false;
			break;
		}

		//Advance to the next packet read
		currentpacket += cmd->datasize + sizeof( struct command_packet );
		if( currentpacket >= readsize ) {
			moredata = false;
		}
	}
	return true;
}

void game_cleanup( struct gamedata_t *mygamedata, struct playerdata_t *myplayers )
{
	uint8_t commandbuffer[ COMMAND_SIZE ];

	close( mygamedata->input[READPIPE] );
	close( mygamedata->input[WRITEPIPE] );
	close( mygamedata->output[READPIPE] );
	close( mygamedata->output[WRITEPIPE] );
	close( mygamedata->epfd );

	for( int i = 0; i < mygamedata->numplayers; i++ ){
		if ( -1 == write( myplayers[i].output[WRITEPIPE], commandbuffer, create_terminate_packet( commandbuffer ) ) ) {
			perror( "Error trying to write to game input pipe to enter sim mode\n" );
			exit ( EXIT_FAILURE );
		}
	}
	free( myplayers );
	mygamedata->numplayers=0;
	mygamedata->gamenumber = -1;
}



//Game functions
void * game_f( void *data )
{
	struct gamedata_t *mygamedata;
	mygamedata = (struct gamedata_t *) data;
	struct playerdata_t *myplayers;
	bool run=true;
	int retval;
  
	struct epoll_event event_setup;

	myplayers = malloc( sizeof (struct playerdata_t) * MAX_PLAYERS );
	for( int i = 0; i < MAX_PLAYERS; i++ ){
		myplayers[i].playerid = -1;
	}
	mygamedata->numplayers=0;

	// (MAX_PLAYERS+1) because of the extra pipe to talk to the server
	mygamedata->epfd = epoll_create ( MAX_PLAYERS + 1 );

	printf( "NG %d ip rd %d ip wr %d op rd %d op wr %d epfd %d tid %lu\n", mygamedata->gamenumber, mygamedata->input[READPIPE], mygamedata->input[WRITEPIPE], mygamedata->output[READPIPE], mygamedata->output[WRITEPIPE], mygamedata->epfd, pthread_self() );

	//Setting bit 63 == this is a server command
	event_setup.data.u64 = SRV_CMD;
	event_setup.events = EPOLLIN;
	if ( -1 == epoll_ctl( mygamedata->epfd, EPOLL_CTL_ADD, mygamedata->input[READPIPE], &event_setup ) ) {
		retval = errno; //we return just a success/failure value
		printf( "Error adding server fd to epoll list\n" );
		pthread_exit( (void *) (long)retval );
	}

	while( run )
	{
		int numevents;
		struct epoll_event event_list[MAX_EVENTS];

		// Wait forever for an event from a player or the server
		// Later, for a dyamic game that has timed updates, the 
		// -1 is replaced with how many ms to wait
//		printf( "Game %d waiting for event\n", mygamedata->gamenumber );
		numevents = epoll_wait( mygamedata->epfd, event_list, MAX_EVENTS, -1 );

		// Save a variable by counting down ( currently MAX_EVENTS
		// is 1) , does it matter if we start at the end?
		for( numevents--;numevents >= 0; numevents-- ) {
			if( event_list[numevents].data.u64 == ( SRV_CMD ) ) {
				run = process_server_command( mygamedata, myplayers );
			}
			
			//Process player command
		}

	}

	game_cleanup( mygamedata, myplayers );

	retval = EXIT_SUCCESS;
	pthread_exit( (void *) (long)retval );
}
