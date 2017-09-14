.PHONY: stringlib ini usage test param_test release puh

# DUBUG=0 disables debugging, other levels from 1-3
# LOG_TO_FILE=1 will send dbg output to stdout, else to protohand.log
DEBUG=3
LOG_TO_FILE=1
VERSION=0.1.1
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
CFLAGS = -DLOG_TO_FILE=$(LOG_TO_FILE) -Wall -DDEBUG=$(DEBUG) -DPROGNAME=$(PROGNAME) -DSTDIN_MAX=$(STDIN_MAX) -DMAX_CWD_LENGTH=$(MAX_CWD_LENGTH)
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
timestamp=$(shell date "+%Y%m%d%H%M")
rel = $(operating_system)_$(VERSION)_$(timestamp)

# build the programm
all:
	gcc $(CFLAGS) -c realpath.c -o realpath.o
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -c stringlib.c
	gcc $(CFLAGS) -c uriparse.c
	gcc $(CFLAGS) -o $(PROGNAME)$(_EXT) realpath.o stringlib.o ini.o uriparse.o protohand.c

doc: usage ini
	
# unicode test
phu:
	gcc $(CFLAGS) -o phu8$(_EXT) phu8.c

# create a release
release:
	# get latest code
	git pull
	
	# build the application
	$(MAKE) all
	
	# make release directory
	@echo release/$(rel)
	mkdir "release/$(rel)"
	
	# copy files to release directory
	cp $(PROGNAME)$(_EXT) "release/$(rel)"
	cp README.txt "release/$(rel)/"
	cp protohand.reg "release/$(rel)/"
	cp LICENSE.txt "release/$(rel)/"
	
	# subsitute all keywords in the README.txt file
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' release/$(rel)/README.txt
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' release/$(rel)/README.txt
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' release/$(rel)/README.txt
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' release/$(rel)/README.txt
	# windows line endings
	sed -i $(SEDI_EXT) 's/$$/\r\n/' release/$(rel)/README.txt
	sed -i $(SEDI_EXT) 's/$$/\r\n/' release/$(rel)/LICENSE.txt
	
	# create zip file
	cd release/$(rel); ../../bin/zip.exe "../$(rel).zip" * 
	
	# remove working directory
	rm -r "release/$(rel)"
	
	# add to git repo, tag release and push changes/tags
	git add release/*.zip
	git commit -am "Added release $(rel)"
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
	rm -r phu8$(_EXT) || true
	rm -r $(PROGNAME)$(_EXT) || true
	rm -r stringlib$(_EXT) || true
	rm -r test/realpath_test$(_EXT) || true
	rm -r test/find_param$(_EXT) || true
	rm -r test/cleanpath$(_EXT) || true
	rm -r ini/ini_dump$(_EXT) || true
	rm -r ini/ini_example$(_EXT) || true
	rm -r test/ini_test$(_EXT) || true
	rm -r test/compare$(_EXT) || true
	rm -r *.o || true
	rm -r test/*.o || true
	rm *.bak || true

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

realpath:
	gcc $(CFLAGS) -c realpath.c -o realpath.o
	gcc $(CFLAGS) -o test/realpath_test$(_EXT) realpath.o test/realpath_test.c

cleanpath:
	gcc $(CFLAGS) -o test/cleanpath$(_EXT) test/cleanpath.c

ini_dump:
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -o ini/ini_dump$(_EXT) ini.o ini/ini_dump.c
	
ini_example:
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -o ini/ini_example$(_EXT) ini.o ini/ini_example.c

ini_test:
	gcc $(CFLAGS) -c ini.c
	gcc $(CFLAGS) -o test/ini_test$(_EXT) ini.o test/ini_test.c

nvlist:
	gcc $(CFLAGS) -c stringlib.c
	gcc $(CFLAGS) -D NVLIST_MAIN -o test/nvlist$(_EXT) stringlib.o test/nvlist.c

compare:
	gcc $(CFLAGS) -o test/compare$(_EXT) test/compare.c

