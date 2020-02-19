CC=g++
CFLAGS=   #-pedantic -Werror -Wall #-O3 -fPIC  -lpthread
#CFLAGS=-ggdb3 -fPIC

all: ringmaster player

ringmaster: ringmaster.cpp potato.h ringmaster.h server.h
	$(CC) $(CFLAGS) -o $@ ringmaster.cpp

player: player.cpp potato.h server.h
	$(CC) $(CFLAGS) -o $@ player.cpp

clean:
	rm -rf ringmaster player *.o *~
