#include "game.h"

#include "dll_test.h"

//Pipes are atomic read/write as long as the amount is smaller than PIPE_BUF, with O_NONBLOCK any writes larger than PIPE_BUF will throw an error on write

pthread_t games[max_test_games];
pthread_t players[max_test_players];

// player simulation function
void *player_f(void *);

//Test function for games

void create_game(int gamenum, struct gamedata_t *game_d ) {
	pthread_attr_t attribute;
	int gamep[2];
	int rc;

	//Setup default attribs for new threads
	rc = pthread_attr_init( &attribute );
	if( rc != 0 )
	{
		perror ("Error during game thread attribute creation" );
		exit( EXIT_FAILURE );
	}

	rc = pthread_attr_setstacksize( &attribute, STACK_SIZE );
	if( rc != 0 )
	{
		perror ("Error during game thread stack size change" );
		exit( EXIT_FAILURE );
	}

	game_d->gamenumber = gamenum;
	if( pipe2( gamep, O_NONBLOCK ) != 0 )
	{
		perror ("Error during game pipe creation" );
		exit( EXIT_FAILURE );
	}

	game_d->input = gamep[WRITEPIPE];
	game_d->output = gamep[READPIPE];
	rc = pthread_create( &games[num_games], &attribute, game_f, game_d );
	if( rc != 0 )
	{
		perror ("Error during game thread creation" );
		exit( EXIT_FAILURE );
	}
}

void create_player(const struct gamedata_t gamedata, struct playerdata_t *player_d, int playerid ) {
	pthread_attr_t attribute;
	int playerp[2];
	int32_t rc;
	uint8_t commandbuffer[sizeof(struct playerdata_t) + sizeof(struct command_packet) ];

	//Setup default attribs for new threads
	rc = pthread_attr_init( &attribute );
	if( rc != 0 )
	{
		perror ("Error during player thread attribute creation" );
		exit( EXIT_FAILURE );
	}

	rc = pthread_attr_setstacksize( &attribute, STACK_SIZE );
	if( rc != 0 )
	{
		perror ("Error during player thread stack size change" );
		exit( EXIT_FAILURE );
	}

	player_d->gamenumber = gamedata.gamenumber;
	if( pipe2( playerp, O_NONBLOCK ) != 0 )
	{
		perror ("Error during player pipe creation" );
		exit( EXIT_FAILURE );
	}

	player_d->input = playerp[WRITEPIPE];
	player_d->output = playerp[READPIPE];
	player_d->playerid = playerid;
	rc = pthread_create( &players[num_players], &attribute, player_f, player_d );
	if( rc != 0 )
	{
		perror ("Error during player thread creation" );
		exit( EXIT_FAILURE );
	}

	//notify game about new player
	if ( -1 == write( gamedata.input, commandbuffer, create_add_player_packet( commandbuffer, player_d ) ) )
	{
		perror( "Error trying to write to server input pipe to add a new player\n" );
		exit ( EXIT_FAILURE );
	}

	//Notfiy player to enter simulation mode
	//Need to pass in the player sim file location.
}

int findgame (int gamenum, struct gamedata_t games[], int maxgames )
{
	int i;
	for( i = 0; i < maxgames; i++ ) {
		if( games[i].gamenumber == gamenum ) {
			return i;
		}
	}
	perror( "Could not find gamenum in games array\n" );
	exit( EXIT_FAILURE );
}

//This does minimal bookkeeping for games and players -> each is only used once, ie if a player or game quits the entry isn't reused.
int main(int argc, char *argv[])
{
	int rc;
	int i;
	struct gamedata_t game_data[max_test_games];
	struct playerdata_t player_data[max_test_players];

	char buffer[BUFFER_SIZE];
	FILE *simfile;
	simcommands_t simcommand=-1;
	int gamenum; 
	

	printf( "Executing %s test simulation, pthread id is %lu\n", argv[0], pthread_self() );
	printf( "CTRL-C provides status update, CTRL-\\ terminates\n" );

	setup_signal_handler();

	//Read in the simulation file
	if( argc == 2 )
	{
		sprintf( buffer, "%s", argv[1] );
	} else {
		//default simulation file is found in ./TEST/argc[0]-sim.txt
		sprintf( buffer, "TEST/%s-sim.txt", argv[0] );
	}
	simfile = fopen( buffer, "r" );
	if( simfile == NULL )
	{
		perror( "Error opening simulation file" );
		exit( EXIT_FAILURE );
	}
	
	//Iterate through the simulation lines
	while( (rc = fscanf( simfile, "%d", (int *)&simcommand ) ) != EOF )
	{
		//Check if we failed to convert, and if it was because of a /
		//in the first character, skip this line.  Otherwise bomb
		if( rc == 0 )
		{
			if( ( rc = fgetc(simfile) ) == '/' ) {
				while( fgetc(simfile) != '\n' );
				linecount++;
				continue;
			} else {
				printf( " Error reading the first character of sim line %d, read a %c\n", linecount+1, rc );
				exit( EXIT_FAILURE );
			}
		}

		switch( simcommand )
		{
		case CREATE_GAME:
			if ( fscanf( simfile, ":%d", &gamenum ) != 1 )
			{
				printf( " Error in simfile CREATE_GAME format\n" );
				exit( EXIT_FAILURE );
			}

			create_game( gamenum, &game_data[num_games] );
			num_games++;
			break;
		case ADD_PLAYER:
			if ( fscanf( simfile, ":%d", &gamenum ) != 1 )
			{
				printf( " Error in simfile ADD_PLAYER format\n" );
				exit( EXIT_FAILURE );
			}
			printf( "Adding new player\n" );
			create_player( game_data[findgame( gamenum, game_data, max_test_games )], &player_data[num_players], num_players );
			num_players++;
			break;
		case STOP_GAME:
			{
			uint8_t commandbuffer[ BUFFER_SIZE ];

			if ( fscanf( simfile, ":%d", &gamenum ) != 1 ) {
				printf( " Error in simfile STOP_GAME format\n" );
				exit( EXIT_FAILURE );
			}
			printf( "Stopping game %d\n", gamenum );

			if ( -1 == write( game_data[ findgame( gamenum, game_data, max_test_games ) ].input, commandbuffer, create_stop_game_packet( commandbuffer ) ) ) {
				perror (" Error writing to game input pipe\n" );
				exit( EXIT_FAILURE );
			}

			}
			break;
		case WAIT_TIME:
			{
			int delay;
			if ( fscanf( simfile, ":%d", &delay ) != 1 )
			{
				printf( " Error in simfile WAIT_TIME format\n" );
				exit( EXIT_FAILURE );
			}
			printf( "Delaying for %d seconds\n", delay );
			while(  delay != 0 ) {
				delay = sleep(delay);
			}
			}
			break;
		case QUIT_TEST:
			goto cleanup;
		default:
			printf( " Invalid sim command \n" );
			exit( EXIT_FAILURE );
		}
		//Read the rest of the line and throw it away
		while( fgetc(simfile) != '\n' );
		linecount++;
		simcommand=-1;
	}

cleanup:
	//Wait for all threads to complete
	for( i = 0; i < num_games; i++ ) {
		pthread_join( games[i], (void *)&rc );
		printf( "Child thread complete, returned %d\n", (int) rc);
	}
	for( i = 0; i < num_players; i++ ) {
		pthread_join( players[i], (void *)&rc );
		printf( "Child thread complete, returned %d\n", (int) rc);
	}

	exit( EXIT_SUCCESS );
}
