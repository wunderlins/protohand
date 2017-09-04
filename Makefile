all:
	gcc -o protohand.exe protohand.c

test:
	echo "1234567890123" | protohand.exe