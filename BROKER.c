#include        <sys/types.h>   /* basic system data types */
#include        <sys/socket.h>  /* basic socket definitions */
#include        <sys/time.h>    /* timeval{} for select() */
#include        <time.h>                /* timespec{} for pselect() */
#include        <netinet/in.h>  /* sockaddr_in{} and other Internet defns */
#include        <arpa/inet.h>   /* inet(3) functions */
#include        <errno.h>
#include        <fcntl.h>               /* for nonblocking */
#include        <netdb.h>
#include        <signal.h>
#include        <stdio.h>
#include        <stdlib.h>
#include        <string.h>
#include        <unistd.h>



#define MAXLINE 1024

//#define SA struct sockaddr

#define LISTENQ 4

void
time_conn(int sockfd)
{
	time_t				ticks;
	char				buff[MAXLINE];

	ticks = time(NULL);
	snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
	if (write(sockfd, buff, strlen(buff)) < 0)
		fprintf(stderr, "write error : %s\n", strerror(errno));
	//close(connfd);
}
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
		if (write(socket, line, MAXLINE) < 0)
			sleep(SENDRATE);
	}
}

int
main(int argc, char** argv)
{
	int				listenfd, connfd;
	socklen_t			len;
	char				buff[MAXLINE], str[INET6_ADDRSTRLEN + 1];
	struct sockaddr_in6	servaddr, cliaddr;
	pid_t				childpid;

	if ((listenfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "socket error : %s\n", strerror(errno));
		return 1;
	}
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin6_family = AF_INET6;
	if (argc == 1)
		servaddr.sin6_addr = in6addr_any;
	else {
		if (inet_pton(AF_INET6, argv[1], &servaddr.sin6_addr) != 1) {
			printf("ERROR: Address format error\n");
			return -1;
		}
	}
	servaddr.sin6_port = htons(13);	/* daytime server */
	if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "bind error : %s\n", strerror(errno));
		return 1;
	}
	if (listen(listenfd, LISTENQ) < 0) {
		fprintf(stderr, "listen error : %s\n", strerror(errno));
		return 1;
	}
	fprintf(stderr, "Waiting for clients ... \n");
	for (; ; ) {
		len = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &len)) < 0) {
			fprintf(stderr, "accept error : %s\n", strerror(errno));
			continue;
		}
		////////////////////////////
		bzero(str, sizeof(str));
		inet_ntop(AF_INET6, (struct sockaddr*)&cliaddr.sin6_addr, str, sizeof(str));
		printf("Connection from %s\n", str);

		if ((childpid = fork()) == 0) {	/* child process */
			close(listenfd);	/* close listening socket */
			hand_conn(connfd);	/* process the request */
			time_conn(connfd);
			exit(0);
		}
		close(connfd);			/* parent closes connected socket */
		/////////////////////////////
	}
}
