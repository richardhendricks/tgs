#include "game.h"

int create_stop_game_packet( uint8_t *commandbuffer )
{
	struct command_packet header;

	header.command = stop_game;
	header.datasize = sizeof ( struct playerdata_t );
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
