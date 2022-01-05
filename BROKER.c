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
#include 		<pthread.h>



#define MAXLINE 1024
#define LENTOPIC 4

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


static void*
doit(void* arg)
{
	//pthread_detach(pthread_self());//niezale¿ny process 
	struct sockaddr_in6	servaddr, cliaddr;
	int					n;
	char				line[4] = "";
	char				keywordSub[4] = "sub";
	char				keywordPub[4] = "pub";
	char				fun[4] = "";
	char				topic[LENTOPIC] = "";
	char				topicsub[LENTOPIC] = "";
	//char*				pointTopic =(char *) malloc(LENTOPIC * sizeof(char));
	char				message[MAXLINE];// = "";
	//char* pointMessage = (char*)malloc(MAXLINE * sizeof(char));
	int					lenmsg = 0;
	char				buff[MAXLINE];
	/*
	//BUFOROWANIE
	struct DB {
		int				primKey;
		char			topic[LENTOPIC];
		char			storedMessage[MAXLINE];
		//struct DB*		wsk;
	};
	struct DB*			baza;
	//baza->primKey = 0;
	*/
	struct subscribers {
		char IP[INET6_ADDRSTRLEN + 1];
		char MSG[MAXLINE];
	};
	char				MSG[MAXLINE];
	struct subscribers	pierwszy;
	bzero(pierwszy.IP, sizeof(pierwszy.IP));
	inet_ntop(AF_INET6, (struct sockaddr*)&cliaddr.sin6_addr, pierwszy.IP, sizeof(pierwszy.IP));

	int		connfd;

	connfd = *((int*)arg);
	//free(arg);

	

	//printf("Connection from %s as ", pierwszy.IP);
	/*
	* opis dotyczy dziwnego dzialania ponizszego if'a wpisujemy
	*		sizeof(pierwszy.MSG) to jest inaczej odbieram.... i dwie wiadomosci
	*		MAXLINE to dwa razy (odbieram.... i wiadomosci)
	*/
	if ((n = read(connfd, pierwszy.MSG, sizeof(pierwszy.MSG))) < 0)
		perror("read() error");
	pierwszy.MSG[n] = 0;	/* Enter sign of end of line */

	//Retrieved functionality from message (publisher or subscriber)

	for (int i = 0; i < 3; i++) {
		fun[i] = pierwszy.MSG[i];
	}

	//Connection from SUBSCRIBER
	if (0 == (strcmp(&(fun[0]), &(keywordSub[0])))) {
		printf("Subscriber in topic ");

		//Retrieved topic from message

		for (int i = 3; i < (LENTOPIC + 3); i++) {
			//printf("%c", pierwszy.MSG[i]);
			topicsub[i - 3] = pierwszy.MSG[i];
		}
		printf("%s", topicsub);
		if (0 == (strcmp(&(topic[0]), &(topicsub[0])))) {
			printf("\n%s\n", message);
			snprintf(buff, sizeof(buff), "MSG: %s\r\n", message);
		}
		else {
			snprintf(buff, sizeof(buff), "Empty buff\r\n");
		}
		printf("\n");
		//Zapis adresu do tabeli Subscribers || 
		//Odczytanie buffora w formacie (temat;;wiadomoœæ)
		//DO NAPISANIA
		send_time(connfd);
		sleep(2);
		if (write(connfd, buff, strlen(buff)) < 0)
			fprintf(stderr, "write error : %s\n", strerror(errno));
		//close(connfd);

		//exit(0);
	}
	//Connection from PUBLISHER
	if (0 == (strcmp(&(fun[0]), &(keywordPub[0])))) {
		printf("Publisher in topic ");
		//Retrieved topic from message
		for (int i = 3; i < (LENTOPIC + 2); i++) {
			topic[i - 3] = pierwszy.MSG[i];
			//(*pointTopic[i - 3]) = pierwszy.MSG[i];
			//pointTopic[i] = pierwszy.MSG[i];
		}
		//pointTopic = pierwszy.MSG;
		printf("%s", topic);
		//printf("%s", *pointTopic);
		//Wypisanie odebranej wiadomosci do znaku '0' lub MAXLINE przez co znamy 
		//dlugosc efektywnej wiadomoœci
		for (lenmsg = (LENTOPIC + 2); lenmsg < MAXLINE; lenmsg++) {
			if (pierwszy.MSG[lenmsg] == '0')
				break;
		}
		//Zapisanie urzytecznej wiadomosci do zmiennej message
		for (int i = (LENTOPIC + 2); i < lenmsg; i++) {
			if (pierwszy.MSG[i] == '0')
				break;
			message[i - 6] = pierwszy.MSG[i];
			//pointMessage[i] = pierwszy.MSG[i];
		}
		//Uzupelnienie bazy o widomosc do tematu
		printf("\nSending message: X%sX", message);
		//printf("\nSending message : X%sX", *pointMessage);

		/*BUFOROWANIE
		* 1.Sprawdzenie czy istniej ju¿ taki temat a jesli nie to powiekszyc pamiec
		* 2.dopisac do bazy
		*/
		//baza = (struct DB*) malloc(sizeof(struct baza));
		/*if (strcmp(&(baza->topic[0]), &(topic[0])) != 0) {
			printf("\nReserves memory for new topic");
			baza = (struct DB*)realloc(baza, (baza->primKey + 1) * sizeof(*baza));
		}*/
		/*
		printf("\nWriting data to buffor");
		//baza->topic = topic;
		//baza->storedMessage = message;
		strcat(baza->topic, topic);
		strcat(baza->storedMessage, message);
		printf("\n\nSaved data \nTopic: %s\nMessage: %s", baza->topic, baza->storedMessage);
		*/
		printf("\n");
		//exit(0);
		
	}

	close(connfd);			/* parent closes connected socket */
	//free(pointMessage);
	//pthread_detach(pthread_self());
	pthread_exit(NULL);
}

int
main(int argc, char** argv)
{
	int				listenfd, connfd;
	socklen_t			len;
	char				buff[MAXLINE], str[INET6_ADDRSTRLEN + 1];
	struct sockaddr_in6	servaddr, cliaddr;
	pid_t				childpid;
	pthread_t			tid;

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
		char				line[4]="";
		char				keywordSub[4] = "sub";
		char				keywordPub[4] = "pub";
		char				fun[4]="";
		char				topic[LENTOPIC] = "";
		//char*				pointTopic =(char *) malloc(LENTOPIC * sizeof(char));
		char				message[MAXLINE];// = "";
		char*				pointMessage = (char*) malloc(MAXLINE * sizeof(char));
		int					lenmsg = 0;
		/*
		//BUFOROWANIE
		struct DB {
			int				primKey;
			char			topic[LENTOPIC];
			char			storedMessage[MAXLINE];
			//struct DB*		wsk;
		};
		struct DB*			baza;
		//baza->primKey = 0;
		*/
		struct subscribers {
			char IP[INET6_ADDRSTRLEN + 1];
			char MSG[MAXLINE];
		};
		char				MSG[MAXLINE];
		struct subscribers	pierwszy;
		bzero(pierwszy.IP, sizeof(pierwszy.IP));
		inet_ntop(AF_INET6, (struct sockaddr*)&cliaddr.sin6_addr, pierwszy.IP, sizeof(pierwszy.IP));


		//free(arg);



		printf("Connection from %s as ", pierwszy.IP);


		//clilen = sizeof(cliaddr);
		//connfd = malloc(sizeof(int));
			int ret;
			if (ret = pthread_create(&tid, NULL, &doit, &connfd) != 0) {
				errno = ret;
				perror("pthread_create() error");
			}
	}
}
