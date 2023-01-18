.PHONY: all clean

CC = gcc

CFLAGS = -Wall -g

LD = gcc

LDFLAGS = 

all: default

pipeit: pipeit.o
	$(LD) $(LDFLAGS) -o pipeit pipeit.o

lab01.o: pipeit.c
	$(CC) $(CFLAGS) -c -o pipeit.o pipeit.c

test: pipeit
	echo Testing pipeit
	./pipeit < inputfile
	echo done.

clean:
	rm -f pipeit pipeit.o