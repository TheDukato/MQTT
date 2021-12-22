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
#define SA      struct sockaddr

void
send_sub(int sockfd)
{
	char line[4];
	snprintf(line, sizeof(line), "sub");
	if (write(sockfd, line, MAXLINE) < 0);
	printf("Sending function as Subscriber\n");
}
void
send_pub(int sockfd)
{
	char line[4];
	snprintf(line, sizeof(line), "pub");
	if (write(sockfd, line, MAXLINE) < 0);
	printf("Sending function as Publisher\n");
}
void
send_msg(int sockfd)
{
	//for (;;) {
		char lineTS[50];
		char topic[10];
		char line[MAXLINE];

		printf("Enter topic:");
		scanf("%s", topic);
		printf("Enter msg:");
		scanf("%s", lineTS);
		snprintf(line, sizeof(line), "Topic=%s, PID=%d, Msg:%s",topic, getpid(), lineTS);
		if (write(sockfd, line, MAXLINE) < 0);
	//}
}
void
rcv_msg(int sockfd) {
	int					n;
	char				line[MAXLINE + 1];
	//for (; ; ) {
	//if ((n = read(sockfd, line, MAXLINE)) < 0)
	//	perror("read() error");
	//line[n] = 0;	/* null terminate */
	//printf("%s (%d bytes)\n", line, n);
	//printf("%s\n", line, n);
	//fflush(stdout);
	//}
	char				recvline[MAXLINE + 1];







	//oczekiwanie na przyjêcie danych 
	printf("Zaczynam czekac\n");
	for (;;) {
		if ((n = read(sockfd, line, MAXLINE)) > 0) {
			printf("Koniec czekania\n");
			break;
		}
	}
	//odbior
	printf("Odbieram...\n");
	while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF) {
			fprintf(stderr, "fputs error : %s\n", strerror(errno));
		}
	}
	/*
	while ((n = read(sockfd, line, MAXLINE)) > 0) {
		line[n] = 0;	// null terminate 
		if (fputs(line, stdout) == EOF) {
			fprintf(stderr, "fputs error : %s\n", strerror(errno));
		}
		printf("%s (%d bytes)\n", line, n);
	}
	*/
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

	//char				recvline[MAXLINE + 1];


	int choose1;
	printf("Who am i?\n1.Publisher\n2.Subscriber\nChoose number: ");
	scanf("%d", &choose1);
	switch (choose1) {
	case 1 :
		send_pub(sockfd);
		send_msg(sockfd);
		break;
	case 2 :
		//send_sub(sockfd);
		
//		while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
//			recvline[n] = 0;	/* null terminate */
//			if (fputs(recvline, stdout) == EOF) {
//				fprintf(stderr, "fputs error : %s\n", strerror(errno));
//				return 1;
//			}
//		}
		rcv_msg(sockfd);
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
