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
send_time(int sockfd)
{
	time_t				ticks;
	char				buff[MAXLINE];

	ticks = time(NULL);
	snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
	if (write(sockfd, buff, strlen(buff)) < 0)
		fprintf(stderr, "write error : %s\n", strerror(errno));
	//close(connfd);
}

void
rcv_msg(int sockfd) {
	int					n;
	char				line[MAXLINE + 1];
	//for (; ; ) {
		if ((n = read(sockfd, line, MAXLINE)) < 0)
			perror("read() error");
		line[n] = 0;	/* null terminate */
		printf("%s (%d bytes)\n", line, n);
		fflush(stdout);
	//}
}
char
rcv_fun(int sockfd) {
	int					n;
	char				line[4];
	if ((n = read(sockfd, line, MAXLINE)) < 0)
		perror("read() error");
	line[n] = 0;	/* null terminate */
	printf("%s\n", line);
	fflush(stdout);
	return *line;
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
		int					n;
		char				line[4];
		struct subscribers {
			char IP[INET6_ADDRSTRLEN + 1];
			char TOPIC[10];
		};
		struct subscribers pierwszy;
		bzero(pierwszy.IP, sizeof(pierwszy.IP));
		inet_ntop(AF_INET6, (struct sockaddr*)&cliaddr.sin6_addr, pierwszy.IP, sizeof(pierwszy.IP));

		if ((childpid = fork()) == 0) {	/* child process */
			close(listenfd);	/* close listening socket */
			//Poczatek polaczenia
			printf("Connection from %s as \n", pierwszy.IP);
			//Czytanie funkcji podlaczonego adresu
			//if ((n = read(connfd, line, MAXLINE)) < 0)
			//	perror("read() error");
			//line[n] = 0;	/* null terminate */
			//printf("%s\n", line);
			//Zapis adresu do tabeli Subscribers
			
			//
			// 
			// 
			// 
			//Jeszcze dodanie zapisu do bazy jesli sub dopasowanie lista Subscribes [IP] [TOPIC] 
			//
			//
			//
			//sleep(2);
			//for (int i = 0;i < 100;i++) {
				send_time(connfd);	/* process the request */
				send_time(connfd);
			//}
			//rcv_msg(connfd);
			exit(0);
		}
		close(connfd);			/* parent closes connected socket */
		/////////////////////////////
	}
}
