#include "server.h"
#include "tcp_server.h"

Server::Server(EventLoop *loop, const int port):
									tcpServer_(loop, port, "myServer")
{
	
}

Server::~Server()
{
	
}

void Server::start()
{
	tcpServer_.start();
}