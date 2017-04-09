#ifndef EVENT_LOOP_THREAD_H
#define EVENT_LOOP_THREAD_H

#include <boost/function.hpp>
#include <pthread.h>

#include "base/Condition.h"
#include "base/Mutex.h"
class EventLoop;
class EventLoopThread{
	public:
		typedef boost::function<void(EventLoop *)> ThreadInitCallback;
		EventLoopThread(const ThreadInitCallback& cb, const std::string& nameArg);
		~EventLoopThread();
		EventLoop *startLoop();
		static void *threadFun(void * pParam);

	private:
		Condition cond_;
		MutexLock mutex_;
		ThreadInitCallback callback_;
		const std::string name_;
		bool exiting_;
		EventLoop *loop_;
		pthread_t eventLoopThreadId_;
};


#endif

