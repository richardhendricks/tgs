#include "handler.h"
//Intercept SIGINT and SIGQUIT to display information
//
extern int num_games;
extern int num_players;
extern int linecount;

void handler( int signum )
{
	switch( signum ) {
		case SIGINT:
			printf( "Currently on line %d of sim file\n", linecount );
			printf( "Processed %d game(s) and %d player(s)\n", num_games, num_players );
			break;
		case SIGQUIT:
			printf( " Terminating \n" );
			exit( EXIT_SUCCESS );
			break;
		default:
			printf( " huh? Unknown signal\n" );
			exit( EXIT_FAILURE );
			break;
	}
}

void setup_signal_handler()
{
	int rc;
	sigset_t mymask;
	struct sigaction myaction;

	rc = sigemptyset( &mymask );
	if( rc != 0 )
	{
		perror( "Error, could not create signal mask!\n" );
	}

	myaction.sa_handler = handler;
	myaction.sa_mask = mymask;
	myaction.sa_flags = 0;

	sigaction( SIGINT, &myaction, NULL );
	sigaction( SIGQUIT, &myaction, NULL );
}
