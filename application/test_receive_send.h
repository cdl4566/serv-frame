#ifndef APP_TEST_RECEIVE_SEND_H
#define APP_TEST_RECEIVE_SEND_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#include "../tcp_connection.h"
#include "../buffer.h"

namespace application{

namespace testrs{

typedef struct{
	uint16_t len;
	uint8_t buf[1024];

}__attribute__((packed)) RECVBUF_T;

void retrive_data(const TcpConnectionPtr &conn, Buffer *buf);


}

}

#endif