PROGNAME = protohand
_EXT = .exe
STDIN_MAX = 1024
MAX_CWD_LENGTH = 1024
COPTS = -Wall -DDEBUG=1 -DPROGNAME=$(PROGNAME) -DSTDIN_MAX=$(STDIN_MAX) -DMAX_CWD_LENGTH=$(MAX_CWD_LENGTH)

all:
	gcc $(COPTS) -c ini.c
	gcc $(COPTS) -c urldecode2.c
	gcc $(COPTS) -o $(PROGNAME)$(_EXT) urldecode2.o ini.o protohand.c

ini_dump:
	gcc $(COPTS) -c ini.c
	gcc $(COPTS) -o ini/ini_dump$(_EXT) ini.o ini/ini_dump.c
	
ini_example:
	gcc $(COPTS) -c ini.c
	gcc $(COPTS) -o ini/ini_example$(_EXT) ini.o ini/ini_example.c

ini_test:
	gcc $(COPTS) -c ini.c
	gcc $(COPTS) -o ini_test$(_EXT) ini.o ini_test.c

compare:
	gcc $(COPTS) -o compare$(_EXT) compare.c

test:
	$(PROGNAME)$(_EXT) "12345://67890" || true
	$(PROGNAME)$(_EXT) "12345:///67890" || true
	$(PROGNAME)$(_EXT)$(_EXT) "12345://?67890" || true
	$(PROGNAME)$(_EXT) "12345://as/67890" || true
	$(PROGNAME)$(_EXT) "12345://as?67890" || true
	$(PROGNAME)$(_EXT) "12345://as/xy?67890" || true
	$(PROGNAME)$(_EXT) "12345:67890" || true
	$(PROGNAME)$(_EXT) "12345:/67890" || true
	$(PROGNAME)$(_EXT) "12345:?67890" || true
	$(PROGNAME)$(_EXT) "12345:as/67890" || true
	$(PROGNAME)$(_EXT) "12345:as?67890" || true
	$(PROGNAME)$(_EXT) "12345:as/xy?67890" || true
	$(PROGNAME)$(_EXT) "usb://centricity?--auth=user&pass=ab%20de" || true

clean:
	rm -r *.o *$(_EXT) || true

usage:
	echo "// Automatically generated file. Edit README.txt and run " > README.h
	echo "// 'make usage' to update this documentation!" >> README.h
	echo "" >> README.h
	echo "char* usage_str = \"\"" >> README.h
	sed -e 's/%/%%/g; s/\\/\\\\/g; s/"/\\"/g; s/^/"/g; s/$$/\\n"/g' README.txt >> README.h
	echo \""\";" >> README.h
	sed -i .bak 's/PROGNAME/$(PROGNAME)/g' README.h
	sed -i .bak 's/STDIN_MAX/$(STDIN_MAX)/g' README.h
	sed -i .bak 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' README.h
	sed -i .bak 's/_EXT/$(_EXT)/g' README.h

todo:
	egrep -nr "FIXME|TODO" *.c *.h
