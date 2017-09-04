all:
	gcc -o protohand.exe protohand.c

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