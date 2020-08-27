/*
	Brendan Sailer
	Program 1 - Computer Networks

	Run: ./udpclient <Host Name> <Port Number> <Text to Send>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pg1lib.h>

#define SERVER_PORT 41042

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
	char buf[BUFSIZ];
	int s, len;
	unsigned long check_sum;

	/* translate host name into peer's IP address */
	host = argv[1];
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
	if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "udpclient: failed to open socket\n");
		exit(1);
	}

	/* Generate and send public key */
	char *pubKey = getPubKey();
	len = strlen(pubKey) + 1;
	if(sendto(s, pubKey, len, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr))==-1){
		fprintf(stderr, "udpclient: failed to send public key\n");
		exit(1);
	}
	
	/* Receive and decrypt public key */
	if(recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, sizeof(sin))==-1){
		fprintf(stderr, "udpclient: failed to receive encrypted key\n");
		exit(1);
	}

	char* input = argv[3];
	FILE* inputFile = fopen(input, "r");

	if(inputFile){
		printf("Sending file\n");
		if(fread(buf, sizeof(char), BUFSIZ, inputFile) <= 0){
			fprintf(stderr, "udpclient: failed to read from file\n");
			exit(1);
		}
		fclose(inputFile);
	} else{
		printf("Sending string\n");
		memcpy(buf, input, sizeof(input));
	}
	buf[BUFSIZ-1] = '\0';

	check_sum = checksum(buf);
	printf("Checksum sent: %lu\n", check_sum);	
	
	len = strlen(buf) + 1;
	if(sendto(s, buf, len, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr))==-1){
		fprintf(stderr, "udpclient: failed to send data\n");
		exit(1);
	}

	shutdown(s, 0);
	/*
	// main loop: get and send lines of text
	while (fgets(buf, sizeof(buf), stdin)) {
		buf[BUFSIZ-1] = '\0';
		if (!strncmp(buf,"Exit",4)){
			break;
		}
		len = strlen(buf) + 1;
		if(sendto(s, buf, len, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr))==-1){
			fprintf(stderr, "udpclient: failed to send data\n");
			exit(1);
		}
		printf("length: %d\n", len);
	} */
}
