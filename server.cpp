#include "server.h"
#include "tcp_server.h"
#include "application/test_receive_send.h"
Server::Server(EventLoop *loop, const int port, int maxIdleMin):
									tcpServer_(loop, port, "myServer"),
									connectionBuckets_(maxIdleMin)
{
	
}

Server::~Server()
{
	
}

void Server::start()
{
	tcpServer_.start();
}

void Server::onConnection(TcpConnectionPtr conn)
{
	
}

void Server::onMessage(TcpConnectionPtr conn, Buffer *buf)
{
	application::testrs::retrive_data(conn, buf);
}