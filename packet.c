#include "game.h"

int create_stop_game_packet( uint8_t *commandbuffer )
{
	struct command_packet header;

	header.command = stop_game;
	header.datasize = 0;
	memcpy( (void*)commandbuffer, (void*)&header, sizeof( struct command_packet ) );
	return sizeof(struct command_packet );
}

int create_status_game_packet( uint8_t *commandbuffer )
{
	struct command_packet header;

	header.command = report_status;
	header.datasize = 0;
	memcpy( (void*)commandbuffer, (void*)&header, sizeof( struct command_packet ) );
	return sizeof(struct command_packet );
}

int create_add_player_packet( uint8_t *commandbuffer, struct playerdata_t *player_d  )
{
	struct command_packet header;

	header.command = add_player;
	header.datasize = sizeof ( struct playerdata_t );
	memcpy( (void*)commandbuffer, (void*)&header, sizeof( struct command_packet ) );
	memcpy( (void*)&commandbuffer[sizeof (struct command_packet) ], (void*)player_d, sizeof( struct playerdata_t ) );

	return (sizeof(struct command_packet) + sizeof(struct playerdata_t ) );
}



int create_quit_game_packet( uint8_t *commandbuffer )
{
	struct player_command_packet header;

	header.command = quit_game;
	header.datasize = 0;
	memcpy( (void*) commandbuffer, (void*) &header, sizeof( struct player_command_packet ) );

	return sizeof(struct player_command_packet);
}

int create_gamecommand_packet( uint8_t *commandbuffer, char* s )
{
	struct player_command_packet header;

	header.command = gamecommand;
	header.datasize = strlen( s ) + 1; //Add one for \0
	memcpy( (void*) commandbuffer, (void*) &header, sizeof( struct player_command_packet ) );
	memcpy( (void*) &commandbuffer[sizeof (struct player_command_packet) ], (void *) s, header.datasize );

	return (sizeof(struct player_results_packet) + header.datasize );
}


int create_result_packet( uint8_t *commandbuffer, char* s )
{
	struct player_results_packet header;

	header.result = result;
	header.datasize = strlen( s ) + 1; //Add one for \0
	memcpy( (void*) commandbuffer, (void*) &header, sizeof( struct player_results_packet ) );
	memcpy( (void*) &commandbuffer[sizeof (struct player_results_packet) ], (void *) s, header.datasize );

	return (sizeof(struct player_results_packet) + header.datasize );
}

int create_request_packet( uint8_t *commandbuffer, char* s )
{
	struct player_results_packet header;

	header.result = request;
	header.datasize = strlen( s ) + 1; //Add one for \0
	memcpy( (void*) commandbuffer, (void*) &header, sizeof( struct player_results_packet ) );
	memcpy( (void*) &commandbuffer[sizeof (struct player_results_packet) ], (void *) s, header.datasize );

	return (sizeof(struct player_results_packet) + header.datasize );
}

int create_runplayersim_packet( uint8_t *commandbuffer, char* simfilename )
{
	struct player_results_packet header;

	header.result = runplayersim;
	header.datasize = strlen( simfilename ) + 1; //Add one for \0
	memcpy( (void*) commandbuffer, (void*) &header, sizeof( struct player_results_packet ) );
	memcpy( (void*) &commandbuffer[sizeof (struct player_results_packet) ], (void *) simfilename, header.datasize );

	return (sizeof(struct player_results_packet) + header.datasize );
}

int create_terminate_packet( uint8_t *commandbuffer )
{
	struct player_results_packet header;

	header.result = terminate;
	header.datasize = 0;

	memcpy( (void*)commandbuffer, (void*)&header, sizeof( struct player_results_packet ) );
	return sizeof(struct player_results_packet );
}

