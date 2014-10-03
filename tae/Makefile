# Makefile for IDX.tae
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lncursesw

dtdw : main.o loader.o display.o
	$(CC) main.o loader.o display.o -o dtdw $(LDFLAGS)

main.o : main.c loader.h display.h
	$(CC) $(CFLAGS) -c main.c

loader.o : loader.c loader.h
	$(CC) $(CFLAGS) -c loader.c

display.o : display.c display.h
	$(CC) $(CFLAGS) -c display.c

clean :
	$(RM) *.o