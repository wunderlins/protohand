.PHONY: uriparse regex

CC=gcc 
CFLAGS=-Wall -DEBUG=1 -DURIPARSE_DEBUG=1 -DNVLIST_MAIN=1

uriparse:
	$(CC) $(CFLAGS) -c -o ../uriparse.o ../uriparse.c
	$(CC) $(CFLAGS) -o uriparse.exe ../uriparse.o
	./uriparse.exe

regex:
	cd ../lib/mingw-libgnurx-2.5.1; ./configure; make; cd ../../test
	$(CC) $(CFLAGS) -c -o regex.o regex.c
	$(CC) $(CFLAGS) -o regex.exe regex.o ../lib/mingw-libgnurx-2.5.1/regex.o
	
regexcpp:
	# requires https://nuwen.net/mingw.html
	g++ regex.cpp -lpcrecpp -lpcre -DPCRE_STATIC

regcpp:
	#g++ -o regcpp.exe reg.cpp -lpcrecpp -lpcre -DPCRE_STATIC -DCPPMAIN
	g++ -o regcpp.exe ../lib/reg.cpp -lpcrecpp -lpcre -DCPPMAIN

reg:
	g++ -lpcrecpp -lpcre -c -o ../lib/regcpp.o ../lib/reg.cpp
	gcc -o reg.exe ../lib/regcpp.o reg.c