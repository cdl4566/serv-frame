#include "epoll_poller.h"
#include "../channel.h"


#include <vector>
#include <poll.h>
#include <errno.h>
#include <boost/static_assert.hpp>
#include <assert.h>


BOOST_STATIC_ASSERT(EPOLLIN == POLLIN);
BOOST_STATIC_ASSERT(EPOLLPRI == POLLPRI);
BOOST_STATIC_ASSERT(EPOLLOUT == POLLOUT);
BOOST_STATIC_ASSERT(EPOLLRDHUP == POLLRDHUP);
BOOST_STATIC_ASSERT(EPOLLERR == POLLERR);
BOOST_STATIC_ASSERT(EPOLLHUP == POLLHUP);

namespace  //namespace with no name, cannot use outside
{
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;
}

EpollPoller::EpollPoller(EventLoop *loop):
								Poller(loop),
								epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
								events_(kInitEventListSize)
{
	assert(epollfd_ > 0);
}

EpollPoller::~EpollPoller()
{
	::close(epollfd_);
}

void EpollPoller::polling(int timeoutMs, ChannelList* activeChannels)
{
	int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), 
									static_cast<int>(events_.size()), 
									timeoutMs);

	int savedErrno = errno;
	if (numEvents > 0){
		fillActiveChannels(numEvents, activeChannels);
		if(static_cast<size_t>(numEvents) == events_.size()){
			events_.resize(events_.size()*2);
		}
	}
	else if(numEvents == 0){
		//no happen
	}
	else{
		assert(savedErrno == EINTR);
	}
	
}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
	assert(static_cast<size_t>(numEvents) <= events_.size());
	for(int i=0; i<numEvents; i++){
		Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
		channel->set_revents(events_[i].events);
		activeChannels->push_back(channel);
	}
}

void EpollPoller::updateChannel(Channel* channel)  
{
	const int index = channel->index();
	if (index == kNew || index == kDeleted){
		int fd = channel->fd();
		if (index == kNew){
			assert(channels_.find(fd) == channels_.end());
			channels_[fd] = channel;
		}
		else{
			assert(channels_.find(fd) != channels_.end());
			assert(channels_[fd] == channel);
		}

		channel->set_index(kAdded);
		update(EPOLL_CTL_ADD, channel);
	}
	else{
		int fd = channel->fd();
		assert(channels_.find(fd) != channels_.end());
		assert(channels_[fd] == channel);
		assert(index == kAdded);
		
		if(channel->isNoneEvent()){
			channel->set_index(kDeleted);
			update(EPOLL_CTL_DEL, channel);
		}
		else{
			update(EPOLL_CTL_MOD, channel);
		}
	}
}

void EpollPoller::removeChannel(Channel* channel)
{
	int fd = channel->fd();
	assert(channels_.find(fd) != channels_.end());
	assert(channels_[fd] == channel);
	assert(channel->isNoneEvent());
	int index = channel->index();
	assert(index == kAdded || index == kDeleted);
	size_t n = channels_.erase(fd);
	assert(n == 1);

	if(index == kAdded){
		update(EPOLL_CTL_DEL, channel);
	}
	channel->set_index(kNew);
}

void EpollPoller::update(int operation, Channel *channel)
{
	struct epoll_event event;
	bzero(&event, sizeof event);
	event.events = channel->events();
	event.data.ptr = channel;
	int fd = channel->fd();

	int ret = ::epoll_ctl(epollfd_, operation, fd, &event);
	assert(ret == 0);	
}
