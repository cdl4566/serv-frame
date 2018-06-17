#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#include "tcp_server.h"

#include <set>
#include <boost/circular_buffer.hpp>

class EventLoop;
class Server{
	public:
		Server(EventLoop *loop, const int port, int maxIdleMin);
		~Server();
		void start();
		void onTimer();
		
	private:
		void onConnection(TcpConnectionPtr conn);
		void onMessage(TcpConnectionPtr conn, Buffer *buf);

		typedef boost::weak_ptr<TcpConnection> WeakTcpConnectionPtr;


		struct Entry
		{
			explicit Entry(const TcpConnectionPtr conn)
				:weakConn_(conn)
			{

			}

			~Entry()
			{
				TcpConnectionPtr conn = weakConn_.lock();
				if(conn){
					//conn->shutdown();
				}
			}

			WeakTcpConnectionPtr weakConn_;
			

		};


	private:
		typedef boost::shared_ptr<Entry> EntryPtr;
		typedef boost::weak_ptr<Entry> WeakEntryPtr;
		typedef std::set<EntryPtr> Bucket;
		typedef boost::circular_buffer<Bucket> WeakConnectionList;

		EventLoop *loop_;
		TcpServer tcpServer_;
		WeakConnectionList connectionBuckets_;
};


#endif