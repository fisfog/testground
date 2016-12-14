/* epoll test */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>

#define PORT 20199

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

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	listen(listenfd, 20);

	close(listenfd);
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
				break;
			case 'c':
				c = 1;
				break;
			case -1:
				flag = -1;
				break;
			default:
				usage();
				break;
		}
	}
	printf("args:s=%d,c=%d\n",s,c); 
	return 0;
}
