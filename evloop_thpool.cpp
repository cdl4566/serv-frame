#include "evloop_thpool.h"
#include "evloop_thread.h"

#include <stdio.h>

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, const std::string& nameArg)
												:baseLoop_(baseLoop),
												name_(nameArg),
												started_(false),
												next_(0),
												numThreads_(0)

{
}
EventLoopThreadPool::~EventLoopThreadPool()
{
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb)
{
	assert(!started_);

	started_ = true;
	for(int i=0; i<numThreads_; i++){
		char buf[name_.size() + 32];
		snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
		EventLoopThread *t = new EventLoopThread(cb, buf);
		threads_.push_back(t);
		loops_.push_back(t->startLoop());
	}

	if(numThreads_==0 && cb){
		cb(baseLoop_);
	}
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
	assert(started_);

	EventLoop *loop = baseLoop_;

	if(!loops_.empty()){
		loop = loops_[next_++];
		if(static_cast<size_t>(next_) >= loops_.size()){
			next_ = 0;
		}	
	}
	return loop;
}

EventLoop *EventLoopThreadPool::getLoopForHash(size_t hashCode)
{
	EventLoop *loop = baseLoop_;

	if(!loops_.empty()){
		loop = loops_[hashCode % loops_.size()];
	}
	return loop;
}

std::vector<EventLoop *>EventLoopThreadPool::getAllLoops()
{
	assert(started_);

	if(!loops_.empty()){
		return loops_;
	}else{
		return std::vector<EventLoop *>(1, baseLoop_);
	}
	
}

