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
#include <errno.h>

#define PORT 20199
#define MAXFDNUM 100
#define HEADLEN 10
#define MAXEVENTS 64

/* read n bytes from socket fd buffer*/
	ssize_t
readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;
	ptr = vptr;
	nleft = n;
	while(nleft > 0){ 
		if((nread=read(fd, ptr, nleft)) < 0){ 
			if(errno==EINTR)
				nread = 0;
			else
				return -1; 
		}else if(nread==0)
			break;
		nleft -= nread;
		ptr += nread;
	}
	return n-nleft;
}

/* write n bytes from socket fd buffer*/
	ssize_t
writen(int fd, void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	char *ptr;
	ptr = vptr;
	nleft = n;
	while(nleft>0){
		if((nwritten = write(fd, ptr, n))<=0){
			if(nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1; 
		}
		nleft -= nwritten;
		ptr += nwritten;
	}
	return n-nleft;
}

/* 
 * send a msg to socket fd
 * with msg head
 */
	int
sendMsg(int fd, void *vptr, int n)
{
	char head[HEADLEN+1];
	char *ptr = vptr;
	memset(head, 0x00, sizeof(head));
	snprintf(head, sizeof(head), "%010d", n);
	if((writen(fd, head, HEADLEN))!=HEADLEN) return -1;
	if((writen(fd, ptr, n))!=n) return -1;
	return 0;
}

/*
 * recieve a msg from socket fd
 */

	int
recvMsg(int fd, void *vptr, int *n)
{
	char head[HEADLEN+1];
	char *ptr = vptr;
	memset(head, 0x00, sizeof(head));
	if((readn(fd, head, HEADLEN))!=HEADLEN) return -1;
	*n = atoi(head);
	if((readn(fd, ptr, *n))!=*n) return -1;
	return 0;
}


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
	char msgbuf[1024] = {0};
	char msg[1024] = {0};
	char header[HEADLEN+1] = {0};
	int msglen = 0;
	int count = 0;
	int countSum = 0;

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
	event.events = EPOLLIN ; // LT
	ret = epoll_ctl(efd, EPOLL_CTL_ADD, listenfd, &event);
	if(ret==-1){
		perror("epoll ctl add listenfd err!");
		exit(-1);
	}

	events = calloc(MAXEVENTS, sizeof(event));

	while(1){
		int n,i;
		n = epoll_wait(efd, events, MAXEVENTS , -1);
		//fprintf(stdout, "debug:n[%d]\n",n);
		for(i=0;i<n;i++){
			if((events[i].events&EPOLLERR)||
					(events[i].events&EPOLLHUP)||
					(!(events[i].events&EPOLLIN))){
				fprintf(stdout, "epoll err\n");
				close(events[i].data.fd);
				continue;
			}
			else if(events[i].data.fd==listenfd){
				while(1){
					struct sockaddr_in cliaddr;
					socklen_t cliaddr_len;
					char addrbuf[INET_ADDRSTRLEN];
					int connfd;
					connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
					if(connfd<0){
						if((errno==EAGAIN)||(errno==EWOULDBLOCK))
							break;
						else{
							perror("accept err!");
							break;
						}
					}

					inet_ntop(AF_INET, &cliaddr.sin_addr, addrbuf, sizeof(addrbuf));
					fprintf(stdout,"connection from [%s] port [%d]\n", addrbuf, ntohs(cliaddr.sin_port));

					ret = setSocketNonblock(connfd);
					if(ret){
						perror("setSocketNonblock err!");
						exit(-1);
					}

					event.data.fd = connfd;
					event.events = EPOLLIN | EPOLLERR;
					ret = epoll_ctl(efd, EPOLL_CTL_ADD, connfd, &event);
					if(ret){
						perror("epoll ctl add err!");
						exit(-1);
					}
				}
				continue;
			}
			else{
				int done = 0;

				if(countSum<HEADLEN){
					memset(msgbuf, 0x00, sizeof(msgbuf));
					count = read(events[i].data.fd, msgbuf, HEADLEN-countSum);
					if(count == -1){
						if(errno!=EAGAIN||errno!=EWOULDBLOCK){
							perror("read err!");
							done = 1;
						}
					}else if(count == 0){
						done = 1;
					}else{
						strncat(header, msgbuf, count);
						countSum += count;
						if(countSum==HEADLEN){
							fprintf(stdout, "debug:header[%s]\n", header);
							msglen = atoi(header);
							memset(header, 0x00, sizeof(header));
						}
						fprintf(stdout, "debug:countSum[%d]\n", countSum);
					}
				}else{
					memset(msgbuf, 0x00, sizeof(msgbuf));
					count = read(events[i].data.fd, msgbuf, msglen-countSum+HEADLEN);
					fprintf(stdout, "debug:count[%d]\n", count);
					fprintf(stdout, "debug:msgbuf[%s]\n", msgbuf);
					if(count == -1){
						if(errno!=EAGAIN||errno!=EWOULDBLOCK){
							perror("read err!");
							done = 1;
						}
					}else if(count == 0){
						done = 1;
					}else{
						strncat(msg, msgbuf, count);
						countSum += count;
						if(countSum==msglen+HEADLEN){
							fprintf(stdout, "debug:msg[%s]\n", msg);
							//do echo 
							sendMsg(events[i].data.fd, msg, strlen(msg));
							//reset variables
							countSum = 0;
							msglen = 0;
							memset(msg, 0x00, sizeof(msg));
						}
					}
				}

				if(done){
					fprintf(stdout, "client close the connection\n");
					close(events[i].data.fd);
				}
			}
		}
	}

	free(events);

	close(listenfd);
	close(efd);
}

#define SERVERIP "127.0.0.1"
#define MAXMSGLEN 1024

void client()
{
	struct sockaddr_in servaddr;
	int socketfd;
	int ret;

	socketfd = socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_port = htons(PORT);
	inet_pton(AF_INET, (const char *)SERVERIP, &servaddr.sin_addr);
	servaddr.sin_family = AF_INET;

	ret = connect(socketfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
	if(ret){
		fprintf(stdout, "Cant connect to the server [%s]\n", SERVERIP);
		exit(-1);
	}else{
		fprintf(stdout, "Connect success!\n");
	}

	char msgbuf[MAXMSGLEN] = {0};
	int msglen;
	fprintf(stdout, "> ");
	while(fgets(msgbuf, MAXMSGLEN, stdin)){
		memset(&msgbuf[strlen(msgbuf)-1],0x00,1);
		fprintf(stdout, "debug:msgbuf[%s]\n", msgbuf);
		ret = sendMsg(socketfd, msgbuf, strlen(msgbuf));
		if(ret){
			perror("send err!");
			exit(-1);
		}

		memset(msgbuf, 0x00, sizeof(msgbuf));
		ret = recvMsg(socketfd, msgbuf, &msglen);
		if(ret){
			if(errno==EINTR){
				continue;
			}else{
				perror("recv err!");
				exit(-1);
			}
		}
		if(msglen==0){
			fprintf(stdout, "the server disconnected\n");
			break;
		}

		fprintf(stdout, "msg:[%s]\n",msgbuf);
		fprintf(stdout, "> ");
	}

	close(socketfd);
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
