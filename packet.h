//generic header for all game types
#ifndef PACKET_H
#define PACKET_H

typedef enum e_server_commands { stop_game=0, add_player, report_status} server_commands_t;

struct command_packet {
	server_commands_t command;
	int datasize;
	// actual data follows packet header
};

typedef enum e_server_results { ok = 0, error_game_full, status_report } server_results_t;

struct result_packet {
	server_results_t result;
	int datasize;
	// actual data follows packet header
};

typedef enum e_player_commands { quit_game = 0, status, join, watch, gamecommand } player_commands_t;

struct player_command_packet{
	player_commands_t command;
	int datasize;
	// actual data follows packet header
};

typedef enum e_player_results { result=0, request, runplayersim, terminate } player_results_t;

struct player_results_packet{
	player_results_t result;
	int datasize;
	// actual data follows packet header
};

int create_add_player_packet( uint8_t *, struct playerdata_t *);
int create_stop_game_packet( uint8_t * );

#endif