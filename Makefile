PROGNAME = protohand.exe
COPTS = -Wall -DDEBUG=1 -DPROGNAME=$(PROGNAME)

all:
	gcc $(COPTS) -c ini.c
	gcc $(COPTS) -c urldecode2.c
	gcc $(COPTS) -o $(PROGNAME) urldecode2.o ini.o protohand.c

ini_dump:
	gcc $(COPTS) -c ini.c
	gcc $(COPTS) -o ini/ini_dump.exe ini.o ini/ini_dump.c
	
ini_example:
	gcc $(COPTS) -c ini.c
	gcc $(COPTS) -o ini/ini_example.exe ini.o ini/ini_example.c

ini_test:
	gcc $(COPTS) -c ini.c
	gcc $(COPTS) -o ini_test.exe ini.o ini_test.c

compare:
	gcc $(COPTS) -o compare.exe compare.c

test:
	$(PROGNAME) "12345://67890" || true
	$(PROGNAME) "12345:///67890" || true
	$(PROGNAME) "12345://?67890" || true
	$(PROGNAME) "12345://as/67890" || true
	$(PROGNAME) "12345://as?67890" || true
	$(PROGNAME) "12345://as/xy?67890" || true
	$(PROGNAME) "12345:67890" || true
	$(PROGNAME) "12345:/67890" || true
	$(PROGNAME) "12345:?67890" || true
	$(PROGNAME) "12345:as/67890" || true
	$(PROGNAME) "12345:as?67890" || true
	$(PROGNAME) "12345:as/xy?67890" || true
	$(PROGNAME) "usb://centricity?--auth=user&pass=ab%20de" || true

clean:
	rm -r *.o *.exe || true
