#ifndef NET_BUFFER_H
#define NET_BUFFER_H

#include <vector>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>
#include <string.h>

#include "base/endian.h"

class Buffer{
	public:
		static const size_t kInitialSize = 1024;
		static const size_t kCheapPrepend = 8;
		
		explicit Buffer(size_t initialSize = kInitialSize)
								:readerIndex_(kCheapPrepend),
								writerIndex_(kCheapPrepend),
								buffer_(initialSize + kCheapPrepend)
		{
			assert(readableBytes() == 0);
			assert(writableBytes() == initialSize);
			assert(prependableBytes() == kCheapPrepend);
		}
		~Buffer(){}

		size_t readableBytes() const {
			return (writerIndex_ - readerIndex_);
		}

		size_t writableBytes() const {
			return buffer_.size() - writerIndex_;
		}

		size_t prependableBytes() const {
			return readerIndex_;
		}

		ssize_t readFd(int fd, int *saveErrno);

		std::string retriveAllAsString(){
			std::string result(beginRead(), readableBytes());
			retriveAll();
			return result;
		}

		void retriveAll(){
			readerIndex_ = kCheapPrepend;
			writerIndex_ = kCheapPrepend;
		}

		void retrive(size_t len){
			assert(len <= readableBytes());
			if(len < readableBytes()){
				readerIndex_ += len;
			}
			else{
				retriveAll();
			}
		}
		
		void append(const char *data, size_t len){
			ensureWritableBytes(len);
			std::copy(data, data+len, beginWrite());
			hasWritten(len);			
		}

		char *beginRead() {
			return begin() + readerIndex_;
		}

		int16_t tryReadInt16(EUNM_ENDIAN1 endian){
			return peekInt16(endian);
		}

		int16_t readInt16(EUNM_ENDIAN1 endian){
			int16_t result = peekInt16(endian);
			retrive(sizeof(int16_t));
			return result;
		}

		void tryReadBytes(size_t len, char *buf){
			assert(readableBytes() >= len);
			::memcpy(buf, beginRead(), len);
		}

		void readBytes(size_t len, char *buf){
			assert(readableBytes() >= len);
			::memcpy(buf, beginRead(), len);
			retrive(len);
		}

		int16_t peekInt16(EUNM_ENDIAN1 endian) {
			assert(readableBytes() >= sizeof(int16_t));
			char be16[sizeof(int16_t)]={0};
			::memcpy(be16, (void *)beginRead(), sizeof(int16_t));
			return u8_u16((uint8_t *)be16, endian);
		}
		
		
	private:
		char *begin(){
			return &*buffer_.begin();
		}
		
		char *beginWrite() {
			return begin() + writerIndex_;
		}
		
		void hasWritten(size_t len){
			assert(len <= writableBytes());
			writerIndex_ += len;
		}

		void ensureWritableBytes(size_t len){
			if(len > writableBytes()){
				makeSpace(len);
			}
			assert(writableBytes() >= len);
		}

		void makeSpace(size_t len){
			if(writableBytes()+prependableBytes() < len+kCheapPrepend){
				buffer_.resize(writerIndex_+len);
			}else{
				const size_t readable = readableBytes();
				std::copy(begin()+readerIndex_,
						begin()+writerIndex_,
						begin()+kCheapPrepend);

				readerIndex_ = kCheapPrepend;
				writerIndex_ = readerIndex_ + readable;
				assert(readable == readableBytes());
			}
		}

	private:
		std::vector<char> buffer_;
		size_t readerIndex_;
		size_t writerIndex_;
		static const char kCRLF[];
	

};

#endif