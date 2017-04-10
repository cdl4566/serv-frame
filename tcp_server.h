#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <map>

#include "base/Atomic.h"
#include "tcp_connection.h"

class EventLoop;
class Acceptor;
class EventLoopThreadPool;
class Buffer;
class TcpServer{
	public:
		typedef boost::function<void(EventLoop *)> ThreadInitCallback;

		TcpServer(EventLoop *loop, const int port, const std::string& name);
		~TcpServer();
		
		void start();
		void setThreadInitCallback(const ThreadInitCallback& cb){
			threadInitCallback_ = cb;
		}
		void newConnection(int fd);
		void removeConnectionInLoop(const TcpConnectionPtr & conn);
		void removeConnection(const TcpConnectionPtr & conn);

		void setMessageCallback(){

		}
		void setConnectionCallback(){

		}
	private:
		EventLoop *loop_;
		boost::scoped_ptr<Acceptor> acceptor_;
		const std::string name_;
		const int port_;
		boost::shared_ptr<EventLoopThreadPool> threadPool_;
		ThreadInitCallback threadInitCallback_;
		muduo::AtomicInt32 started_;
		int nextConnId_;

		typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
		ConnectionMap connections_;
	 
};
#endif