#include "timer_queue.h"
#include "timer.h"
#include "event_loop.h"


#include <sys/timerfd.h> 

struct timespec howMuchTimeFromNow(TimeStamp when)
{
	int64_t microseconds = when.microSec() - TimeStamp::now();
	if (microseconds < 100)
	{
    	microseconds = 100;
  	}

	struct timespec ts;
	ts.tv_sec = static_cast<time_t>(microseconds / 1000000);
	ts.tv_nsec = static_cast<long>((microseconds % 1000000) * 1000);

	return ts;
}

int createTimerfd()
{
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

	assert(timerfd > 0);
	return timerfd;
}

void resetTimerfd(int timerfd, TimeStamp expiration)
{
  // wake up loop by timerfd_settime()
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof newValue);
  bzero(&oldValue, sizeof oldValue);
  newValue.it_value = howMuchTimeFromNow(expiration);
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue); //TFD_TIMER_ABSTIME, 0（相对定时器）
  if (ret)
  {
    // error: "timerfd_settime()";
  }
}

void readTimerfd(int timerfd, TimeStamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
  
  if (n != sizeof howmany)
  {
    // error "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
  }
}

TimerQueue::TimerQueue(EventLoop *loop):loop_(loop),
											timerfd_(createTimerfd()),
											timerfdChannel_(loop_, timerfd_),
											timers_()
{

	timerfdChannel_.setReadCallback(boost::bind(&TimerQueue::handleRead, this));
	timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
	timerfdChannel_.disableAll();
	timerfdChannel_.remove();
	::close(timerfd_);

}

void TimerQueue::addTimer(const TimerCallback& cb, TimeStamp when, double interval)
{
	Timer *timer = new Timer(cb, when, interval);

	//loop_->runInLoop(boost::bind(&TimerQueue::addTimerInLoop, this, timer));
}

void TimerQueue::addTimerInLoop(Timer *timer)
{
	loop_->assertInThreadLoop();

	bool earliestChanged = insert(timer);

	if(earliestChanged){
		resetTimerfd(timerfd_, timer->expiration());
	}

}

void TimerQueue::handleRead()
{
  loop_->assertInThreadLoop();
  TimeStamp now(TimeStamp::now());
  readTimerfd(timerfd_, now);

  std::vector<Entry> expired = getExpired(now);

  for (std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    it->second->run();
  }

  reset(expired, now);
}


bool TimerQueue::insert(Timer* timer)
{
  loop_->assertInThreadLoop();
  assert(timers_.size() == activeTimers_.size());
  bool earliestChanged = false;
  TimeStamp when = timer->expiration();
  TimerList::iterator it = timers_.begin();
  if (it == timers_.end() || when < it->first)
  {
    earliestChanged = true;
  }
  {
    std::pair<TimerList::iterator, bool> result
      = timers_.insert(Entry(when, timer));
    assert(result.second); (void)result;
  }
  {
    std::pair<ActiveTimerSet::iterator, bool> result
      = activeTimers_.insert(ActiveTimer(timer, timer->sequence()));
    assert(result.second); (void)result;
  }

  assert(timers_.size() == activeTimers_.size());
  return earliestChanged;
}


std::vector<TimerQueue::Entry> TimerQueue::getExpired(TimeStamp now)
{
  assert(timers_.size() == activeTimers_.size());
  std::vector<Entry> expired;
  Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
  
  TimerList::iterator end = timers_.lower_bound(sentry); //set 默认升序，返回容器中第一个值【大于或等于】val的元素的iterator位置，返回第一个未到期的Timer
  assert(end == timers_.end() || now < end->first);

  std::copy(timers_.begin(), end, back_inserter(expired));
  timers_.erase(timers_.begin(), end);

  for (std::vector<Entry>::iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    ActiveTimer timer(it->second, it->second->sequence());
    size_t n = activeTimers_.erase(timer);
    assert(n == 1); (void)n;
  }

  assert(timers_.size() == activeTimers_.size());
  return expired;
}


void TimerQueue::reset(const std::vector<Entry>& expired, TimeStamp now)
{
  TimeStamp nextExpire;

  for (std::vector<Entry>::const_iterator it = expired.begin();
      it != expired.end(); ++it)
  {
    ActiveTimer timer(it->second, it->second->sequence());
    if (it->second->repeat())
    {
      it->second->restart(now);
      insert(it->second);
    }
    else
    {
      // FIXME move to a free list
      delete it->second; // FIXME: no delete please
    }
  }

  if (!timers_.empty())
  {
    nextExpire = timers_.begin()->second->expiration();
  }

  if (nextExpire.valid())
  {
    resetTimerfd(timerfd_, nextExpire);
  }
}

