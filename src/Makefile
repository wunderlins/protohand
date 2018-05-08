.PHONY: ini usage test param_test release puh ph

PATH := $(PATH):$(PWD)/bin

## library versions
PCRE_VERSION=8.41
OPENSSL_VERSION=1.0.2m
CURL_VERSION=7.56.1

# DUBUG=0 disables debugging, other levels from 1-3
# LOG_TO_FILE=1 will send dbg output to stdout, else to protohand.log
DEBUG=1
LOG_TO_FILE=1
VERSION=0.7.3
# naming
PROGNAME = protohand
PROGNAME_SHORT = ph
CC = gcc

#export PATH := ./bin:$(PATH)
R_DIR=../release
B_DIR=../build

pwd := $(shell sh -c 'pwd || echo ""')
detected_OS := $(shell sh -c 'uname -s 2>/dev/null || echo not')
SEDI_EXT=
ifeq ($(detected_OS),Darwin)  # Mac OS X
	SEDI_EXT = .bak 
endif

# limits
STDIN_MAX = 1024
MAX_CWD_LENGTH = $(shell sh -c './bin/MAX_CWD_LENGTH$(_EXT)')

include os.mk
USR=$(shell ./bin/realpath $(pwd)/../usr | sed -e 's,/\([a-zA-Z]\)/,\1:/,')-$(operating_system)

# cflags, strip if debugging is disabled
CFLAGS = -DLOG_TO_FILE=$(LOG_TO_FILE) -Wall -DPROGNAME_SHORT=$(PROGNAME_SHORT) -DDEBUG=$(DEBUG) -DPROGNAME=$(PROGNAME) # -DSTDIN_MAX=$(STDIN_MAX) -DMAX_CWD_LENGTH=$(MAX_CWD_LENGTH)
ifeq ($(DEBUG),0) 
	CFLAGS += -s
	CFLAGS += -O3
else 
	CFLAGS += -g
endif
CFLAGS += -I$(USR)/include -L$(USR)/lib

#CFLAGS += -DPROGNAME_SHORT=$(PROGNAME_SHORT) -DMAX_CWD_LENGTH=$(MAX_CWD_LENGTH)

# use our own realpath on windows
ifeq ($(OS),Windows_NT)
	REALPATH=lib/realpath.o
	APP_ICO=ico/app.res
	TESTCMD_ICO=ico/testcmd_generated.res
	TESTREGEX_ICO=ico/testregex_generated.res
	BUILD_ICONS=icon
	DEP_REALPATH=dep_realpath
	TEST_REALPATH=realpath
	_EXT = .exe
	SSLOPT=mingw64
	REPLACE=-win
	RN=\r
	LIBS=-lgdi32 -lwldap32 -lz -lws2_32
	ZIP=7z.exe a
else
	REALPATH=
	APP_ICO=
	TESTCMD_ICO=
	TESTREGEX_ICO=
	BUILD_ICONS=
	DEP_REALPATH=
	TEST_REALPATH=
	_EXT=
	SSLOPT=linux-x86_64
	REPLACE=
	RN=
	LIBS=-pthread -lz -ldl
	ZIP=zip -q -r
endif
ifeq ($(UNAME_S),Darwin)
	SSLOPT=darwin64-x86_64-cc
endif

CONFIG_STATIC  = --disable-shared --enable-static
COMPILE_STATIC = -static -static-libgcc -DPCRE_STATIC -DCURL_STATICLIB

COMPILE_PCRE=
COMPILE_LIBPCRE=$(USR)/lib/libpcre.a
ifneq ($(shell test -e $(COMPILE_LIBPCRE) && echo -n yes),yes)
    COMPILE_PCRE=pcre
endif

COMPILE_CURL=
COMPILE_LIBCURL=$(USR)/lib/libcurl.a
ifneq ($(shell test -e $(COMPILE_LIBCURL) && echo -n yes),yes)
    COMPILE_CURL=curl
endif

COMPILE_OPENSSL=
COMPILE_LIBOPENSSL=$(USR)/lib/libssl.a
ifneq ($(shell test -e $(COMPILE_LIBOPENSSL) && echo -n yes),yes)
    COMPILE_CURL=openssl
endif

# create release name: OS_VERSION_TIMESTAMP
timestamp=$(shell date "+%Y%m%d%H%M")
rel = $(operating_system)_$(VERSION)_$(timestamp)

export

#CFLAGS += -I $(USR)/include -L$(USR)/lib

os:
	@echo $(operating_system)

# build everything including 3rd party libs
all: $(COMPILE_PCRE) $(COMPILE_OPENSSL) $(COMPILE_CURL) app

# build all without deps
app: clean create_error max_path doc $(BUILD_ICONS) dep ph testcmd testregex

## DEPENDENCIES ################################################################
pcre:
	-mkdir $(USR) | true
	tar xfj pcre-$(PCRE_VERSION).tar.bz2 -C ..
	cd ../pcre-$(PCRE_VERSION) && ./configure --prefix $(USR) $(CONFIG_STATIC) # --disable-cpp
	cd ../pcre-$(PCRE_VERSION) && make
	cd ../pcre-$(PCRE_VERSION) && make install

openssl:
	-mkdir $(USR) | true
	tar xfz openssl-$(OPENSSL_VERSION).tar.gz -C ..
	cd ../openssl-$(OPENSSL_VERSION) && ./Configure --prefix=$(USR)/ $(SSLOPT)
	cd ../openssl-$(OPENSSL_VERSION) && make
	cd ../openssl-$(OPENSSL_VERSION) && make install

curl:
	-mkdir $(USR) | true
	tar xfz curl-$(CURL_VERSION).tar.gz -C ..
	cd ../curl-$(CURL_VERSION) && ./configure --prefix $(USR) --with-ssl=$(USR) $(CONFIG_STATIC) # --disable-manual --disable-unix-sockets --disable-verbose --disable-versioned-symbols --enable-hidden-symbols --without-librtmp
	cd ../curl-$(CURL_VERSION) && make
	-cd ../curl-$(CURL_VERSION) && make install

dep: regexcpp errstr cmdparser $(DEP_REALPATH) dep_httpget dep_xor
	$(CC) $(CFLAGS) -c lib/mydir.c -o lib/mydir.o
	$(CC) $(CFLAGS) -c lib/ini.c -DINI_MAX_LINE=4096 -o lib/ini.o
	$(CC) $(CFLAGS) -c lib/stringlib.c -o lib/stringlib.o
	$(CC) $(CFLAGS) -c lib/uriparse.c -o lib/uriparse.o
	$(CC) $(CFLAGS) -c lib/nvlist.c -o lib/nvlist.o
	$(CC) $(CFLAGS) -c lib/transform.c -o lib/transform.o
	$(CC) $(CFLAGS) -c lib/getexe.c -o lib/getexe.o

dep_getexe:	
	$(CC) $(CFLAGS) -c lib/getexe.c -o lib/getexe.o
	
dep_xor:
	$(CC) $(CFLAGS) -c lib/xor.c -o lib/xor.o

dep_httpget:
	$(CC) $(CFLAGS) \
		-c lib/httpget.c -o lib/httpget.o \
		-DSKIP_PEER_VERIFICATION -DCURL_STATICLIB \
		-lcurl -lssl -lcrypto -lgdi32 -lwldap32 -lz -lws2_32

dep_realpath:
	$(CC) $(CFLAGS) -c lib/realpath.c -o lib/realpath.o

cmdparser:
	$(CC) $(CFLAGS) -c lib/cmd_parser.c -o lib/cmd_parser.o
	
regexcpp:
	g++ $(COMPILE_STATIC) -std=c++11 $(CFLAGS) -lpcrecpp -lpcre -c -o lib/regcpp.o lib/reg.cpp

icon:
	$(MAKE) -C ico
	


## APPLICATIONS ################################################################
ph:
	g++ $(CFLAGS) -o $(PROGNAME_SHORT)$(_EXT) \
		lib/httpget.o \
		lib/xor.o \
		lib/regcpp.o \
		lib/mydir.o \
		lib/nvlist.o $(REALPATH) \
		lib/stringlib.o \
		lib/ini.o lib/uriparse.o \
		lib/cmd_parser.o \
		lib/errstr.o \
		lib/transform.o \
		lib/getexe.o \
		ph.c $(APP_ICO) -lpcrecpp -lpcre \
		-lcurl -lssl -lcrypto \
		$(LIBS) $(COMPILE_STATIC)

max_path:
	$(CC) $(CFLAGS) -o bin/MAX_CWD_LENGTH$(_EXT) bin/MAX_CWD_LENGTH.c
	$(eval MAX_CWD_LENGTH := $(shell sh -c './bin/MAX_CWD_LENGTH$(_EXT)'))

# generate a simple cmd utility that will print all parameters
testcmd: $(DEP_REALPATH)
	$(CC) $(CFLAGS) -c lib/mydir.c -o lib/mydir.o
	$(CC) $(CFLAGS) -o testcmd$(_EXT) $(REALPATH) lib/mydir.o testcmd.c $(TESTCMD_ICO)

# test targets
testregex:
	g++ $(CFLAGS) -o testregex$(_EXT) $(REALPATH) lib/regcpp.o testregex.c $(TESTREGEX_ICO) -lpcrecpp -lpcre $(COMPILE_STATIC)

## DOCUMENTATION ###############################################################
# auto-update documentation
doc: release_doc usage ini errstr create_error reg error

errstr:
	$(CC) $(CFLAGS) -c lib/errstr.c -o lib/errstr.o

create_error: errstr
	$(CC) $(CFLAGS) -o create_error.exe lib/errstr.o create_error.c
	./create_error.exe
	cp generated/error.html error.html

# create usage generated/README.h header file from README.txt
usage:
	echo "// Automatically generated file. Edit README.txt and run " > generated/README.h
	echo "// 'make usage' to update this documentation!" >> generated/README.h
	$(shell echo -en "$(RN)\n" >> generated/README.h)
	echo "const char* usage_str = \"\"" >> generated/README.h
	sed -f bin/replace$(REPLACE).sed "$(B_DIR)/doc/README.txt" >> generated/README.h
	echo \""\";" >> generated/README.h

# create example ini content header file generated/example_ini.h from example.ini
ini:
	echo "// Automatically generated file. Edit example.ini and run " > generated/example_ini.h
	echo "// 'make ini' to update this documentation!" >> generated/example_ini.h
	$(shell echo -en "$(RN)\n" >> generated/example_ini.h)
	echo "const char* ini_str = \"\"" >> generated/example_ini.h
	sed -f bin/replace$(REPLACE).sed example.ini >> generated/example_ini.h
	echo \""\";" >> generated/example_ini.h
	sed -i $(SEDI_EXT) 's/PROGNAME_SHORT/$(PROGNAME_SHORT)/g' generated/example_ini.h
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' generated/example_ini.h
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' generated/example_ini.h
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' generated/example_ini.h
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' generated/example_ini.h

error:
	echo "// Automatically generated file. Edit generated/error.html and run " > generated/error.h
	echo "// 'make ini' to update this documentation!" >> generated/error.h
	$(shell echo -en "$(RN)\n" >> generated/error.h)
	echo "const char* error_str = \"\"" >> generated/error.h
	sed -f bin/replace$(REPLACE).sed generated/error.html >> generated/error.h
	echo \""\";" >> generated/error.h
	sed -i $(SEDI_EXT) 's/PROGNAME_SHORT/$(PROGNAME_SHORT)/g' generated/error.h
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' generated/error.h
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' generated/error.h
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' generated/error.h
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' generated/error.h

reg:
	echo "const char* reg_str = \"\"" > generated/reg.h
	sed -f bin/replace$(REPLACE).sed example.reg >> generated/reg.h
	echo \""\";" >> generated/reg.h
	sed -i $(SEDI_EXT) 's/PROGNAME_SHORT/$(PROGNAME_SHORT)/g' generated/reg.h
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' generated/reg.h
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' generated/reg.h

# show todo list
todo:
	egrep -nr "FIXME|TODO" *.c *.h

release_doc:
	cp ../doc/README.txt "$(B_DIR)/doc/"
	cp ../doc/LICENSE.txt "$(B_DIR)/doc/"
	cp ../doc/INSTALL.txt "$(B_DIR)/doc/"
	cp ../doc/BUILD.txt "$(B_DIR)/doc/"
	cp example.reg "$(B_DIR)/ph.reg"
	cp example.ini "$(B_DIR)/ph.ini"
	
	# subsitute all keywords in the README.txt file
	sed -i $(SEDI_EXT) 's/PROGNAME_SHORT/$(PROGNAME_SHORT)/g' $(B_DIR)/doc/README.txt
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' $(B_DIR)/doc/README.txt
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' $(B_DIR)/doc/README.txt
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' $(B_DIR)/doc/README.txt
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' $(B_DIR)/doc/README.txt
	
	sed -i $(SEDI_EXT) 's/PROGNAME_SHORT/$(PROGNAME_SHORT)/g' $(B_DIR)/doc/INSTALL.txt
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' $(B_DIR)/doc/INSTALL.txt
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' $(B_DIR)/doc/INSTALL.txt
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' $(B_DIR)/doc/INSTALL.txt
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' $(B_DIR)/doc/INSTALL.txt
	
	sed -i $(SEDI_EXT) 's/PROGNAME_SHORT/$(PROGNAME_SHORT)/g' $(B_DIR)/doc/BUILD.txt
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' $(B_DIR)/doc/BUILD.txt
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' $(B_DIR)/doc/BUILD.txt
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' $(B_DIR)/doc/BUILD.txt
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' $(B_DIR)/doc/BUILD.txt
	
	sed -i $(SEDI_EXT) 's/PROGNAME_SHORT/$(PROGNAME_SHORT)/g' $(B_DIR)/ph.reg
	sed -i $(SEDI_EXT) 's/PROGNAME/$(PROGNAME)/g' $(B_DIR)/ph.reg
	sed -i $(SEDI_EXT) 's/STDIN_MAX/$(STDIN_MAX)/g' $(B_DIR)/ph.reg
	sed -i $(SEDI_EXT) 's/MAX_CWD_LENGTH/$(MAX_CWD_LENGTH)/g' $(B_DIR)/ph.reg
	sed -i $(SEDI_EXT) 's/_EXT/$(_EXT)/g' $(B_DIR)/ph.reg
	
	
	# windows line endings
	sed -i $(SEDI_EXT) 's/$$/\r/' $(B_DIR)/doc/README.txt
	sed -i $(SEDI_EXT) 's/$$/\r/' $(B_DIR)/doc/INSTALL.txt
	sed -i $(SEDI_EXT) 's/$$/\r/' $(B_DIR)/doc/BUILD.txt
	sed -i $(SEDI_EXT) 's/$$/\r/' $(B_DIR)/doc/LICENSE.txt
	sed -i $(SEDI_EXT) 's/$$/\r/' $(B_DIR)/ph.ini
	sed -i $(SEDI_EXT) 's/$$/\r/' $(B_DIR)/ph.reg


# create a release
release: release_doc test_httpget
	# get latest code
	#git pull
	
	# build the application
	#$(MAKE) all
	mkdir ../build/doc/integration | true
	cp ../doc/integration/* ../build/doc/integration
	
	# make release directory
	@echo $(R_DIR)/$(rel)
	#mkdir "$(R_DIR)/$(rel)" | true
	
	# copy files to release directory
	cp $(PROGNAME_SHORT)$(_EXT) "$(B_DIR)/"
	cp generated/error.html "$(B_DIR)/"
	cp testcmd$(_EXT) "$(B_DIR)/"
	cp testregex$(_EXT) "$(B_DIR)/"
	cp ../test/httpget$(_EXT) "$(B_DIR)/testhttp$(_EXT)"
	strip "$(B_DIR)/testcmd$(_EXT)"
	strip "$(B_DIR)/testregex$(_EXT)"
	strip "$(B_DIR)/testregex$(_EXT)"
	strip "$(B_DIR)/$(PROGNAME_SHORT)$(_EXT)"
	strip "$(B_DIR)/testhttp$(_EXT)"
		
	# create zip file
	#cp $(R_DIR)/$(rel)/* ../build
	#$(shell cd $(B_DIR)/; $(ZIP) -q -r ../$(rel).zip * )
	cd $(B_DIR)/ && $(ZIP) ../$(rel).zip *
	mv "../$(rel).zip" ../release 
	#$(shell test $(B_DIR)/zip.zip && mv $(B_DIR)/zip.zip "../$(rel).zip") | true
	
	
	# remove working directory
	#rm -r "$(R_DIR)/$(rel)"
	
	# add to git repo, tag release and push changes/tags
	#git add release/*.zip
	#git commit -am "Added release $(rel)"
	#git push
	#git tag $(rel) && git push --tags
	
# cleanup
clean:
	rm -r $(PROGNAME)$(_EXT) || true
	rm -r $(PROGNAME_SHORT)$(_EXT) || true
	rm -r testcmd$(_EXT) || true
	rm -r create_error.exe || true
	rm -r testcmd.exe || true
	rm -r testregex.exe || true
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
	rm ../test/* || true
	rm ../build/* || true


clearlog:
	rm ph.log || true
	rm testcmd.log || true


## TEST ########################################################################
test: dep test_cmdparser $(TEST_REALPATH) test_mydir test_nvlist test_regcpp test_httpget test_transform test_split_arg
	#g++ -std=c++11 $(CFLAGS) -o ../test/regcpp$(_EXT) lib/reg.cpp -lpcrecpp -lpcre -DCPPMAIN
	$(CC) $(CFLAGS) -DURIPARSE_DEBUG=1 -c -o lib/uriparse.o lib/uriparse.c
	$(CC) $(CFLAGS) -DURIPARSE_DEBUG=1 -o ../test/uriparse$(_EXT) lib/nvlist.o lib/stringlib.o lib/uriparse.o
	
	@echo 
	../test/cmd_parser$(_EXT)
	../test/mydir$(_EXT)
	../test/nvlist$(_EXT)
	../test/regcpp$(_EXT)
	../test/uriparse$(_EXT)
	../test/httpget$(_EXT) "https://www.example.com"
	../test/transform$(_EXT)
	../test/split_arg$(_EXT)

test_nvlist:
	$(CC) $(CFLAGS) -D NVLIST_MAIN -o ../test/nvlist$(_EXT) lib/stringlib.o lib/nvlist.c

test_mydir:
	#$(CC) $(CFLAGS) -c lib/realpath.c -o lib/realpath.o
	$(CC) $(CFLAGS) -o ../test/mydir$(_EXT) lib/stringlib.o $(REALPATH) -DMYDIR_MAIN lib/mydir.c
	
test_cmdparser:
	$(CC) $(CFLAGS) -g -DCMD_PARSER_MAIN -o ../test/cmd_parser$(_EXT) lib/stringlib.o lib/nvlist.o lib/uriparse.o lib/cmd_parser.c 
	#../test/cmd_parser$(_EXT)
	
test_ph: ph
	$(PROGNAME_SHORT)$(_EXT) 'proto:auth?query=value%201&param2&param3 '

test_split_arg:
	$(CC) $(CFLAGS) -o ../test/split_arg$(_EXT) lib/stringlib.o test/split_arg.c
	
	
uriparse: stringlib
	$(CC) $(CFLAGS) -c -o lib/nvlist.o lib/nvlist.c
	$(CC) $(CFLAGS) -DDEBUG=0 -DURIPARSE_DEBUG=1 -c -o lib/uriparse.o lib/uriparse.c
	$(CC) $(CFLAGS) -o ../test/uriparse$(_EXT) lib/nvlist.o lib/stringlib.o lib/uriparse.o
	#../test/uriparse$(_EXT)

ini_test:
	$(CC) $(CFLAGS) -c ini.c
	$(CC) $(CFLAGS) -o test/ini_test$(_EXT) ini.o test/ini_test.c

test_httpget:	
	$(CC) $(CFLAGS) \
		lib/httpget.c -o ../test/httpget$(_EXT) \
		-DSKIP_PEER_VERIFICATION -DMAIN_GETFILE  \
		-lpcrecpp -lpcre \
		-lcurl -lssl -lcrypto \
		$(LIBS) $(COMPILE_STATIC)

# example programs for library test
find_param:
	$(CC) $(CFLAGS) -c lib/stringlib.c
	$(CC) $(CFLAGS) -o test/find_param$(_EXT) stringlib.o test/find_param.c

stringlib:
	$(CC) $(CFLAGS) -c lib/stringlib.c -o lib/stringlib.o

realpath:
	$(CC) $(CFLAGS) -c lib/realpath.c -o lib/realpath.o
	$(CC) $(CFLAGS) -o ../test/realpath_test$(_EXT) lib/realpath.o test/realpath_test.c

cleanpath:
	$(CC) $(CFLAGS) -o test/cleanpath$(_EXT) test/cleanpath.c

ini_dump:
	$(CC) $(CFLAGS) -c ini.c
	$(CC) $(CFLAGS) -o ini/ini_dump$(_EXT) ini.o ini/ini_dump.c
	
ini_example:
	$(CC) $(CFLAGS) -c ini.c
	$(CC) $(CFLAGS) -o ini/ini_example$(_EXT) ini.o ini/ini_example.c

nvlist:
	#$(CC) $(CFLAGS) -c lib/stringlib.c -o lib/stringlib.o
	$(CC) $(CFLAGS) -D NVLIST_MAIN -o ../test/nvlist$(_EXT) lib/stringlib.o lib/nvlist.c
	../test/nvlist$(_EXT)

compare:
	$(CC) $(CFLAGS) -o test/compare$(_EXT) test/compare.c

test_regcpp:
	#g++ -o regcpp.exe reg.cpp -lpcrecpp -lpcre -DPCRE_STATIC -DCPPMAIN
	g++ $(CFLAGS) $(COMPILE_STATIC) -std=c++11 -DCPPMAIN -o ../test/regcpp$(_EXT) lib/reg.cpp -lpcrecpp -lpcre 

test_transform:
	$(CC) $(CFLAGS) -DTRANSFORM_MAIN -o ../test/transform$(_EXT) lib/nvlist.o lib/transform.c

test_getexe:
	$(CC) $(CFLAGS) -DGETEXE_MAIN -o ../test/getexe$(_EXT) lib/getexe.c lib/stringlib.o
