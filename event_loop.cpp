#include "event_loop.h"
#include "poller/poll_poller.h"
#include "poller/epoll_poller.h"
#include "channel.h"

#include <sys/eventfd.h>
#include <boost/bind.hpp>
#include <iostream>

__thread EventLoop *t_loopInThisThread = NULL;

const int kPollTimeMs = 10000;
int createEventfd()
{  
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);  
	if (evtfd < 0)  {        
		::abort();  
	}  

	return evtfd;
}

EventLoop *EventLoop::getLoopOfCurrentThread()
{
	return t_loopInThisThread;
}


EventLoop::EventLoop():	poller_(new EpollPoller(this)),//poller_(new PollPoller(this)),
							threadId_(CurrentThread::tid()), //onwered by one thread, can't be changed
							wakeupFd_(createEventfd()),
							wakeupChannel_(new Channel(this, wakeupFd_)),
							quit_(false),
							currentActiveChannel_(NULL),
							eventHandling_(false)
{
	assert(t_loopInThisThread == NULL);
	t_loopInThisThread = this;

	wakeupChannel_->setReadCallback(boost::bind(&EventLoop::handleRead, this));
	wakeupChannel_->enableReading();
}
	
EventLoop::~EventLoop()
{
	close(wakeupFd_);
	wakeupChannel_->disableAll();
	wakeupChannel_->remove();
	t_loopInThisThread = NULL;
}

// ioLoop->runInLoop(cb) can be called by other thread
//cb() is supposed to be called by the thread which owns ioLoop
//if cb() is for regist event, is called by other thread, so the event is registered to this ioLoop
//but this ioLoop is at running looping(), it will never know a new event registered.
/*void EventLoop::runInLoop(const Functor&& cb) // cb is rvalue
{
	if(isInLoopThread()){
		cb();
	}
	else{
		queueInLoop(std::move(cb)); //cb becomes lvalue, so need std::move(cb)
	}
}

void EventLoop::queueInLoop(const Functor&& cb){
	MutexLockGuard lock(mutex_);
	pendingFunctors_.push_back(std::move(cb)); //cb becomes lvalue, so need ...

	wakeup();
	
}*/

void EventLoop::runInLoop(const Functor& cb)
{
	if(isInLoopThread()){
		cb();
	}
	else{
		queueInLoop(cb);
	}
}

void EventLoop::queueInLoop(const Functor& cb){
	MutexLockGuard lock(mutex_);
	pendingFunctors_.push_back(cb);
	//std::cout << "wakeup" << std::endl;
	wakeup();
	
}

void EventLoop::wakeup()
{
	uint64_t one = 1; //must uint64_t (8 bytes)
	ssize_t n = ::write(wakeupFd_, &one, sizeof one);
	//std::cout << "wakeup::write::n=" << n << std::endl;
	if(n != sizeof one){
		//log err
	}
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = ::read(wakeupFd_, &one, sizeof one);
	//std::cout << "wakeup::read::n=" << n << std::endl;
	if(n != sizeof one){
		//log err
	}
}

void EventLoop::loop()
{
	assert(isInLoopThread()); //if other thread gets this EventLoop* and call loop(), it will give fault
	quit_ = false;

	while(!quit_){
		activeChannels_.clear();
		poller_->polling(kPollTimeMs, &activeChannels_);
		std::cout << "EventLoop::loop " << "polling return" << std::endl;

		if(activeChannels_.empty()){
			continue;
		}
		eventHandling_ = true;
		for(ChannelList::const_iterator ch=activeChannels_.begin();
										ch!=activeChannels_.end(); ch++)
		{
			currentActiveChannel_ = *ch;
			currentActiveChannel_->handleEvent();
		}
		currentActiveChannel_ = NULL;
		eventHandling_ = false;
		doPendingFunctors();
		
	}
}

//ioloop->quit() called by this thread, it means the thread is not at looping(),
//set quit_ = true, when thread start looping(), it will exit
//if ioloop->quit() called by other thread, it means the thread that owns this ioloop is at looping(),
//need wakeup to let this thread know quit_ = true
void EventLoop::quit()
{
	quit_ = true;

	if(!isInLoopThread()){
		wakeup();
	}
}

void EventLoop::updateChannel(Channel *channel)
{
	assert(channel->ownerLoop() == this);
	poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
	assert(channel->ownerLoop() == this);
	assert(isInLoopThread()); // prevent multi-thread

	if(eventHandling_){
		assert(currentActiveChannel_ == channel || 
			std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
	}

	poller_->removeChannel(channel);
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;	

	MutexLockGuard lock(mutex_);
	functors.swap(pendingFunctors_);

	for(size_t i=0; i<functors.size(); i++){
		functors[i]();
	}
}