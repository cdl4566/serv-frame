#include "acceptor.h"
#include "channel.h"
#include "base/netopr.h"

#include <iostream>

//for example: Acceptor create Channel, so in Acceptor, responsible to set callbacks for Channel
Acceptor::Acceptor(EventLoop *loop, int port)
							:loop_(loop),
							acceptSocket_(bnet::init_listener(port)),
							acceptChannel_(loop, acceptSocket_),
							listenning_(false)
{
	std::cout << "Acceptor::acceptSocket_ = " << acceptSocket_ << std::endl;
	assert(acceptSocket_>0);
	acceptChannel_.setReadCallback(boost::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
}
		
void Acceptor::listen()
{
	listenning_ = true;
	acceptChannel_.enableReading();
}

void Acceptor::handleRead()
{
	int connfd = bnet::accept_connection(acceptSocket_);
	std::cout << "Acceptor::connfd = " << connfd << std::endl;
	if(connfd >= 0){
		if(newConnectionCallback_){
			newConnectionCallback_(connfd);
		}else{
			bnet::close_connection(connfd);
		}
		
	}else{
		//err
	}
}
