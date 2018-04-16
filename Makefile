CC=gcc
CFLAGS=-pthread -Wall

all: proj1

proj1: sisinger_andrew_proj1_COMP323.c proj1.h
	$(CC) $(CFLAGS) -o proj1 sisinger_andrew_proj1_COMP323.c

clean:
	rm proj1
