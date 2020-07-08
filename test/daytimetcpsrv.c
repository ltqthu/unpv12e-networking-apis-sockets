#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<time.h>

#define MAXLINE 4096
#define LISTENQ 1024

int main(int argc, char **argv) {
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
	servaddr.sin_port        = htons(13);

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        printf("bind error\n");
        exit(1);
    }

	if (listen(listenfd, LISTENQ) < 0) {
        printf("listen error\n");
        exit(1);
    }

	for ( ; ; ) {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        if (connfd < 0) {
            printf("connfd < 0");
        }
        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
        if (write(connfd, buff, strlen(buff)) != strlen(buff)) {
            printf("write error");
        }

	    if (close(connfd) == -1) {
            printf("close error");
        }
	}
}
