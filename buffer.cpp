#include "buffer.h"
#include "base/netopr.h"

#include <sys/uio.h>

const char Buffer::kCRLF[] = "\r\n";

const size_t Buffer::kInitialSize;  //static, must specify class outside 
const size_t Buffer::kCheapPrepend;
ssize_t Buffer::readFd(int fd, int *saveErrno)
{
	char extrabuf[65536];
	struct iovec vec[2];

	const size_t writable = writableBytes();
	vec[0].iov_base = begin() + writerIndex_;
	vec[0].iov_len = writable;
	vec[1].iov_base = extrabuf;
	vec[1].iov_len = sizeof extrabuf;
	const int iovcnt = writable < sizeof extrabuf ? 2 : 1;
	const ssize_t n = bnet::readv(fd, vec, iovcnt);

	if(n < 0){
		return n;
	}
	else if(static_cast<size_t>(n) <= writable){
		writerIndex_ += n;
	}
	else{
		writerIndex_ = buffer_.size();  //+= writable;
		append(extrabuf, n-writable);
	}
	return n;
}