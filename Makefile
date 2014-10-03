# Makefile for IDX.tae
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lncursesw

dtdw : ./tae/main.o ./tae/display.o
	$(CC) ./tae/main.o ./tae/display.o -o dtdw $(LDFLAGS)

main.o : ./tae/main.c ./tae/display.h
	$(CC) $(CFLAGS) -c ./tae/main.c

display.o : ./tae/display.c ./tae/display.h
	$(CC) $(CFLAGS) -c ./tae/display.c

clean :
	$(RM) ./tae/*.o