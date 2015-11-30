CC=g++
CFLAGS=-I.

all: server 
	$(CC) -o server server.cpp $(CFLAGS)
