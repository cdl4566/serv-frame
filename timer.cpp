#include "timer.h"

muduo::AtomicInt64 Timer::s_numCreated_;

Timer::Timer(const TimerCallback& cb, TimeStamp when, double interval)
:callback_(cb),
expiration_(when),
interval_(interval),
repeat_(interval > 0.0),
sequence_(s_numCreated_.incrementAndGet())

{

}

Timer::~Timer()
{

}

void Timer::restart(TimeStamp now)
{
  if (repeat_)
  {
	expiration_.addTime(interval_);
  }
  else
  {
    expiration_ = TimeStamp::invalid();
  }
}



