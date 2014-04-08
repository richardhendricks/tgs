#include "game.h"

//Specific header for player

//RAH Will need to add smarter control over adding/removing players
//    Right now all it does is use an array, and it doesn't recover empty
//    or deleted players
int process_game_command( struct playerdata_t *myplayer )
{
	uint8_t commandbuffer[ COMMAND_SIZE ];
	struct player_results_packet *cmd;
	int readsize;
	int currentpacket=0;
	bool moredata = true;

	// read() is not buffered, so it is possible to receive multiple
	// packets of data if we don't get scheduled soon enough.
	// currentpacket points to the start of the current packet in commandbuffer
	// This implementation fails if read returns a partial packet.
	readsize =  read( myplayer->output[READPIPE], commandbuffer, COMMAND_SIZE );
	if ( -1 == readsize ) {
		printf( "Error %d reading in player results packet for player %d\n", errno,  myplayer->playerid );
		return false;
	}

	while( moredata )
	{
		cmd = (struct player_results_packet *)&commandbuffer[ currentpacket ];

		switch( cmd->result )
		{
		case result:
			printf("Results packet:\n%s", (char *)&commandbuffer[ currentpacket + sizeof(struct player_results_packet *) ] );
			break;

		case request:
			break;

		case runplayersim:
			{
				char filenamesimin[BUFFER_SIZE], filenamesimout[BUFFER_SIZE];
				sscanf( (char*)&commandbuffer[ sizeof( struct player_results_packet) ], "%[^:]:%s", filenamesimin, filenamesimout );
				printf( "Player %d simfilein %s simfileout %s\n", myplayer->playerid, filenamesimin, filenamesimout );

				myplayer->simmode=true;
			}
			
			break;

		case terminate:
			printf( "Stopping player %d\n", myplayer->playerid );
			return false;
			break;

		default:
			return false;
			break;
		}

		//Advance to the next packet read
		currentpacket += cmd->datasize + sizeof( struct player_results_packet );
		if( currentpacket >= readsize ) {
			moredata = false;
		}
	}
	return true;
}


//player functions
void * player_f( void *data )
{
	struct playerdata_t *myplayerdata;
	myplayerdata = (struct playerdata_t *) data;
	int retval;
	bool run=true;
  
	int epfd;
	struct epoll_event event_setup;

	epfd = epoll_create ( MAX_EVENTS );

	printf( "NP %d, game %d ip rd %d ip wr %d op rd %d op wr %d epfd %d tid %lu\n", myplayerdata->playerid, myplayerdata->gamenumber, myplayerdata->input[READPIPE], myplayerdata->input[WRITEPIPE], myplayerdata->output[READPIPE], myplayerdata->output[WRITEPIPE], epfd, pthread_self() );

	event_setup.data.fd = myplayerdata->output[READPIPE];
	event_setup.events = EPOLLIN;
	if ( -1 == epoll_ctl( epfd, EPOLL_CTL_ADD, myplayerdata->output[READPIPE], &event_setup ) ) {
		retval = errno; //we return just a success/failure value
		printf( "Error adding server fd to epoll list\n" );
		pthread_exit( (void *) (long)retval );
	}

	while( run )
	{
		int numevents;
		struct epoll_event event_list[ MAX_EVENTS ];

		// Wait forever for an event from the game or the server
//		printf( "Game %d waiting for event\n", mygamedata->gamenumber );
//		Need to figure out if we can add STDIN so player can quit
//		when they want to.
		numevents = epoll_wait( epfd, event_list, MAX_EVENTS, -1 );

		// Save a variable by counting down ( currently MAX_EVENTS
		// is 1) , does it matter if we start at the end?
		// actually it is a horrible way to do this, needs to be redone
		// to use int i instead.
		for( numevents--;numevents >= 0; numevents-- ) {
			if( myplayerdata->output[READPIPE] == event_list[numevents].data.fd ) {
				run = process_game_command( myplayerdata );
			}
		}

	}

	close( myplayerdata->input[READPIPE] );
	close( myplayerdata->input[WRITEPIPE] );
	close( myplayerdata->output[READPIPE] );
	close( myplayerdata->output[WRITEPIPE] );
	myplayerdata->playerid = -1;

	retval = EXIT_SUCCESS;
	pthread_exit( (void *)(long)retval );
}
