#include "channel.h"
#include "event_loop.h"
#include "base/netopr.h"

#include <poll.h>

const int Channel::kNoneEvent = 0;  
const int Channel::kReadEvent = POLLIN | POLLPRI;  
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, const int fd)
											:fd_(fd),
											loop_(loop),
											index_(-1),
											events_(0),
											revents_(0),
											addedToLoop_(false)
{
}

Channel::~Channel()
{
	bnet::close_connection(fd_);
}

void Channel::setReadCallback(const EventCallback& cb)
{
	readCallback_ = cb;
}

void Channel::setWriteCallback(const EventCallback& cb)
{
	writeCallback_ = cb;
}

void Channel::setErrorCallback(const EventCallback& cb)
{
	errorCallback_ = cb;
}

void Channel::setCloseCallback(const EventCallback & cb)
{
	closeCallback_ = cb;
}

void Channel::update()
{
	addedToLoop_ = true;
	
	loop_->updateChannel(this);
}

void Channel::remove()
{
	assert(isNoneEvent());
	addedToLoop_ = false;

	loop_->removeChannel(this);
}

void Channel::handleEvent()
{
	if (revents_ & (POLLERR | POLLNVAL)){
		if(errorCallback_) errorCallback_();
	}
	if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)){
		if(readCallback_) readCallback_();
	}
	if (revents_ & POLLOUT){
		if(writeCallback_) writeCallback_();
	}
}