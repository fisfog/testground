/* epoll test */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#include <fcntl.h>

#include <unistd.h>

#define PORT 20199
#define MAXFDNUM 100

/*
*	将描述字设置成非阻塞模式
*/
int setSocketNonblock(int sfd)
{
	int flags, ret;
	flags = fcntl(sfd, F_GETFL, 0);
	if(flags==-1){
		perror("fcntl get fd flag error");
		return -1;
	}

	flags |= O_NONBLOCK;
	ret = fcntl(sfd, F_SETFL, flags);
	if(ret==-1){
		perror("fcntl set fd flag error");
		return -1;
	}
	return 0;
}

void server()
{
	struct sockaddr_in servaddr, cliaddr;
	int listenfd;
	int efd;
	int ret;
	struct epoll_event event;
	struct epoll_event *events;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

	ret = setSocketNonblock(listenfd);
	if(ret){
		perror("setSocketNonBlock err!");
		exit(-1);
	}

	ret = listen(listenfd, 20);
	if(ret){
		perror("listen fd err!");
		exit(-1);
	}


	efd = epoll_create(MAXFDNUM);
	if(efd==-1){
		perror("epoll create err!");
		exit(-1);
	}

	event.data.fd = listenfd;
	event.events = EPOLLIN | EPOLLET;
	ret = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &event);
	if(ret==-1){
		perror("epoll ctl add listenfd err!");
		exit(-1);
	}

	while(1){
		int n,i;
		n = epoll_wait(efd, events, MAXFDNUM, 0);
		for(i=0;i<n;i++){
			if((events[i].events&EPOLLERR)||
				(events[i].events&EPOLLHUP)||
				!(events[i].events&EPOLLIN)){
				fprintf(stderr, "epoll err\n");
				close(event[i].data.fd);
				continue;
			}
			else if()
		}
	}


	
	close(listenfd);
	close(efd);
}

void client()
{
	struct sockaddr_in servaddr;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_port = htons(PORT);
}

void usage()
{
	printf("usage: epolltest [-s|-c]\n");
	exit(0);
}

int main(int argc, char **argv)
{
	int flag = 0;
	int s=0,c=0;
	if(argc==1){
		usage();
	}

	while(flag!=-1){
		switch(getopt(argc, argv, "sc")){
			case 's':
				s = 1;
				server();
				break;
			case 'c':
				c = 1;
				client();
				break;
			case -1:
				flag = -1;
				break;
			default:
				usage();
				break;
		}
	}
	//printf("args:s=%d,c=%d\n",s,c); 
	return 0;
}
