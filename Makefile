.PHONY: stringlib

detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')

DEBUG=1

SEDI_EXT=
ifeq ($(detected_OS),Darwin)  # Mac OS X
	SEDI_EXT = .bak 
endif

PROGNAME = protohand
_EXT = .exe
STDIN_MAX = 1024
MAX_CWD_LENGTH = 1024

CFLAGS = -Wall -DDEBUG=$(DEBUG) -DPROGNAME=$(PROGNAME) -DSTDIN_MAX=$(STDIN_MAX) -DMAX_CWD_LENGTH=$(MAX_CWD_LENGTH)
ifeq ($(DEBUG),0) 
	CFLAGS += -s
endif

stringlib:
	gcc $(CFLAGS) -o stringlib$(_EXT) stringlib.c

all: usage
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -c stringlib.c
	gcc $(CFLAGS) -c urldecode2.c
	gcc $(CFLAGS) -o $(PROGNAME)$(_EXT) stringlib.o urldecode2.o ini.o protohand.c

ini_dump:
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -o ini/ini_dump$(_EXT) ini.o ini/ini_dump.c
	
ini_example:
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -o ini/ini_example$(_EXT) ini.o ini/ini_example.c

ini_test:
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -o ini_test$(_EXT) ini.o ini_test.c

compare:
	gcc $(CFLAGS) -o compare$(_EXT) compare.c

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

param_test:
	$(PROGNAME)$(_EXT) "aaa://fertimed/document?--document=C:\\windows\\CSUP.txt&a=b&%20c=d" || true
	
clean:
	rm -r *.o *$(_EXT) || true

usage:
	echo "// Automatically generated file. Edit README.txt and run " > README.h
	echo "// 'make usage' to update this documentation!" >> README.h
	echo "" >> README.h
	echo "char* usage_str = \"\"" >> README.h
	sed -e 's/%/%%/g; s/\\/\\\\/g; s/"/\\"/g; s/^/"/g; s/$$/\\n"/g' README.txt >> README.h
	echo \""\";" >> README.h
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' README.h
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' README.h
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' README.h
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' README.h

todo:
	egrep -nr "FIXME|TODO" *.c *.h

find_param:
	gcc $(CFLAGS) -c stringlib.c
	gcc $(CFLAGS) -o test/find_param$(_EXT) stringlib.o test/find_param.c