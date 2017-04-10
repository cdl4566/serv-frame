#include "server.h"
#include "tcp_server.h"
#include "application/test_receive_send.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

Server::Server(EventLoop *loop, const int port, int maxIdleMin):
									tcpServer_(loop, port, "myServer"),
									connectionBuckets_(maxIdleMin)
{
	tcpServer_.setConnectionCallback(boost::bind(&Server::onConnection, this, _1));
	tcpServer_.setMessageCallback(boost::bind(&Server::onMessage, this, _1, _2));
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