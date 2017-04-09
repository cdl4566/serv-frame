#ifndef EPOLL_POLLER_H
#define EPOLL_POLLER_H

#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "poller.h"

class EventLoop;

class EpollPoller : public Poller
{
	public:
		EpollPoller(EventLoop *loop);
		~EpollPoller();

		void polling(int timeoutMs, ChannelList* activeChannels);  
		void updateChannel(Channel* channel);  
		void removeChannel(Channel* channel);

	private:
		void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;
		void update(int operation, Channel *channel);

	private:
		static const int kInitEventListSize = 16;

		typedef std::vector<struct epoll_event> EventList;

		int epollfd_;
		EventList events_;
};

#endif

