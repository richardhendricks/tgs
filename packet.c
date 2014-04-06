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

int create_runplayersim_packet( uint8_t *commandbuffer, char* simfilename )
{
	struct player_results_packet header;

	header.result = runplayersim;
	header.datasize = strlen( simfilename );
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

