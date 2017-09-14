.PHONY: uriparse

CC=gcc 
CFLAGS=-Wall -DEBUG=1 -DURIPARSE_DEBUG=1

uriparse:
	$(CC) $(CFLAGS) -c -o uriparse.o uriparse.c
	$(CC) $(CFLAGS) -o uriparse.exe uriparse.o
	./uriparse.exe

