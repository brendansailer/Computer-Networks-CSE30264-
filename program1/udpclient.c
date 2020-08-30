/*
	Brendan Sailer
	Netid: bsailer1
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
	struct hostent *hp;
	struct sockaddr_in sin;
	char buf[BUFSIZ];
	char server_key[BUFSIZ];
	char check_sum_buf[BUFSIZ];
	int s;

	/* Translate host name into peer's IP address */
	char * host = argv[1];
	hp = gethostbyname(host);
	if(!hp) {
		fprintf(stderr, "udpclient: unknown host: %s\n", host);
		exit(1);
	}

	/* Build address data structure */
	int server_port = atoi(argv[2]);
	if(server_port == 0){ // atoi returns 0 on bad input
		fprintf(stderr, "udpclient: bad port address - please enter an integer port\n");
		exit(1);
	}

	bzero((char *)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
	sin.sin_port = htons(server_port);

	/* Active open */
	if((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "udpclient: failed to open socket\n");
		exit(1);
	}

	/* Generate and send public key */
	char *pubKey = getPubKey();
	int len = strlen(pubKey) + 1;
	if(sendto(s, pubKey, len, 0, (struct sockaddr *)&sin, sizeof(struct sockaddr))==-1){
		fprintf(stderr, "udpclient: failed to send public key\n");
		exit(1);
	}
	
	/* Receive and decrypt public key */
	int addr_len = sizeof(sin);
	if(recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &addr_len)==-1){
		fprintf(stderr, "udpclient: failed to receive encrypted key\n");
		exit(1);
	}
	memcpy(server_key, buf, sizeof(buf));
	bzero((char*)&buf, sizeof(buf));
	char * server_key_decrypted = decrypt(server_key);

	/* Load the file or string into the buffer */
	char* input = argv[3];
	FILE* inputFile = fopen(input, "r");

	if(inputFile){ // Send a file
		if(fread(buf, sizeof(char), BUFSIZ, inputFile) <= 0){
			fprintf(stderr, "udpclient: failed to read from file\n");
			exit(1);
		}
		fclose(inputFile);
	} else { // Send a string from stdin
		memcpy(buf, input, strlen(input));
	}
	buf[BUFSIZ-1] = '\0';

	/* Calculate checksum and send it */
	unsigned long check_sum = checksum(buf);
	printf("Checksum sent: %lu\n", check_sum);
	if(snprintf(check_sum_buf, sizeof(check_sum_buf), "%lu", check_sum) < 0){
		fprintf(stderr, "udpclient: failed to snprintf checksum\n");
		exit(1);
	}
	if(sendto(s, check_sum_buf, sizeof(check_sum_buf), 0, (struct sockaddr *)&sin, sizeof(struct sockaddr))==-1){
		fprintf(stderr, "udpclient: failed to send checksum\n");
		exit(1);
	}

	/* Get the start time */	
	struct timeval start_time;
	gettimeofday(&start_time, NULL);

	/* Encrypt the message and send it */
	char * encrypted_msg = encrypt(buf, server_key_decrypted);
	if(sendto(s, encrypted_msg, strlen(encrypted_msg), 0, (struct sockaddr *)&sin, sizeof(struct sockaddr))==-1){
		fprintf(stderr, "udpclient: failed to send the encrypted data\n");
		exit(1);
	}
	bzero((char*)&buf, sizeof(buf));
	
	/* Receive response message from server */
	if(recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &addr_len)==-1){
		fprintf(stderr, "udpclient: failed to receive encrypted key\n");
		exit(1);
	}

	/* Get the end time */	
	struct timeval end_time;
	gettimeofday(&end_time, NULL);

	/* Check if the message was received properly by the server*/
	if(strcmp(buf, "0")==0){ // String matches 0, which indicates an error
		fprintf(stderr, "udpclient: server's calculated checksum does not match sent checksum\n");
		exit(1);
	} else { // Successfully sent the message and got a response confirming it - print RTT
		printf("Server has successfully received the message at: \n%s", buf);
		printf("RTT: %ldus\n", (end_time.tv_sec - start_time.tv_sec)*1000000 + (end_time.tv_usec - start_time.tv_usec));
	}
	bzero((char*)&buf, sizeof(buf));

	shutdown(s, 0);
}
