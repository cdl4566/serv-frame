#ifndef SERVER_ACCEPTOR_H
#define SERVER_ACCEPTOR_H

#include <boost/bind.hpp>

#include "channel.h"

class EventLoop;
class Acceptor{
	public:
		typedef boost::function<void(int)> NewConnectionCallback;
		Acceptor(EventLoop *loop, int port);
		~Acceptor();
		
		void listen();
		void handleRead();
		bool listenning() const{
			return listenning_;
		}
		void setNewConnectionCallback(const NewConnectionCallback& cb){
			newConnectionCallback_ = cb;
		}
	private:
		EventLoop *loop_;
		int acceptSocket_;
		Channel acceptChannel_;
		bool listenning_;

		NewConnectionCallback newConnectionCallback_;
};

#endif