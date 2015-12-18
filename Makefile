CC=gcc
CFLAGS=-Wall -std=c99

LIBS = -lpthread
all: blaster blastee

blaster: blaster.c
	gcc $(CFLAGS) -g -o blaster blaster.c $(LIBS)

blastee: blastee.c
	gcc $(CFLAGS) -o blastee blastee.c

clean:
	rm blaster blastee
