#ifndef TCP_CONNECTION_AND_PTR_H
#define TCP_CONNECTION_AND_PTR_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include "buffer.h"
#include "callbacks.h"

class EventLoop;
class Channel;
class TcpConnection;
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef boost::function<void (const TcpConnectionPtr&)>  CloseCallback;

class TcpConnection : public boost::enable_shared_from_this<TcpConnection>
{
	public:
		TcpConnection(EventLoop* loop, int fd, const std::string& nameArg);
		~TcpConnection();

		void handleRead();
		void handleWrite();
		void handleClose();
		void handleError();
		void connectEstablished();
		void connectDestroyed();
		void setCloseCallback(const CloseCallback & cb){
			closeCallback_ = cb;
		}
		void setConnectionCallback(const ConnectionCallback& cb){
			connectionCallback_ = cb;
		}
		
		void setMessageCallback(const MessageCallback & cb){
			messageCallback_ = cb;
		}
		const std::string& name() const {
			return name_;
		}
		EventLoop* getLoop() const{
			return loop_;
		}
		void send(const void *data, size_t len);
		void sendInLoop(const void *data, size_t len);
		
	private:
		EventLoop *loop_;
		boost::scoped_ptr<Channel> channel_;
		const std::string name_;
		const int fd_;
		CloseCallback closeCallback_;
		MessageCallback messageCallback_;
		ConnectionCallback connectionCallback_;
		Buffer inputBuffer_;
		Buffer outputBuffer_;

};

void defaultOnMessage(const TcpConnectionPtr &conn, Buffer *buf);

#endif