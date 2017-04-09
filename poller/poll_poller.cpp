#include "poll_poller.h"
#include "../channel.h"

#include <iostream>

PollPoller::PollPoller(EventLoop *loop):Poller(loop)
{
}

PollPoller::~PollPoller()
{
}

void PollPoller::polling(int timeoutMs, ChannelList* activeChannels)
{
	int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
	if(numEvents == 0){

	}
	else if(numEvents > 0){
		fillActiveChannel(numEvents, activeChannels);
	}
	else{
		assert(errno == EAGAIN);
	}
}

void PollPoller::fillActiveChannel(int numEvents, ChannelList* activeChannels) const
{
	for(PollFdList::const_iterator pfd=pollfds_.begin(); 
								pfd!=pollfds_.end() && numEvents>0; pfd++)
	{
		if(pfd->revents > 0){
			numEvents--;
			ChannelMap::const_iterator ch = channels_.find(pfd->fd);
			assert(ch != channels_.end());
			Channel *channel = ch->second;
			assert(pfd->fd == channel->fd());
			channel->set_revents(pfd->revents);
			activeChannels->push_back(channel);
		}
		if(numEvents == 0){
			break;
		}
	}
}

void PollPoller::updateChannel(Channel* channel)
{
	if(channel->index()<0){
		std::cout << "update channel->fd():" << channel->fd() << std::endl;
		assert(channels_.find(channel->fd()) == channels_.end());
		channels_[channel->fd()] = channel;
		struct pollfd pfd;
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		pollfds_.push_back(pfd);
		int idx = static_cast<int>(pollfds_.size())-1;  //guess: pollfds_.size() type is size_t
		channel->set_index(idx);
	}else{
		assert(channels_.find(channel->fd()) != channels_.end());
		assert(channels_[channel->fd()] == channel);
		int idx = channel->index();
		assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));	
		struct pollfd& pfd = pollfds_[idx];
		assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if(channel->isNoneEvent()){
			pfd.fd = -channel->fd()-1; //no event set fd < 0
		}
	}
}

void PollPoller::removeChannel(Channel* channel)
{
	std::cout << "remove channel->fd():" << channel->fd() << std::endl;
	assert(channels_.find(channel->fd()) != channels_.end());
	assert(channels_[channel->fd()] == channel);
	assert(channel->isNoneEvent());
	int idx = channel->index();
	assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));
	const struct pollfd& pfd = pollfds_[idx];
	assert(pfd.fd == -channel->fd()-1 && pfd.events == channel->events());
	size_t n = channels_.erase(channel->fd());
	assert(n == 1);
	if(static_cast<size_t>(idx) == pollfds_.size()-1){
		pollfds_.pop_back();
	}else{
		int channelAtEnd = pollfds_.back().fd;
		iter_swap(pollfds_.begin()+idx, pollfds_.end()-1);
		if(channelAtEnd < 0){
			channelAtEnd = -channelAtEnd -1;
		}
		channels_[channelAtEnd]->set_index(idx);
		pollfds_.pop_back();
	}
}

