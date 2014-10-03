# Makefile for IDX.tae
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lncursesw

dtdw : main.o display.o
	$(CC) main.o display.o -o dtdw $(LDFLAGS)

main.o : main.c display.h
	$(CC) $(CFLAGS) -c main.c

display.o : display.c display.h
	$(CC) $(CFLAGS) -c display.c

clean :
	$(RM) *.o