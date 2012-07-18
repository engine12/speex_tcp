#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char * argv[])
{
    int n, s, len;
    char buf[1024];
    char hostname[64];
    struct hostent *hp;
    struct sockaddr_in name;
	unsigned int portNum;  

    switch(argc) {

		case 3:
			portNum = atoi(argv[2]);
			strcpy(hostname, argv[1]);
			break;

				
        default:
			printf("Usage: %s [server_name] [port_num]\n",argv[0]);
			exit(-1);
    }
   
    /* Look up our host's network address.*/
    hp = gethostbyname(hostname);

    /* Create a socket in the INET domain.*/
    s = socket(AF_INET, SOCK_STREAM, 0);

    /* Create the address of the server. */
    name.sin_family = AF_INET;
    name.sin_port = htons(portNum);
    memcpy(&name.sin_addr, hp->h_addr_list[0], hp->h_length);
    len = sizeof(struct sockaddr_in);

    /* Connect to the server. */
    connect(s, (struct sockaddr *) &name, len);

    /* Read from standard input, and copy the
     * data to the socket. */
    while ((n = read(0, buf, sizeof(buf))) > 0) {
        if (send(s, buf, n, 0) < 0) {
            perror("send");
            exit(1);
        }
    }

    close(s);
    exit(0);
}
