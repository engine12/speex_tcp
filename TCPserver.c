#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



int main(int argc, char * argv[])
{
    char buf[10];
    int s, n, ns, len;
    struct sockaddr_in name;
	unsigned int portNum;  //time in seconds

    switch(argc) {
        case 2:
			portNum = atoi(argv[1]);
			break;
			
        default:
			printf("Usage: %s [port_num]\n",argv[0]);
			exit(-1);
    }
	
    /* Create the socket. */
    s = socket(AF_INET, SOCK_STREAM, 0);

    /* Create the address of the server. */
    name.sin_family = AF_INET;
    name.sin_port = htons(portNum);
    name.sin_addr.s_addr = htonl(INADDR_ANY); /* Use the wildcard address.*/
    len = sizeof(struct sockaddr_in);

    /* Bind the socket to the address. */
    bind(s, (struct sockaddr *) &name, len);

	for(;;)
	{
		/* Listen for connections.  */
		listen(s, 10);

		/* Accept a connection.  */
		ns = accept(s, (struct sockaddr *) &name, &len);

		/* Read from the socket until end-of-file and
		 * print what we get on the standard output.  */
	//	printf("Connection from : %s\n",inet_ntoa(name.sin_addr));
		
		while ((n = recv(ns, buf, sizeof(buf), 0)) > 0)
			write(1, buf, n);

		close(ns);
	}
	
    close(s);
    exit(0);
}
