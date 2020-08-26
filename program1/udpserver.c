#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pg1lib.h>

#define SERVER_PORT 41042
#define MAX_LINE 256
#define MAX_PENDING 5

void server(char * argv[]);

int main(int argc, char * argv[]) {
	if (argc == 2) {
		server(argv);
	} else {
		fprintf(stderr, "udpclient <Host Name> <Port Number> <Text to Send>\n");
		exit(1);
	}
}

void server(char * argv[]){
	struct sockaddr_in sin, client_addr;
	char buf[MAX_LINE];
	int len,addr_len;
	int s;

	//* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY; //Use the default IP address of server
	sin.sin_port = htons(SERVER_PORT);
	printf("Waiting ...\n");

	/* setup passive open */
	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "udpclient: failed to open socket\n");
		exit(1);
	}

	if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		fprintf(stderr, "udpclient: failed to bind\n");
		exit(1);
	}

	addr_len = sizeof (client_addr);

	while (1){
		printf("TEST123");
		if(recvfrom(s, buf, sizeof(buf), 0,  (struct sockaddr *)&client_addr, &addr_len)==-1){
			fprintf(stderr, "udpclient: failed to receive message\n");
    	exit(1);
		}
		printf("Server Recived:%s\n", buf);
		bzero((char*)&buf, sizeof(buf));
	}
	close(s);
}
