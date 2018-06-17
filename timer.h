#ifndef SERVER_TIMERQUEUE_TIMER_H
#define SERVER_TIMERQUEUE_TIMER_H

#include "callbacks.h"
#include "base/Atomic.h"
#include "base/time_stamp.h"


class Timer{
	public:
		Timer(const TimerCallback& cb, TimeStamp when, double interval);
		~Timer();

		void run() const{
			callback_();
		}

		void restart(TimeStamp now);

		TimeStamp expiration() const  { return expiration_; }
  		bool repeat() const { return repeat_; }
  		int64_t sequence() const { return sequence_; }

		static int64_t numCreated() { return s_numCreated_.get(); }
		
	private:
		const double interval_;
		const TimerCallback callback_;
		TimeStamp expiration_;
		const bool repeat_;
  		const int64_t sequence_;
		static muduo::AtomicInt64 s_numCreated_;
};


#endif
