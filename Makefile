.PHONY: stringlib ini usage test param_test release puh ph

# DUBUG=0 disables debugging, other levels from 1-3
# LOG_TO_FILE=1 will send dbg output to stdout, else to protohand.log
DEBUG=3
LOG_TO_FILE=1
VERSION=0.2.0
# naming
PROGNAME = protohand
PROGNAME_SHORT = ph
_EXT = .exe
CC = gcc
include os.mk

# create release name: OS_VERSION_TIMESTAMP
timestamp=$(shell date "+%Y%m%d%H%M")
rel = $(operating_system)_$(VERSION)_$(timestamp)

ph:
	$(CC) $(CFLAGS) -c lib/realpath.c -o lib/realpath.o
	$(CC) $(CFLAGS) -c lib/ini.c -o lib/ini.o
	$(CC) $(CFLAGS) -c lib/stringlib.c -o lib/stringlib.o
	$(CC) $(CFLAGS) -c lib/uriparse.c -o lib/uriparse.o
	$(CC) $(CFLAGS) -c lib/nvlist.c -o lib/nvlist.o
	#$(CC) $(CFLAGS) -c ph.c
	$(CC) $(CFLAGS) -o $(PROGNAME_SHORT)$(_EXT) lib/nvlist.o lib/realpath.o lib/stringlib.o lib/ini.o lib/uriparse.o ph.c

test_ph: ph
	$(PROGNAME_SHORT)$(_EXT) 'proto:auth?query=value%201&param2&param3'

# build the programm
all:
	$(CC) $(CFLAGS) -c lib/realpath.c -o lib/realpath.o
	$(CC) $(CFLAGS) -c lib/ini.c -o lib/ini.o
	$(CC) $(CFLAGS) -c lib/stringlib.c -o lib/stringlib.o
	$(CC) $(CFLAGS) -c lib/uriparse.c -o lib/uriparse.o
	$(CC) $(CFLAGS) -o $(PROGNAME)$(_EXT) lib/realpath.o lib/stringlib.o lib/ini.o lib/uriparse.o protohand.c

# auto-update documentation
doc: usage ini
	
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
	$(CC) $(CFLAGS) -o testcmd$(_EXT) testcmd.c

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
	rm -r lib/*.o || true
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
