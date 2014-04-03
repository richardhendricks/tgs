HDRS := gameserver.h game.h handler.h dll_test.h

OBJS := game_dl.so libhandler.so.1.0 libhandler.so libhandler.so.1 dll_test.o player.o packet.o server.out

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

libhandler.so.1: libhandler.so.1.0
	rm -f libhandler.so.1
	ln -s libhandler.so.1.0 libhandler.so.1

libhandler.so: libhandler.so.1.0 libhandler.so.1
	rm -f libhandler.so
	ln -s libhandler.so.1.0 libhandler.so

player.o: player.c $(HDRS)
	gcc $(OBJCFLAGS) -o player.o player.c

packet.o: packet.c $(HDRS)
	gcc $(OBJCFLAGS) -o packet.o packet.c

dll_test.o: dll_test.c player.o packet.o $(HDRS)
	gcc $(OBJCFLAGS) -o dll_test.o dll_test.c

game_dl.so: game.c libhandler.so dll_test.o $(HDRS)
	gcc -O2 -std=gnu99 -Wall -pedantic -fPIC -shared -pie -pthread -o game_dl.so dll_test.o player.o packet.o game.c -L. -lhandler -Wl,-E,-rpath=.

server.out: server.c $(HDRS)
	gcc -O2 -Wall -pedantic -pthread -o server.out server.c -ldl

clean:
	rm -f $(OBJS)

