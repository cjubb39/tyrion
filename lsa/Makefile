# +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
# Makefile for Problem Set 1; CS 445
# Anton Petrov - 22-09-11

INCLUDES = ../svd_base_impl/Singular-Value-Decomposition--cs445-/ 
CFLAGS = -g -std=c99 -pedantic -Wall -I$(INCLUDES)
LDLIBS=-lm  
LDFLAGS= -L$(INCLUDES)
CC = gcc

all: clean lsa

lsa: jacobi.o  

jacobi.o: jacobi.c jacobi.h


clean:
	/bin/rm -f *.o lsa *-sqrt.txt *-square_and_add.txt
