.PHONY: stringlib ini usage test param_test release

# DUBUG=0 disables debugging, other levels from 1-3
DEBUG=3
VERSION=0.1.0
# naming
PROGNAME = protohand
_EXT = .exe

# on darwin, we need to add a file extension to the -i flag of sed
detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')
SEDI_EXT=
ifeq ($(detected_OS),Darwin)  # Mac OS X
	SEDI_EXT = .bak 
endif

# limits
STDIN_MAX = 1024
MAX_CWD_LENGTH = 1024

# cflags, strip if debuggign is disabled
CFLAGS = -Wall -DDEBUG=$(DEBUG) -DPROGNAME=$(PROGNAME) -DSTDIN_MAX=$(STDIN_MAX) -DMAX_CWD_LENGTH=$(MAX_CWD_LENGTH)
ifeq ($(DEBUG),0) 
	CFLAGS += -s
endif

# OS DETECTION
operating_system=
ifeq ($(OS),Windows_NT)
    operating_system=WIN32
    ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
        operating_system=WIN_AMD64
    else
        ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
            operating_system=WIN_AMD64
        endif
        ifeq ($(PROCESSOR_ARCHITECTURE),x86)
            operating_system=WIN_IA32
        endif
    endif
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Linux)
        operating_system=LINUX
    endif
    ifeq ($(UNAME_S),Darwin)
        operating_system=OSX
    endif
    UNAME_P := $(shell uname -p)
    ifeq ($(UNAME_P),x86_64)
        operating_system=AMD64
    endif
    ifneq ($(filter %86,$(UNAME_P)),)
        operating_system=IA32
    endif
    ifneq ($(filter arm%,$(UNAME_P)),)
        operating_system=ARM
    endif
endif

# create release name: OS_VERSION_TIMESTAMP
timestamp=$(shell date "+%Y%m%d%H%M%S")
rel = $(operating_system)_$(VERSION)_$(timestamp)

# build the programm
all: usage ini
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -c stringlib.c
	gcc $(CFLAGS) -c urldecode2.c
	gcc $(CFLAGS) -o $(PROGNAME)$(_EXT) stringlib.o urldecode2.o ini.o protohand.c

# create a release
release:
	git pull
	$(MAKE) all
	@echo release/$(rel)
	mkdir "release/$(rel)"
	cd release/$(rel); cp ../../$(PROGNAME)$(_EXT) .; sed 's/$$/\r\n/' ../../README.txt > README.txt; ../../bin/zip.exe "../$(rel).zip" *
	rm -r "release/$(rel)"
	git add release/*.zip
	git commit -am "Added release $rel"
	git push
	git tag $(rel) && git push --tags
	
# generate a simple cmd utility that will print all parameters
testcmd:
	gcc $(CFLAGS) -o testcmd$(_EXT) testcmd.c

# test url parser
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

# run parameter test
param_test:
	$(PROGNAME)$(_EXT) "aaa://fertimed/document?--document=C:\\windows\\CSUP.txt&a=b&%20&b=x&c=d&--help" || cat protohand.log
	$(PROGNAME)$(_EXT) "aaa://fertimed/document?--document=%20C:\\windows\\CSUP.txt&--help" ||  cat protohand.log
	$(PROGNAME)$(_EXT) "aaa://fertimed/document?--document=\\\\anacom03\\tegris\\c$$\\windows\\CSUP.txt&--help" || cat protohand.log

# cleanup
clean:
	rm -r *.o *$(_EXT) || true

# create usage README.h header file from README.txt
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

# create example ini content header file example_ini.h from example.ini
ini:
	echo "// Automatically generated file. Edit example.ini and run " > example_ini.h
	echo "// 'make ini' to update this documentation!" >> example_ini.h
	echo "" >> example_ini.h
	echo "char* ini_str = \"\"" >> example_ini.h
	sed -e 's/%/%%/g; s/\\/\\\\/g; s/"/\\"/g; s/^/"/g; s/$$/\\n"/g' example.ini >> example_ini.h
	echo \""\";" >> example_ini.h
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' example_ini.h
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' example_ini.h
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' example_ini.h
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' example_ini.h

# show todo list
todo:
	egrep -nr "FIXME|TODO" *.c *.h

# example programs for library test
find_param:
	gcc $(CFLAGS) -c stringlib.c
	gcc $(CFLAGS) -o test/find_param$(_EXT) stringlib.o test/find_param.c

stringlib:
	gcc $(CFLAGS) -o stringlib$(_EXT) stringlib.c

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

