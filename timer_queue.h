#ifndef TIMER_QUEUE_H
#define TIMER_QUEUE_H

#include "callbacks.h"
#include "channel.h"
#include "base/time_stamp.h"

#include <set>
#include <vector>

class EventLoop;
class Timer;

class TimerQueue{

	public:
		TimerQueue(EventLoop *loop);
		~TimerQueue();

		void addTimer(const TimerCallback& cb, TimeStamp when, double interval);
		
	private:
		typedef std::pair<TimeStamp, Timer *> Entry;
		typedef std::set<Entry> TimerList;
		typedef std::pair<Timer *, int64_t> ActiveTimer;
		typedef std::set<ActiveTimer> ActiveTimerSet;
		
		void addTimerInLoop(Timer *timer);
		void handleRead();
		bool insert(Timer* timer);
		std::vector<TimerQueue::Entry> getExpired(TimeStamp now);
		void reset(const std::vector<Entry>& expired, TimeStamp now);
		
		EventLoop *loop_;
		const int timerfd_;
		Channel timerfdChannel_;
		TimerList timers_;
		ActiveTimerSet activeTimers_;

};



#endif