#include "tcp_server.h"
#include "event_loop.h"
#include "acceptor.h"
#include "evloop_thpool.h"
#include "base/log.h"

#include <boost/bind.hpp>

//TcpServer create Acceptor, so responsible callbacks for Acceptor
//TcpServer is for lifecycle of Acceptor
TcpServer::TcpServer(EventLoop *loop, const int port, const std::string& name)
										:loop_(loop),
										port_(port),
										name_(name),
										acceptor_(new Acceptor(loop, port)),
										threadPool_(new EventLoopThreadPool(loop, name)),
										nextConnId_(1)
{
	assert(loop_);
	acceptor_->setNewConnectionCallback(boost::bind(&TcpServer::newConnection, this, _1));
	threadPool_->setNumThreads_(3);
}

TcpServer::~TcpServer()
{
	
}

void TcpServer::start()
{
	if(started_.getAndSet(1) == 0){
		threadPool_->start(threadInitCallback_);
	
		loop_->runInLoop(boost::bind(&Acceptor::listen, get_pointer(acceptor_)));
	}	
}

void TcpServer::newConnection(int fd)
{
	EventLoop *ioLoop = threadPool_->getNextLoop();
	char buf[64];
	snprintf(buf, sizeof buf, "-%d#%d", port_, nextConnId_);
	++nextConnId_;
	std::string connName = name_ + buf;

	TcpConnectionPtr conn(new TcpConnection(ioLoop, fd, connName));
	connections_[connName] = conn; //otherwise conn will be destroyed by shared_ptr when this function ends
	conn->setMessageCallback(messageCallback_);
	conn->setConnectionCallback(connectionCallback_);
	conn->setCloseCallback(boost::bind(&TcpServer::removeConnection, this, _1));
	ioLoop->runInLoop(boost::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr & conn)
{
	//why use loop_?  //for still refer conn, let it auto-destroyed after removeConnectionInLoop ends
	//user loop_->runInLoop, connections_.erase is done only by main loop thread, not any other threads
	loop_->runInLoop(boost::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr & conn)
{
	size_t n = connections_.erase(conn->name());
	assert(n == 1);
	EventLoop *ioLoop = conn->getLoop();
	ioLoop->queueInLoop(boost::bind(&TcpConnection::connectDestroyed, conn));
}