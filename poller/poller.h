#ifndef BASE_POLLER_H
#define BASE_POLLER_H

#include <vector>
#include <map>
class EventLoop;
class Channel;
class Poller{
	public:
		typedef std::vector<Channel *> ChannelList;
		Poller(EventLoop *loop);
		virtual ~Poller();

		virtual void polling(int timeoutMs, ChannelList* activeChannels)=0;  
		virtual void updateChannel(Channel* channel)=0;  
		virtual void removeChannel(Channel* channel)=0;

	protected:
		typedef std::map<int, Channel *> ChannelMap;
		ChannelMap channels_;

	private:
		EventLoop *ownerLoop_;
};

#endif