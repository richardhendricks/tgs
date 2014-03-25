//Specific header for dll_test

#ifndef DLL_TEST_H
#define DLL_TEST_H

#include <fcntl.h>
#include <unistd.h>
#include <time.h>

#define max_test_games 3
#define max_test_players 10

int num_games;
int num_players;
int linecount;

typedef enum e_simcommands { CREATE_GAME=1, ADD_PLAYER, KILL_GAME, WAIT_TIME } simcommands_t;

#define _GNU_SOURCE

int pipe2(int pipefd[2], int flags );

#endif
