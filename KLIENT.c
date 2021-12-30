#include        <sys/types.h>   /* basic system data types */
#include        <sys/socket.h>  /* basic socket definitions */
#include        <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include        <arpa/inet.h>   /* inet(3) functions */
#include        <errno.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <unistd.h>

#define MAXLINE 1023
#define LENTOPIC 15
#define SA      struct sockaddr

void
send_sub(int sockfd)
{
	char topic[LENTOPIC];
	char fun[4] = "sub";
	char line[MAXLINE];
	//char sign[] = "_";
	printf("Enter topic to subscribe: ");
	scanf("%s", topic);
	//for (int i = 0; i < (LENTOPIC - sizeof(topic));i++) {
	//	strcat(topic, sign);
	//}
	//printf("%s\n", topic);
	strcat(fun, topic);
	snprintf(line, sizeof(line), fun);
	if (write(sockfd, line, MAXLINE) < 0);
}
void
send_pub(int sockfd)
{
	char lineTS[50];
	char topic[LENTOPIC];
	char line[MAXLINE];
	//char sign[] = "-";
	printf("Enter topic to publish:");
	scanf("%s", topic);
	///for (int i = 0; i < (LENTOPIC - sizeof(topic));i++) {
	//	strcat(topic, sign);
	//}
	//printf("%s\n", topic);
	printf("Enter msg:");
	scanf("%s", lineTS);
	snprintf(line, sizeof(line), "pub%smsg%s", topic, lineTS);
	if (write(sockfd, line, MAXLINE) < 0);
}
int
main(int argc, char** argv)
{
	int					sockfd, n;
	struct sockaddr_in6	servaddr;
	int err;

	if (argc != 2) {
		fprintf(stderr, "ERROR: usage: a.out <IPaddress>  \n");
		return 1;
	}
	if ((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "socket error : %s\n", strerror(errno));
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
	servaddr.sin6_port = htons(13);	/* daytime server */
	if ((err = inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr)) <= 0) {
		if (err == 0)
			fprintf(stderr, "inet_pton error for %s \n", argv[1]);
		else
			fprintf(stderr, "inet_pton error for %s : %s \n", argv[1], strerror(errno));
		return 1;
	}
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "connect error : %s \n", strerror(errno));
		return 1;
	}
	//////////////////////////////////////////////////////////

	char				recvline[MAXLINE + 1];
	int choose1;

	printf("Who am i?\n1.Rcv\n2.Rcv\n3.Subscriber\n4.Publisher\n5.Rcv\nChoose number: ");
	scanf("%d", &choose1);
	switch (choose1) {
	case 3:
		send_sub(sockfd);//Sending function(subscrber / publisher and topic)
		//Waiting for message
		printf("\nCzekam na dane\n");
		for (;;) {
			if ((n = read(sockfd, recvline, MAXLINE)) > 0) {
				printf("Odbieram...\n");
				recvline[n] = 0;	/* null terminate */
				if (fputs(recvline, stdout) == EOF) {
					fprintf(stderr, "fputs error : %s\n", strerror(errno));
				}
				//break;//do usuniecia
			}
		}
		break;
	case 4 :
		send_pub(sockfd);
		break;
	case 5 :
		printf("Czekam na dane\n");
		for (;;) {
				if ((n = read(sockfd, recvline, MAXLINE)) > 0) {
				printf("Odbieram...\n");
				recvline[n] = 0;	/* null terminate */
				if (fputs(recvline, stdout) == EOF) {
					fprintf(stderr, "fputs error : %s\n", strerror(errno));
				}
				//break;//do usuniecia
			}
		}
		break;
	default :
		break;

	}
	/////////////////////////////////////////////////////////////
	if (n < 0)
		fprintf(stderr, "read error : %s\n", strerror(errno));

	fflush(stdout);
	fprintf(stderr, "\nOK\n");

	exit(0);
}
