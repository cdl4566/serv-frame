#include "netopr.h"

int bnet::init_listener(int port){
	struct sockaddr_in	servaddr;
	unsigned int value = 0x1;
	int listenfd;

	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1){
		return -1;
	}
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);	
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,(void *)&value,sizeof(value));
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)) == -1){
    	return -1;
    }
	if(listen(listenfd, 30)== -1){
    	return -1;
    }
	set_nonblock(listenfd);
	return listenfd;
}

int bnet::accept_connection(const int listenfd)
{
	struct sockaddr_in client_addr;
	socklen_t sin_size = sizeof(client_addr);
	int newfd = -1;

	if((newfd =accept(listenfd,(struct sockaddr *)&client_addr,&sin_size))==-1){
		return -1;
	}
	set_nonblock(newfd);
	return newfd;
}

void bnet::close_connection(int fd)
{
	close(fd);
}

ssize_t bnet::recv_data(int fd, void *buf, size_t len)
{
	int	rc = recv(fd, buf, len, 0);
	return rc;
}

ssize_t bnet::send_data(int fd, const void *buf, size_t len)
{
	int rc = send(fd, buf, len, 0);
}

void bnet::set_nonblock(int fd)
{
	int flags = ::fcntl(fd, F_GETFL, 0);  
	flags |= O_NONBLOCK;  
	int ret = ::fcntl(fd, F_SETFL, flags);

	//void(ret);
}

ssize_t bnet::readv(int sockfd, const struct iovec *iov, int iovcnt)
{  
	return ::readv(sockfd, iov, iovcnt);
}

