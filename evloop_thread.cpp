#include "evloop_thread.h"
#include "event_loop.h"

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& nameArg)
										:callback_(cb),
										name_(nameArg),
										exiting_(false),
										loop_(NULL),
										mutex_(),
										cond_(mutex_)
{
}

EventLoopThread::~EventLoopThread()
{
	exiting_ = true;
	if(loop_){
		loop_->quit();
		pthread_join(eventLoopThreadId_, NULL);
	}
}

EventLoop *EventLoopThread::startLoop()
{
	pthread_create(&eventLoopThreadId_, NULL, EventLoopThread::threadFun, this);
	MutexLockGuard lock(mutex_);
	while(loop_ == NULL){
		cond_.wait();
	}
	return loop_;
}

void *EventLoopThread::threadFun(void * pParam)
{
	EventLoopThread *pThis = (EventLoopThread *)pParam;
	EventLoop loop;
	if(pThis->callback_){
		pThis->callback_(&loop);
	}
	pThis->loop_ = &loop;
	pThis->cond_.notify();
	
	loop.loop();
	pThis->loop_ = NULL;
}

