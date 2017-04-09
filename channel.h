#ifndef NET_COM_CHANNEL_H
#define NET_COM_CHANNEL_H

#include <boost/function.hpp>

/*
channel might be accept_channel, or connection_channel,
read function for above two channels are different,
so need callback set for the ower class of channel,
that is: class acceptor sets "accept connection" callback
class tcp_connection sets "read data" callback
*/

class EventLoop;
class Channel{
	public:
		typedef boost::function<void()> EventCallback;
		Channel(EventLoop *loop, const int fd);
		~Channel();
		void setReadCallback(const EventCallback& cb);
		void setWriteCallback(const EventCallback& cb);
		void setErrorCallback(const EventCallback& cb);
		void setCloseCallback(const EventCallback& cb);
		void enableReading(){events_ |= kReadEvent; update();}
		void disableReading(){events_ &= ~kReadEvent; update();}
		void enableWriting(){events_ |= kWriteEvent; update();}
		void disableWriting(){events_ &= ~kWriteEvent; update();}
		void disableAll(){events_ = kNoneEvent; update();}
		bool isReading() const {return events_ && kReadEvent;}
		bool isWriting() const {return events_ && kWriteEvent;}
		void update();
		EventLoop *ownerLoop(){
			return loop_;
		}
		void set_index(int idx){
			index_ = idx;
		}
		int index(){
			return index_;
		}
		int fd()const{
			return fd_;
		}
		int events() const{
			return events_;
		}
		void set_revents(int revt){ // used by poller
			revents_ = revt;
		}
		bool isNoneEvent() const{
			return events_ == kNoneEvent;
		}
		void remove();
		void handleEvent();
	private:
		static const int kNoneEvent;  
		static const int kReadEvent;  
		static const int kWriteEvent;
		
		const int fd_;
		EventLoop *loop_;
		int index_;  // index in poller
		int events_; // events set for poller
		int revents_; //events return by poller
		bool addedToLoop_;
		EventCallback readCallback_;
		EventCallback writeCallback_;
		EventCallback errorCallback_;
		EventCallback closeCallback_;


};

#endif