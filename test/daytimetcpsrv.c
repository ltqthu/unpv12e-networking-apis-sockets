#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include"../lib/unp.h"

#define MAXLINE 4096
#define LISTENQ 1024

int main(int argc, char **argv)
{
	int					listenfd, connfd;
	struct sockaddr_in	servaddr;
	char				buff[MAXLINE];
	time_t				ticks;

    if (argc != 2) {
        printf("usage: a.out <IPaddress>\n");
        exit(1);
    }

    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket error\n");
        exit(1);
    }

    memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(13);	/* daytime server */

	// Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("bind error\n");
        exit(1);
    }

	// Listen(listenfd, LISTENQ);
	if (listen(listenfd, LISTENQ) < 0) {
        printf("listen error\n");
        exit(1);
    }

	for ( ; ; ) {
		connfd = Accept(listenfd, (SA *) NULL, NULL);

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        Write(connfd, buff, strlen(buff));

		Close(connfd);
	}
}
