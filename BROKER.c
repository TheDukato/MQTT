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
		char				keywordSub1[4] = "sub";
		char				keywordSub2[4] = "sub";
		struct subscribers {
			char IP[INET6_ADDRSTRLEN + 1];
			char TOPIC[MAXLINE];
		};
		char				MSG[MAXLINE];
		struct subscribers pierwszy;
		bzero(pierwszy.IP, sizeof(pierwszy.IP));
		inet_ntop(AF_INET6, (struct sockaddr*)&cliaddr.sin6_addr, pierwszy.IP, sizeof(pierwszy.IP));

		if ((childpid = fork()) == 0) {	/* child process */
			close(listenfd);	/* close listening socket */
			//Poczatek polaczenia
			printf("Connection from %s as ", pierwszy.IP);
			//Czytanie funkcji podlaczonego adresu
			/*
			* opis dotyczy dziwnego dzialania ponizszego if'a wpisujemy
			*		sizeof(pierwszy.TOPIC) to jest inaczej odbieram.... i dwie wiadomosci
			*		MAXLINE to dwa razy (odbieram.... i wiadomosci)
			*/
			if ((n = read(connfd, pierwszy.TOPIC, MAXLINE)) < 0)
				perror("read() error");
			//
			// 
			// 
			//Porownanie typu polaczenia czy sub czy pub |
			//											 |
			//											 |
			// 
			//printf("%i", strcmp(&(pierwszy.TOPIC[0]), &(keywordSub1[0])));//funkcja do porownania stringow
			printf("%i", strcmp(&(pierwszy.TOPIC[0]), &(keywordSub1[0])));
			//printf("%i", strcmp(&(keywordSub2[0]), &(keywordSub1[0])));//
			if (0==(strcmp(&(pierwszy.TOPIC[0]), &(keywordSub1[0])))) {///////////////////////////POROWNAIE STRING / CHAR
				printf("Werification completed\n");
				//Zapis adresu do tabeli Subscribers
				//DO NAPISANIA
			}
			//printf("%c", pierwszy.TOPIC[0]);
			//printf("%c", pierwszy.TOPIC[1]);
			//printf("%c\n", pierwszy.TOPIC[2]);
			pierwszy.TOPIC[n] = 0;	/* null terminate */
			for (int i = 0; i < 15; i++) {
				printf("%c", pierwszy.TOPIC[i]);
			}
			printf(" in topic: ");
			for (int i = 3; i < 30; i++) {
				printf("%c", pierwszy.TOPIC[i]);
				//if (pierwszy.TOPIC[i] == "\0") {///////////////////////////POROWNAIE STRING / CHAR
				//	break;
				//}
			}
///////			Operating Publisher
			/*for (;;) {
				if ((n = read(connfd, MSG, MAXLINE)) > 0) {
					printf("Odbieram...\n");
					MSG[n] = 0;	
					if (fputs(MSG, stdout) == EOF) {
						fprintf(stderr, "fputs error : %s\n", strerror(errno));
					}
					//break;//do usuniecia
				}
			}*/

//////
			printf("\n");
			/* process the request */

			send_time(connfd);	
			send_time(connfd);
			sleep(2);
			send_time(connfd);
			sleep(5);
			send_time(connfd);

			exit(0);
		}
		close(connfd);			/* parent closes connected socket */
		/////////////////////////////
	}
}
