.PHONY: uriparse regex

CC=gcc 
CFLAGS=-Wall -DEBUG=1 -DURIPARSE_DEBUG=1 -DNVLIST_MAIN=1
CFLAGS=-Wall -DEBUG=1 -DURIPARSE_DEBUG=0 

# use our own realpath on windows
ifeq ($(OS),Windows_NT)
	REALPATH=../lib/realpath.o
else
	REALPATH=
endif

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
	g++ -lpcrecpp -lpcre -c -o ../lib/regcpp.o ../lib/reg.cpp -DPCRE_STATIC
	g++ -o reg.exe ../lib/regcpp.o reg.c -DPCRE_STATIC -I../../mingw-15/MinGW/include \
		../../mingw-15/MinGW/lib/libpcre.a \
		../../mingw-15/MinGW/lib/libpcrecpp.a \
		../../mingw-15/MinGW/lib/libpcreposix.a

mydir:
	$(CC) $(CFLAGS) -c ../lib/realpath.c -o ../lib/realpath.o
	gcc -o mydir.exe $(REALPATH) -DMYDIR_MAIN ../lib/mydir.c

quote:
	gcc -o quote.exe quote.c -DQUOTE_MAIN

expenv:
	g++ -o expenv.exe expenv.c -DEXPENV_MAIN

cmd_parser:
	$(CC) $(CFLAGS) -c -o ../lib/stringlib.o ../lib/stringlib.c
	$(CC) $(CFLAGS) -c -o ../lib/nvlist.o ../lib/stringlib.o ../lib/nvlist.c
	$(CC) $(CFLAGS) -c -o ../lib/uriparse.o ../lib/uriparse.c
	g++ -DCMD_PARSER_MAIN -o cmd_parser.exe ../lib/stringlib.o ../lib/nvlist.o ../lib/uriparse.o cmd_parser.c 