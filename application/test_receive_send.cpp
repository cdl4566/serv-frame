#include "test_receive_send.h"
#include "../base/endian.h"
#include "crc.h"

#include <iostream>
#include <boost/atomic.hpp>

using namespace application;

void testrs::retrive_data(const TcpConnectionPtr &conn, Buffer *buf)
{
	//static int count=0; //for test, counting
	static boost::atomic_int count(0);
	testrs::RECVBUF_T recvbuf;

	if(buf->readableBytes() < sizeof(uint16_t)){
		return;
	}
	recvbuf.len = static_cast<uint16_t>(buf->tryReadInt16(LITTLE_ENDIAN1));
	if(buf->readableBytes() < sizeof(uint16_t) + recvbuf.len){
		return;
	}
	//buf->readInt16(LITTLE_ENDIAN1);
	buf->readBytes(static_cast<size_t>(recvbuf.len+2), (char *)recvbuf.buf);

	//check crc
	if(CRCCode_get(recvbuf.buf, recvbuf.len) ==
					u8_u16(&recvbuf.buf[recvbuf.len], LITTLE_ENDIAN1)){
		
		std::cout << "recv count:" << count++ << std::endl;
	}

	
	
}

