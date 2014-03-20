HDRS := gameserver.h game.h handler.h dll_test.h

OBJS := tictactoe_dl.so libhandler.so.1.0 libhandler.so libhandler.so.1 dll_test.o player.o server

CFLAGS:= -O2 -std=gnu99 -Wall -pedantic -fPIC

OBJCFLAGS := $(CFLAGS) -c
LIBCFLAGS := $(CFLAGS) -shared -lc
DLLCFLAGS := $(CFLAGS) -shared -pie -pthread -L. -lhandler -Wl,-E,-rpath=.

all: clear $(OBJS)

cleanall: clean clear $(OBJS)

clear:
	clear

libhandler.so.1.0: handler.c $(HDRS)
	gcc -O2 -std=gnu99 -Wall -pedantic -fPIC -shared -Wl,-soname=libhandler.so.1 handler.c -o libhandler.so.1.0 -lc 
	ln -s libhandler.so.1.0 libhandler.so
	ln -s libhandler.so.1.0 libhandler.so.1

player.o: player.c $(HDRS)
	gcc $(OBJCFLAGS) -o player.o player.c
dll_test.o: dll_test.c player.o $(HDRS)
	gcc $(OBJCFLAGS) -o dll_test.o dll_test.c

tictactoe_dl.so: tictactoe.c libhandler.so.1.0 dll_test.o $(HDRS)
	gcc -O2 -std=gnu99 -Wall -pedantic -fPIC -shared -pie -pthread -o tictactoe_dl.so dll_test.o player.o tictactoe.c -L. -lhandler -Wl,-E,-rpath=.

server: server.c $(HDRS)
	gcc -O2 -Wall -pedantic -pthread -o server server.c -ldl

clean:
	rm -f $(OBJS)

