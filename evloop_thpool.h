#ifndef EVENT_LOOP_THREAD_POOL_H
#define EVENT_LOOP_THREAD_POOL_H

#include <vector>
#include <boost/function.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

class EventLoop;
class EventLoopThread;
class EventLoopThreadPool{
	public:
		typedef boost::function<void(EventLoop *)> ThreadInitCallback;
		EventLoopThreadPool(EventLoop *baseLoop, const std::string& nameArg);
		~EventLoopThreadPool();
		
		void setNumThreads_(int num){
			numThreads_ = num;
		}
		void start(const ThreadInitCallback& cb);
		const std::string& name() const{
			return name_;
		}
		bool started() const{
			return started_;
		}
		EventLoop *getNextLoop();
		EventLoop *getLoopForHash(size_t hashCode);
		std::vector<EventLoop *>getAllLoops();
		
	
	private:
		int numThreads_;
		std::vector<EventLoop *> loops_;
		boost::ptr_vector<EventLoopThread> threads_; 
		std::string name_;
		EventLoop *baseLoop_;
		bool started_;
		int next_;

};
#endif