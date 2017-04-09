#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "tcp_server.h"

class EventLoop;
class Server{
	public:
		Server(EventLoop *loop, const int port);
		~Server();
		void start();
	private:
		TcpServer tcpServer_;

};

#endif