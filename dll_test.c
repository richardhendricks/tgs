#include "game.h"

#include "dll_test.h"

//Pipes are atomic read/write as long as the amount is smaller than PIPE_BUF, with O_NONBLOCK any writes larger than PIPE_BUF will throw an error on write

pthread_t games[max_test_games];
pthread_t players[max_test_players];

// player simulation function
void *player_f(void *);

//Test function for games

void create_game(int gamenum, struct gamedata_t *game_d, int gamep[2] ) {
	pthread_attr_t attribute;
	int rc;
	struct timespec delay;

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
	rc = pthread_create( &games[num_games], &attribute, server_f, game_d );
	if( rc != 0 )
	{
		perror ("Error during game thread creation" );
		exit( EXIT_FAILURE );
	}
	delay.tv_sec = 0;
	delay.tv_nsec = 10000;
	//nanosleep(&delay, NULL );

}

//
//This does minimal bookkeeping for games and players -> each is only used once, ie if a player or game quits the entry isn't reused.
int main(int argc, char *argv[])
{
	int rc;
	int i;
	pthread_attr_t attribute;
	struct gamedata_t game_data[max_test_games];
	struct playerdata_t player_data[max_test_players];
	int gamepipes[max_test_games][2];
	int playerpipes[max_test_games][2];

	char buffer[BUFFER_SIZE];
	FILE *simfile;
	simcommands_t simcommand=-1;
	

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
			{
			int gamenum; 
			if ( fscanf( simfile, ":%d", &gamenum ) != 1 )
			{
				printf( " Error in simfile CREATE_GAME format\n" );
				exit( EXIT_FAILURE );
			}

			create_game( gamenum, &game_data[num_games], gamepipes[num_games] );
			num_games++;
			}
			break;
		case ADD_PLAYER:
			printf( "Adding new player\n" );
			num_players++;
			break;
		case KILL_GAME:
			printf( "Killing game\n" );
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

			exit( EXIT_SUCCESS );
			}
			break;
		default:
			printf( " Invalid sim command \n" );
			exit( EXIT_FAILURE );
		}
		//Read the rest of the line and throw it away
		while( fgetc(simfile) != '\n' );
		linecount++;
		simcommand=-1;
	}
	//Create a game with a specific game number
	//Add a player to a game
	//  - player thread uses separate simulation file
	//  - watchers are setup by they player thread input file
	//kill a game with a specific game number

	player_data[num_players].gamenumber=0;
	if( pipe2( playerpipes[num_players], O_NONBLOCK ) != 0 )
	{
		perror( "Error during player pipe creation" );
		exit( EXIT_FAILURE );
	}
	player_data[num_players].input = playerpipes[num_players][WRITEPIPE];
	player_data[num_players].output = playerpipes[num_players][READPIPE];
	rc = pthread_create( &players[num_players], &attribute, player_f, &player_data[num_players] );
	if( rc != 0 ) {
		perror( "Error during player thread creation" );
		exit( EXIT_FAILURE );
	}
	num_players++;

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
