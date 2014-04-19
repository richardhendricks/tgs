#include "game.h"

//Specific header for player

//RAH Will need to add smarter control over adding/removing players
//    Right now all it does is use an array, and it doesn't recover empty
//    or deleted players
int process_game_result( struct playerdata_t *myplayer )
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
		case result: //Just display data to the screen
			printf( "Player %d result packet\n", myplayer->playerid );
			if( true == myplayer->simmode ) {
				fprintf( myplayer->simfileout, "Results packet:\n%s", (char *)&commandbuffer[ currentpacket + sizeof(struct player_results_packet *) ] );
			}
			break;

		case request: //Display data and then wait for player response
			printf( "Player %d request packet\n", myplayer->playerid );
			if( true == myplayer->simmode ) {
				char simcommand[BUFFER_SIZE];
				uint8_t gamecommand[BUFFER_SIZE];
				int retval;

				fprintf( myplayer->simfileout, "Request packet:\n%s", (char *)&commandbuffer[ currentpacket + sizeof(struct player_results_packet *) ] );
				retval = fscanf( myplayer->simfilein, "%s\n", simcommand );
				//If the player simfile is empty
				if( EOF == retval ) {
					printf( "End of player simfile\n");
					//Special player simfile command -1 to quit game
					if( -1 == write( myplayer->input[WRITEPIPE], gamecommand, create_quit_game_packet( gamecommand ) ) ) {
						perror( "Error trying to send quit game packet\n" );
						exit( EXIT_FAILURE );
					}
				} else {
					//Check for special -1 command to quit game
					if( strcmp( "-1", (char *)simcommand ) == 0 )
					{
						if( -1 == write( myplayer->input[WRITEPIPE], gamecommand, create_quit_game_packet( gamecommand ) ) ) {
							perror( "Error trying to send quit game packet\n" );
							exit( EXIT_FAILURE );
						}
					} else {
						//otherwise send command to server
						if( -1 == write( myplayer->input[WRITEPIPE], gamecommand, create_gamecommand_packet( gamecommand, simcommand ) ) ) {
							perror( "Error trying to send gamecommand packet\n" );
							exit( EXIT_FAILURE );
						}
					}
				}
			}
			break;

		case runplayersim: //Switch to SIM mode
			{
				char filenamesimin[BUFFER_SIZE], filenamesimout[BUFFER_SIZE];
				sscanf( (char*)&commandbuffer[ currentpacket + sizeof( struct player_results_packet) ], "%[^:]:%s", filenamesimin, filenamesimout );
				printf( "Player %d runplayersim\n", myplayer->playerid );
				myplayer->simfilein = fopen( filenamesimin, "r" );
				myplayer->simfileout = fopen( filenamesimout, "w" );
				if( NULL == myplayer->simfilein || NULL == myplayer->simfileout ) {
					perror( "Error opening player sim files\n" );
					exit( EXIT_FAILURE );
				}
				myplayer->simmode=true;
			}
			
			break;

		case runplayer:
			//Not implemented yet
			return false;
			break;

		case terminate: //Terminate this player thread
			printf( "Stopping player %d\n", myplayer->playerid );
			if( true == myplayer->simmode ) {
				fprintf( myplayer->simfileout, "Terminate packet\n" );
			}
			return false;
			break;

		default: //Uhoh, don't know what to do, quit
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

void player_cleanup( struct playerdata_t *myplayer )
{
	close( myplayer->input[READPIPE] );
	close( myplayer->input[WRITEPIPE] );
	close( myplayer->output[READPIPE] );
	close( myplayer->output[WRITEPIPE] );
	close( myplayer->epfd );

	if( true == myplayer->simmode ) {
		fclose( myplayer->simfileout );
		fclose( myplayer->simfilein );
	}

	myplayer->playerid = -1;
}

//player functions
void * player_f( void *data )
{
	struct playerdata_t *myplayerdata;
	myplayerdata = (struct playerdata_t *) data;
	int retval;
	bool run=true;
  
	struct epoll_event event_setup;

	//Will need to add socket related info at some point.  Will player threads be remote?
	myplayerdata->epfd = epoll_create ( MAX_EVENTS );

	//printf( "NP %d, game %d ip rd %d ip wr %d op rd %d op wr %d epfd %d tid %lu\n", myplayerdata->playerid, myplayerdata->gamenumber, myplayerdata->input[READPIPE], myplayerdata->input[WRITEPIPE], myplayerdata->output[READPIPE], myplayerdata->output[WRITEPIPE], myplayerdata->epfd, pthread_self() );
	printf( "New Player %d game %d\n", myplayerdata->playerid, myplayerdata->gamenumber );

	event_setup.data.fd = myplayerdata->output[READPIPE];
	event_setup.events = EPOLLIN;
	if ( -1 == epoll_ctl( myplayerdata->epfd, EPOLL_CTL_ADD, myplayerdata->output[READPIPE], &event_setup ) ) {
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
		numevents = epoll_wait( myplayerdata->epfd, event_list, MAX_EVENTS, -1 );

		// Save a variable by counting down ( currently MAX_EVENTS
		// is 1) , does it matter if we start at the end?
		// actually it is a horrible way to do this, needs to be redone
		// to use int i instead.
		for( numevents--;numevents >= 0; numevents-- ) {
			if( myplayerdata->output[READPIPE] == event_list[numevents].data.fd ) {
				run = process_game_result( myplayerdata );
			}
		}

	}

	player_cleanup( myplayerdata );
	retval = EXIT_SUCCESS;
	pthread_exit( (void *)(long)retval );
}
