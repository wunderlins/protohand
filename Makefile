all:
	gcc -o protohand.exe protohand.c

ini_dump:
	gcc -c ini.c
	gcc -o ini/ini_dump.exe ini.o ini/ini_dump.c
	
ini_example:
	gcc -c ini.c
	gcc -o ini/ini_example.exe ini.o ini/ini_example.c

test:
	echo "12345://67890" | protohand.exe || true
	echo "12345:///67890" | protohand.exe || true
	echo "12345://?67890" | protohand.exe || true
	echo "12345://as/67890" | protohand.exe || true
	echo "12345://as?67890" | protohand.exe || true
	echo "12345://as/xy?67890" | protohand.exe || true

	echo "12345:67890" | protohand.exe || true
	echo "12345:/67890" | protohand.exe || true
	echo "12345:?67890" | protohand.exe || true
	echo "12345:as/67890" | protohand.exe || true
	echo "12345:as?67890" | protohand.exe || true
	echo "12345:as/xy?67890" | protohand.exe || true

	echo "usb://centricity?--auth=user&pass=ab%20de" | protohand.exe || true