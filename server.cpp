#include "server.h"
#include "tcp_server.h"
#include "application/test_receive_send.h"
#include "event_loop.h"

#include <boost/bind.hpp>
#include <boost/function.hpp>

Server::Server(EventLoop *loop, const int port, int maxIdleMin):
									loop_(loop),
									tcpServer_(loop, port, "myServer"),
									connectionBuckets_(maxIdleMin)
{
	tcpServer_.setConnectionCallback(boost::bind(&Server::onConnection, this, _1));
	tcpServer_.setMessageCallback(boost::bind(&Server::onMessage, this, _1, _2));
	loop_->runEvery(60.0, boost::bind(&Server::onTimer, this));
	connectionBuckets_.resize(maxIdleMin);
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
	EntryPtr entry(new Entry(conn));
	connectionBuckets_.back().insert(entry);
	WeakEntryPtr weakEntry(entry);
	conn->setContext(weakEntry);
}

void Server::onMessage(TcpConnectionPtr conn, Buffer *buf)
{
	assert(!conn->getContext().empty());
	WeakEntryPtr weakEntry = boost::any_cast<WeakEntryPtr>(conn->getContext());
	EntryPtr entry(weakEntry.lock());
	if(entry){
		connectionBuckets_.back().insert(entry);
	}

	application::testrs::retrive_data(conn, buf);
}

void Server::onTimer()
{
	connectionBuckets_.push_back(Bucket());
}
