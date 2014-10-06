# Makefile for IDX.tae
CC = gcc
CFLAGS = -Wall -ggdb
LDFLAGS = -lncursesw -lm

tae : main.o loader.o inventory.o dialog.o display.o
	$(CC) main.o loader.o inventory.o dialog.o display.o -o run $(LDFLAGS)

main.o : main.c loader.h inventory.h dialog.h display.h
	$(CC) $(CFLAGS) -c main.c $(LDFLAGS)

loader.o : loader.c loader.h
	$(CC) $(CFLAGS) -c loader.c $(LDFLAGS)

inventory.o : inventory.c inventory.h
	$(CC) $(CFLAGS) -c inventory.c $(LDFLAGS)

dialog.o : dialog.c dialog.h
	$(CC) $(CFLAGS) -c dialog.c $(LDFLAGS)

display.o : display.c display.h
	$(CC) $(CFLAGS) -c display.c $(LDFLAGS)

clean :
	$(RM) *.o
