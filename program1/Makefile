all: udpserver udpclient

udpclient: udpclient.c pg1lib.h
	gcc -o udpclient udpclient.c -lcrypto -lz -I.

udpserver: udpserver.c pg1lib.h
	gcc -o udpserver udpserver.c -lcrypto -lz -I.

clean:
	rm udpclient
	rm udpserver
