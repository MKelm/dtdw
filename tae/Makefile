# Makefile for IDX.tae
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lncursesw -lm

tae : main.o loader.o description.o inventory.o dialog.o display.o action.o jsmn/jsmn.o
	$(CC) main.o loader.o description.o inventory.o dialog.o display.o action.o jsmn/jsmn.o -o run $(LDFLAGS)

main.o : main.c loader.h description.h inventory.h dialog.h display.h action.h
	$(CC) $(CFLAGS) -c main.c $(LDFLAGS)

loader.o : loader.c loader.h jsmn/jsmn.h
	$(CC) $(CFLAGS) -c loader.c $(LDFLAGS)

description.o : description.c description.h
	$(CC) $(CFLAGS) -c description.c $(LDFLAGS)

inventory.o : inventory.c inventory.h
	$(CC) $(CFLAGS) -c inventory.c $(LDFLAGS)

dialog.o : dialog.c dialog.h
	$(CC) $(CFLAGS) -c dialog.c $(LDFLAGS)

display.o : display.c display.h
	$(CC) $(CFLAGS) -c display.c $(LDFLAGS)

action.o : action.c action.h
	$(CC) $(CFLAGS) -c action.c $(LDFLAGS)

clean :
	$(RM) *.o
