#ifndef POLL_POLLER_H
#define POLL_POLLER_H

#include "poller.h"

#include <vector>
#include <poll.h>
#include <errno.h>


class EventLoop;

class PollPoller : public Poller
{
	public:
		PollPoller(EventLoop *loop);
		~PollPoller();

		void polling(int timeoutMs, ChannelList* activeChannels);  
		void updateChannel(Channel* channel);  
		void removeChannel(Channel* channel);
		void fillActiveChannel(int numEvents, ChannelList *activeChannels) const;

	private:
		typedef std::vector<struct pollfd> PollFdList;
		PollFdList pollfds_;
};

#endif

