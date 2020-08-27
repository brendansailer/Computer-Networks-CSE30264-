/*
	Brendan Sailer
	Program 1 - Computer Networks

	Run: ./updserver <Port Number>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pg1lib.h>

void server(char * argv[]);

int main(int argc, char * argv[]) {
	if (argc == 2) {
		server(argv);
	} else {
		fprintf(stderr, "udpserver <Port Number> \n");
		exit(1);
	}
}

void server(char * argv[]){
	struct sockaddr_in sin, client_addr;
	struct timeval start_time;
	struct tm *date;
	time_t t;
	unsigned long check_sum;
	char buf[BUFSIZ];
	int len, addr_len;
	int s;

	int port = atoi(argv[1]); 

	//* build address data structure */
	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY; //Use the default IP address of server
	sin.sin_port = htons(port);
	printf("Waiting on port: %d ...\n", port);

	/* Generate public key */
	char *pubKey = getPubKey();

	/* setup passive open */
	if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "udpserver: failed to open socket\n");
		exit(1);
	}
	
	if((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
		fprintf(stderr, "udpserver: failed to bind\n");
		exit(1);
	}
	
	addr_len = sizeof(client_addr);

	while (1){
		if(recvfrom(s, buf, sizeof(buf), 0,  (struct sockaddr *)&client_addr, &addr_len)==-1){
			fprintf(stderr, "udpserver: failed to receive public key\n");
    	exit(1);
		}
		printf("Public Key: %s\n", buf);
		bzero((char*)&buf, sizeof(buf));

		char* encryptedKey = encrypt(buf, pubKey);
		len = strlen(encryptedKey)
		if(sendto(s, encryptedKey, len, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr))==-1){
			fprintf(stderr, "udpserver: failed to send encrypted key\n");
    	exit(1);
		}
		
		if(recvfrom(s, buf, sizeof(buf), 0,  (struct sockaddr *)&client_addr, &addr_len)==-1){
			fprintf(stderr, "udpserver: failed to receive message\n");
    	exit(1);
		}

		gettimeofday(&start_time, NULL);
		t = start_time.tv_sec;
		date = localtime(&t);

		check_sum = checksum(buf);

		printf("*** New Message ***\n");
		printf("Received Time: %s", asctime(date));
		printf("Received Message:\n%s\n", buf);
		printf("Calculated Checksum: %lu\n", check_sum);
		printf("\n");
		bzero((char*)&buf, sizeof(buf));
	}
	close(s);
}
