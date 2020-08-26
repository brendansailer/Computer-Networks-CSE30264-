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

void client(char * argv[]);

int main(int argc, char * argv[]) {
	if(argc == 4){
		client(argv);
	} else {
		fprintf(stderr, "udpclient <Host Name> <Port Number> <Text to Send>\n");
		exit(1);
	}
}

void client(char * argv[]){
	FILE *fp;
	struct hostent *hp;
	struct sockaddr_in sin;
	char *host;
	char buf[MAX_LINE];
	int s, len;

	//host = argv[1];
	host = "student04.cse.nd.edu";

	/* translate host name into peer's IP address */
	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "udpclient: unknown host: %s\n", host);
		exit(1);
	}

	/* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(SERVER_PORT);

	/* active open */
	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "udpclient: failed to open socket\n");
		exit(1);
	}
	printf("Welcome to udplient, type \'Exit\' to quit.\n");

	/* main loop: get and send lines of text */
	while (fgets(buf, sizeof(buf), stdin)) {
		buf[MAX_LINE-1] = '\0';
		if (!strncmp(buf,"Exit",4)) break;
			len = strlen(buf) + 1;
		if(sendto(s, buf, len, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr))==-1){
			perror ("Client Send Error!\n");
			exit(1);
		}
	}
}
