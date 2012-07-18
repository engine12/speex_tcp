CFLAGS+=-L. -L.. -O2

all: tcpclient tcpserver


tcpclient: TCPclient.c
	$(CC) -o TCPclient TCPclient.c

tcpserver: TCPserver.c
	$(CC) -o TCPserver TCPserver.c
	
clean:
	rm -f *.o tcpclient tcpserver
