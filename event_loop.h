#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H

#include <boost/scoped_ptr.hpp>
#include <boost/function.hpp>
#include <vector>

#include "base/CurrentThread.h"
#include "base/Mutex.h"
#include "callbacks.h"
#include "timer_queue.h"

int createEventfd();

class Poller;
class Channel;
class Channel;
class EventLoop{
	public:
		typedef boost::function<void()> Functor;
		EventLoop();
		~EventLoop();
		bool isInLoopThread() const {
			return threadId_ == CurrentThread::tid();
		}
		void assertInThreadLoop(){
			assert(isInLoopThread());
		}
		void runInLoop(const Functor& cb);
		void loop();
		void quit();
		void updateChannel(Channel *channel);
		void removeChannel(Channel * channel);
		EventLoop *getLoopOfCurrentThread();
		void handleRead();
		void queueInLoop(const Functor& cb);

		void runEvery(double interval, TimerCallback& cb);
		//void runAt(double delay, TimerCallback& cb);
		//void runAfter(double delay, TimerCallback& cb);
		

	private:
		void wakeup();
		void doPendingFunctors();

		
	private:
		boost::scoped_ptr<Poller> poller_;
		const pid_t threadId_;
		mutable MutexLock mutex_;
		std::vector<Functor> pendingFunctors_;
		int wakeupFd_;
		boost::scoped_ptr<Channel> wakeupChannel_;
		bool quit_;
		bool eventHandling_;

		typedef std::vector<Channel *> ChannelList;

		ChannelList activeChannels_;
		Channel *currentActiveChannel_;
		boost::scoped_ptr<TimerQueue> timerQueue_;
};

#endif