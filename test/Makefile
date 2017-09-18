.PHONY: uriparse regex

CC=gcc 
CFLAGS=-Wall -DEBUG=1 -DURIPARSE_DEBUG=1 -DNVLIST_MAIN=1 -I C:\MinGW\msys\1.0\include

uriparse:
	$(CC) $(CFLAGS) -c -o ../uriparse.o ../uriparse.c
	$(CC) $(CFLAGS) -o uriparse.exe ../uriparse.o
	./uriparse.exe

regex:
	$(CC) $(CFLAGS) -L C:\MinGW\msys\1.0\lib -lregex -o regex.exe regex.c