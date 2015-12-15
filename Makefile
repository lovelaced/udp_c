CC=gcc
CFLAGS=-Wall -std=c99

LIBS = -lpthread
all: blaster blastee

blaster: blaster.c
	gcc -g -o blaster blaster.c $(LIBS)

blastee: blastee.c
	gcc -o blastee blastee.c

clean:
	rm blaster blastee
