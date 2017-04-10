#include "tcp_connection.h"
#include "channel.h"
#include "event_loop.h"
#include "base/netopr.h"

#include<iostream>

void defaultOnMessage(const TcpConnectionPtr &conn, Buffer *buf)
{
	/*static int cnt = 0;
	std::string msg(buf->retriveAllAsString());
	std::cout << "recive count " << cnt++ << std::endl;
	std::cout << "connection " << conn->name() << " msg " << msg << std::endl;
*/
	buf->retriveAll();
}

TcpConnection::TcpConnection(EventLoop* loop, int fd, 
								const std::string& nameArg) : 
							    loop_(loop),
								name_(nameArg),
								fd_(fd),
								channel_(new Channel(loop, fd))
{
	channel_->setErrorCallback(boost::bind(&TcpConnection::handleError, this));
	channel_->setReadCallback(boost::bind(&TcpConnection::handleRead, this));
	channel_->setWriteCallback(boost::bind(&TcpConnection::handleWrite, this));
	channel_->setCloseCallback(boost::bind(&TcpConnection::handleClose, this));
}
TcpConnection::~TcpConnection()
{

}

void TcpConnection::handleRead()
{
	assert(loop_->isInLoopThread());

	int savedErrno;
	ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
	
	if(n > 0){
		messageCallback_(shared_from_this(), &inputBuffer_);
	}else if(n == 0){
		handleClose();
	}else{
		handleError();
	}
}

void TcpConnection::handleWrite()
{
	loop_->assertInThreadLoop();
	if(channel_->isWriting()){
		ssize_t n = bnet::send_data(channel_->fd(), 
									outputBuffer_.beginRead(),
									outputBuffer_.readableBytes());

		if(n > 0){
			outputBuffer_.retrive(n);
			if(outputBuffer_.readableBytes() == 0){
				channel_->disableWriting();
			}
		}
	}
}

void TcpConnection::handleClose()
{
	channel_->disableAll();
	TcpConnectionPtr guardThis(shared_from_this());
	closeCallback_(guardThis);
}

void TcpConnection::handleError()
{

}

void TcpConnection::connectEstablished()
{
	loop_->assertInThreadLoop();
	channel_->enableReading();

	connectionCallback_(shared_from_this());
}

void TcpConnection::connectDestroyed()
{
	loop_->assertInThreadLoop();
	channel_->disableAll();

	channel_->remove();
}

void TcpConnection::send(const void *data, size_t len)
{
	if(loop_->isInLoopThread()){
		sendInLoop(data, len);
	}
	else{
		loop_->runInLoop(boost::bind(&TcpConnection::sendInLoop, this, data, len));
	}
}

void TcpConnection::sendInLoop(const void *data, size_t len)
{
	loop_->assertInThreadLoop();
	ssize_t nwrote = 0;
	ssize_t remaining = len;
	bool faultError = false;

	if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0){
		nwrote = bnet::send_data(channel_->fd(), data, len);

		if(nwrote >= 0){
			remaining -= nwrote;
		}
		else{
			nwrote = 0;
			if(errno != EAGAIN){
				std::cout << "sendInLoop::send err" << std::endl;
			}
			faultError = true;
		}
	}

	if(remaining > 0 && !faultError){
		outputBuffer_.append(static_cast<const char *>(data)+nwrote, remaining);
		if(!channel_->isWriting()){
			channel_->enableWriting();
		}
	}
}