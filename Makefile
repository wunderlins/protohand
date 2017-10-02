.PHONY: ini usage test param_test release puh ph

# DUBUG=0 disables debugging, other levels from 1-3
# LOG_TO_FILE=1 will send dbg output to stdout, else to protohand.log
DEBUG=3
LOG_TO_FILE=1
VERSION=0.2.3
# naming
PROGNAME = protohand
PROGNAME_SHORT = ph
_EXT = .exe
CC = gcc

detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')
SEDI_EXT=
ifeq ($(detected_OS),Darwin)  # Mac OS X
	SEDI_EXT = .bak 
endif

# limits
STDIN_MAX = 1024
MAX_CWD_LENGTH = $(shell sh -c './bin/MAX_CWD_LENGTH.exe')

# cflags, strip if debuggign is disabled
CFLAGS = -DLOG_TO_FILE=$(LOG_TO_FILE) -Wall -DDEBUG=$(DEBUG) -DPROGNAME=$(PROGNAME) # -DSTDIN_MAX=$(STDIN_MAX) -DMAX_CWD_LENGTH=$(MAX_CWD_LENGTH)
ifeq ($(DEBUG),0) 
	CFLAGS += -s
endif

include os.mk

# use our own realpath on windows
ifeq ($(OS),Windows_NT)
	REALPATH=lib/realpath.o
else
	REALPATH=
endif

# create release name: OS_VERSION_TIMESTAMP
timestamp=$(shell date "+%Y%m%d%H%M")
rel = $(operating_system)_$(VERSION)_$(timestamp)

export

dep: regexcpp
	$(CC) $(CFLAGS) -c lib/mydir.c -o lib/mydir.o
	$(CC) $(CFLAGS) -c lib/realpath.c -o lib/realpath.o
	$(CC) $(CFLAGS) -c lib/ini.c -o lib/ini.o
	$(CC) $(CFLAGS) -c lib/stringlib.c -o lib/stringlib.o
	$(CC) $(CFLAGS) -c lib/uriparse.c -o lib/uriparse.o
	$(CC) $(CFLAGS) -c lib/nvlist.c -o lib/nvlist.o

regexcpp:
	g++ $(CFLAGS) -lpcrecpp -lpcre -c -o lib/regcpp.o lib/reg.cpp -DPCRE_STATIC -I../mingw-15/MinGW/include \
		../mingw-15/MinGW/lib/libpcre.a \
		../mingw-15/MinGW/lib/libpcrecpp.a \
		../mingw-15/MinGW/lib/libpcreposix.a
	
ph: doc
	g++ $(CFLAGS) -o $(PROGNAME_SHORT)$(_EXT) lib/regcpp.o lib/errstr.o lib/mydir.o lib/nvlist.o $(REALPATH) lib/stringlib.o lib/ini.o lib/uriparse.o ph.c ico/app.res -DPCRE_STATIC -I../mingw-15/MinGW/include \
		../mingw-15/MinGW/lib/libpcre.a \
		../mingw-15/MinGW/lib/libpcrecpp.a \
		../mingw-15/MinGW/lib/libpcreposix.a

testregex:
	g++ $(CFLAGS) -o testregex$(_EXT) $(REALPATH) lib/regcpp.o testregex.c ico/testregex_generated.res -DPCRE_STATIC -I../mingw-15/MinGW/include \
		../mingw-15/MinGW/lib/libpcre.a \
		../mingw-15/MinGW/lib/libpcrecpp.a \
		../mingw-15/MinGW/lib/libpcreposix.a

errstr:
	$(CC) $(CFLAGS) -c lib/errstr.c -o lib/errstr.o
	
create_error: errstr
	$(CC) $(CFLAGS) -o create_error.exe lib/errstr.o create_error.c
	./create_error.exe
	
max_path:
	$(CC) $(CFLAGS) -o bin/MAX_CWD_LENGTH.exe bin/MAX_CWD_LENGTH.c
	$(eval MAX_CWD_LENGTH := $(shell sh -c './bin/MAX_CWD_LENGTH.exe'))

test_ph: ph
	$(PROGNAME_SHORT)$(_EXT) 'proto:auth?query=value%201&param2&param3 '

icon:
	$(MAKE) -C ico
	
# build the programm
all: clean max_path doc icon usage ini error dep ph testcmd testregex

protohand:
	$(CC) $(CFLAGS) -c lib/realpath.c -o lib/realpath.o
	$(CC) $(CFLAGS) -c lib/ini.c -o lib/ini.o
	$(CC) $(CFLAGS) -c lib/stringlib.c -o lib/stringlib.o
	$(CC) $(CFLAGS) -c lib/uriparse.c -o lib/uriparse.o
	$(CC) $(CFLAGS) -c lib/nvlist.c -o lib/nvlist.o
	$(CC) $(CFLAGS) -o $(PROGNAME)$(_EXT) lib/nvlist.o lib/realpath.o lib/stringlib.o lib/ini.o lib/uriparse.o protohand.c ico/app.res

# auto-update documentation
doc: usage ini errstr create_error reg error
	
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
	cp $(PROGNAME_SHORT)$(_EXT) "release/$(rel)"
	strip "release/$(rel)/$(PROGNAME_SHORT)$(_EXT)"
	cp README.txt "release/$(rel)/"
	cp generated/error.html "release/$(rel)/"
	cp testcmd$(_EXT) "release/$(rel)/"
	cp testregex$(_EXT) "release/$(rel)/"
	cp protohand.reg "release/$(rel)/ph.reg"
	cp ph.ini "release/$(rel)"
	cp LICENSE.txt "release/$(rel)/"
	strip "release/$(rel)/testcmd$(_EXT)"
	strip "release/$(rel)/testregex$(_EXT)"
	
	# subsitute all keywords in the README.txt file
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME_SHORT)/g' release/$(rel)/README.txt
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' release/$(rel)/README.txt
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' release/$(rel)/README.txt
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' release/$(rel)/README.txt
	# windows line endings
	sed -i $(SEDI_EXT) 's/$$/\r\n/' release/$(rel)/README.txt
	sed -i $(SEDI_EXT) 's/$$/\r\n/' release/$(rel)/LICENSE.txt
	sed -i $(SEDI_EXT) 's/$$/\r\n/' release/$(rel)/ph.ini
	sed -i $(SEDI_EXT) 's/protohand\.exe/$(PROGNAME_SHORT)$(_EXT)/g' release/$(rel)/ph.reg
	sed -i $(SEDI_EXT) 's/$$/\r\n/' release/$(rel)/ph.reg
	
	
	# create zip file
	cd release/$(rel); ../../bin/zip.exe "../$(rel).zip" * 
	
	# remove working directory
	rm -r "release/$(rel)"
	
	# add to git repo, tag release and push changes/tags
	#git add release/*.zip
	#git commit -am "Added release $(rel)"
	#git push
	#git tag $(rel) && git push --tags
	
# generate a simple cmd utility that will print all parameters
testcmd:
	$(CC) $(CFLAGS) -c lib/mydir.c -o lib/mydir.o
	$(CC) $(CFLAGS) -c lib/realpath.c -o lib/realpath.o
	$(CC) $(CFLAGS) -o testcmd$(_EXT) $(REALPATH) lib/mydir.o testcmd.c ico/testcmd_generated.res

# test url parser
test:
	./$(PROGNAME)$(_EXT) "12345://67890" || true
	./$(PROGNAME)$(_EXT) "12345:///67890" || true
	./$(PROGNAME)$(_EXT)$(_EXT) "12345://?67890" || true
	./$(PROGNAME)$(_EXT) "12345://as/67890" || true
	./$(PROGNAME)$(_EXT) "12345://as?67890" || true
	./$(PROGNAME)$(_EXT) "12345://as/xy?67890" || true
	./$(PROGNAME)$(_EXT) "12345:67890" || true
	./$(PROGNAME)$(_EXT) "12345:/67890" || true
	./$(PROGNAME)$(_EXT) "12345:?67890" || true
	./$(PROGNAME)$(_EXT) "12345:as/67890" || true
	./$(PROGNAME)$(_EXT) "12345:as?67890" || true
	./$(PROGNAME)$(_EXT) "12345:as/xy?67890" || true
	./$(PROGNAME)$(_EXT) "usb://centricity?--auth=user&pass=ab%20de" || true

# run parameter test
param_test:
	./$(PROGNAME)$(_EXT) "aaa://fertimed/document?--document=C:\\windows\\CSUP.txt&a=b&%20&b=x&c=d&--help" || cat protohand.log
	./$(PROGNAME)$(_EXT) "aaa://fertimed/document?--document=%20C:\\windows\\CSUP.txt&--help" ||  cat protohand.log
	./$(PROGNAME)$(_EXT) "aaa://fertimed/document?--document=\\\\anacom03\\tegris\\c$$\\windows\\CSUP.txt&--help" || cat protohand.log

# cleanup
clean:
	rm -r $(PROGNAME)$(_EXT) || true
	rm -r $(PROGNAME_SHORT)$(_EXT) || true
	rm -r testcmd$(_EXT) || true
	rm -r create_error.exe || true
	rm -r testcmd.exe || true
	rm -r stringlib$(_EXT) || true
	rm -r test/realpath_test$(_EXT) || true
	rm -r test/find_param$(_EXT) || true
	rm -r test/cleanpath$(_EXT) || true
	rm -r ini/ini_dump$(_EXT) || true
	rm -r ini/ini_example$(_EXT) || true
	rm -r test/ini_test$(_EXT) || true
	rm -r test/compare$(_EXT) || true
	#find ./ -iname "*.exe" -exec rm {} \; || true
	rm -r *.o || true
	rm -r lib/*.o || true
	rm -r test/*.o || true
	rm *.bak || true

# create usage generated/README.h header file from README.txt
usage:
	echo "// Automatically generated file. Edit README.txt and run " > generated/README.h
	echo "// 'make usage' to update this documentation!" >> generated/README.h
	echo "" >> generated/README.h
	echo "const char* usage_str = \"\"" >> generated/README.h
	#sed -e 's/%/%%/g; s/\\/\\\\/g; s/"/\\"/g; s/^/"/g; s/$$/\\n"/g' README.txt >> generated/README.h
	#sed -i 's/"/\\"/g;' generated/README.h
	#sed -i 's/$$/\\n"/g' generated/README.h
	sed -f bin/replace.sed README.txt >> generated/README.h
	echo \""\";" >> generated/README.h
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' generated/README.h
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' generated/README.h
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' generated/README.h
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' generated/README.h

# create example ini content header file generated/example_ini.h from example.ini
ini:
	echo "// Automatically generated file. Edit example.ini and run " > generated/example_ini.h
	echo "// 'make ini' to update this documentation!" >> generated/example_ini.h
	echo "" >> generated/example_ini.h
	echo "const char* ini_str = \"\"" >> generated/example_ini.h
	sed -f bin/replace.sed example.ini >> generated/example_ini.h
	echo \""\";" >> generated/example_ini.h
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' generated/example_ini.h
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' generated/example_ini.h
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' generated/example_ini.h
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' generated/example_ini.h

error:
	echo "// Automatically generated file. Edit generated/error.html and run " > generated/error.h
	echo "// 'make ini' to update this documentation!" >> generated/error.h
	echo "" >> generated/error.h
	echo "const char* error_str = \"\"" >> generated/error.h
	sed -f bin/replace.sed generated/error.html >> generated/error.h
	echo \""\";" >> generated/error.h
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' generated/error.h
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' generated/error.h
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' generated/error.h
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' generated/error.h

reg:
	echo "const char* reg_str = \"\"" > generated/reg.h
	sed -f bin/replace.sed example.reg >> generated/reg.h
	echo \""\";" >> generated/reg.h
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' generated/reg.h
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' generated/reg.h

# show todo list
todo:
	egrep -nr "FIXME|TODO" *.c *.h

# example programs for library test
find_param:
	$(CC) $(CFLAGS) -c lib/stringlib.c
	$(CC) $(CFLAGS) -o test/find_param$(_EXT) stringlib.o test/find_param.c

stringlib:
	$(CC) $(CFLAGS) -c lib/stringlib.c -o lib/stringlib.o

realpath:
	$(CC) $(CFLAGS) -c realpath.c -o realpath.o
	$(CC) $(CFLAGS) -o test/realpath_test$(_EXT) realpath.o test/realpath_test.c

cleanpath:
	$(CC) $(CFLAGS) -o test/cleanpath$(_EXT) test/cleanpath.c

ini_dump:
	$(CC) $(CFLAGS) -c ini.c
	$(CC) $(CFLAGS) -o ini/ini_dump$(_EXT) ini.o ini/ini_dump.c
	
ini_example:
	$(CC) $(CFLAGS) -c ini.c
	$(CC) $(CFLAGS) -o ini/ini_example$(_EXT) ini.o ini/ini_example.c

ini_test:
	$(CC) $(CFLAGS) -c ini.c
	$(CC) $(CFLAGS) -o test/ini_test$(_EXT) ini.o test/ini_test.c

nvlist:
	$(CC) $(CFLAGS) -c stringlib.c
	$(CC) $(CFLAGS) -D NVLIST_MAIN -o test/nvlist$(_EXT) stringlib.o test/nvlist.c

compare:
	$(CC) $(CFLAGS) -o test/compare$(_EXT) test/compare.c

uriparse: stringlib
	$(CC) $(CFLAGS) -c -o test/nvlist.o test/nvlist.c
	$(CC) $(CFLAGS) -DURIPARSE_DEBUG=1 -c -o lib/uriparse.o lib/uriparse.c
	$(CC) $(CFLAGS) -o lib/uriparse.exe test/nvlist.o lib/stringlib.o lib/uriparse.o
	lib/uriparse.exe

clearlog:
	rm ph.log || true
	rm testcmd.log || true