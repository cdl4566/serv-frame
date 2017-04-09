#include "poller.h"

Poller::Poller(EventLoop *loop)
					:ownerLoop_(loop)
{
}

Poller::~Poller()
{
}
