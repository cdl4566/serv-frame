#ifndef CALL_BACK_H
#define CALL_BACK_H

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>

class TcpConnection;
typedef boost::shared_ptr<TcpConnection> TcpConnectionPtr;

typedef boost::function<void (const TcpConnectionPtr&, Buffer *)> MessageCallback;
typedef boost::function<void (const TcpConnectionPtr&)> ConnectionCallback;

#endif