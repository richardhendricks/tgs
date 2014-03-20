#ifndef HANDLER_H
#define HANDLER_H

#include <signal.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

void handler( int );
void setup_signal_handler();

#endif
