
#################################################################
##
## FILE:	Makefile
## PROJECT:	CS 3251 Project 2 - Professor Traynor
## DESCRIPTION: Compile Project 2
##
#################################################################

CC=gcc

OS := $(shell uname -s)

# Extra LDFLAGS if Solaris
ifeq ($(OS), SunOS)
	LDFLAGS=-lsocket -lnsl
    endif

all: client server-thread

client: client.c
	$(CC) client.c  -o nameChanger

server-thread: server/server-thread.c

	$(CC) -pthread -lcrypto server/server-thread.c -o threadedServer

fm:
	gcc filemanager.c -o filemanager

debug:
	gcc filemanager.c -g -o filemanager

fm-test:
	gcc filemanager.c test.c -o filemanager_test

clean:
	rm -f nameChanger filemanager threadedServer filemanager_test *.o
