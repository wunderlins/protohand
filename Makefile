COPTS = -Wall -std=c99

all:
	gcc $(COPTS) -o protohand.exe protohand.c

ini_dump:
	gcc $(COPTS) -c ini.c
	gcc $(COPTS) -o ini/ini_dump.exe ini.o ini/ini_dump.c
	
ini_example:
	gcc $(COPTS) -c ini.c
	gcc $(COPTS) -o ini/ini_example.exe ini.o ini/ini_example.c

ini_test:
	gcc $(COPTS) -c ini.c
	gcc $(COPTS) -o ini_test.exe ini.o ini_test.c

test:
	protohand.exe "12345://67890" || true
	protohand.exe "12345:///67890" || true
	protohand.exe "12345://?67890" || true
	protohand.exe "12345://as/67890" || true
	protohand.exe "12345://as?67890" || true
	protohand.exe "12345://as/xy?67890" || true
	protohand.exe "12345:67890" || true
	protohand.exe "12345:/67890" || true
	protohand.exe "12345:?67890" || true
	protohand.exe "12345:as/67890" || true
	protohand.exe "12345:as?67890" || true
	protohand.exe "12345:as/xy?67890" || true
	protohand.exe "usb://centricity?--auth=user&pass=ab%20de" || true
