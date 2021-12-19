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

#define	SENDRATE	5		/* send one datagram every five seconds */
void
hand_conn(int sockfd)
{
	for (;;) {
		char lineTS[50];
		char line[MAXLINE];

		printf("Enter msg:");
		scanf("%s", lineTS);
		snprintf(line, sizeof(line), "PID=%d, Msg:%s", getpid(), lineTS);
		//if (sendto(sendfd, line, strlen(line), 0, sadest, salen) < 0)
		//	fprintf(stderr, "sendto() error : %s\n", strerror(errno));
		if (write(sockfd, line, MAXLINE) < 0)
			sleep(SENDRATE);
	}
}

int
main(int argc, char** argv)
{
	int					sockfd, n;
	struct sockaddr_in6	servaddr;
	char				recvline[MAXLINE + 1];
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
	hand_conn(sockfd);
	while ((n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF) {
			fprintf(stderr, "fputs error : %s\n", strerror(errno));
			return 1;
		}
	}
	/////////////////////////////////////////////////////////////
	if (n < 0)
		fprintf(stderr, "read error : %s\n", strerror(errno));

	fflush(stdout);
	fprintf(stderr, "\nOK\n");

	exit(0);
}
