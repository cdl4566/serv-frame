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
				TcpConnection conn = weakConn_.lock();
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