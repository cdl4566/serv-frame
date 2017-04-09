#ifndef BASE_NET_OPERATION_H
#define BASE_NET_OPERATION_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <pthread.h>
#include <signal.h>
#include <sys/param.h>
#include <errno.h>
#include <unistd.h>


namespace bnet{
int init_listener(int port);
int accept_connection(const int listenfd);

void close_connection(int fd);

ssize_t recv_data(int fd, void *buf, size_t len);

ssize_t send_data(int fd, const void *buf, size_t len);

void set_nonblock(int fd);

ssize_t readv(int sockfd, const struct iovec *iov, int iovcnt);

}


#endif