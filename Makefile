###########################################
#Makefile
###########################################

#CROSS_COMPILE = arm-linux-
CROSS_COMPILE = 
AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)g++
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm

STRIP		= $(CROSS_COMPILE)strip
OBJCOPY		= $(CROSS_COMPILE)objcopy
OBJDUMP		= $(CROSS_COMPILE)objdump

export AS LD CC CPP AR NM
export STRIP OBJCOPY OBJDUMP

#CFLAGS := -Wall -O2 -g
CFLAGS := -Wall -g -std=c++11
CFLAGS +=
LDFLAGS := -lpthread

export CFLAGS LDFLAGS

TOPDIR := $(shell pwd)
export TOPDIR

TARGET := myserver

obj-y += main.o server.o tcp_server.o acceptor.o channel.o event_loop.o evloop_thread.o \
			evloop_thpool.o tcp_connection.o buffer.o timer_queue.o timer.o
obj-y += poller/
obj-y += base/
obj-y += application/

all : 
	make -C ./ -f $(TOPDIR)/Makefile.build
	$(CC) -o $(TARGET) built-in.o $(CFLAGS) $(LDFLAGS)
	
clean:
	rm -f $(shell find -name "*.o")
	rm -f $(TARGET)

distclean:
	rm -f $(shell find -name "*.o")
	rm -f $(shell find -name "*.d")
	rm -f $(TARGET)

.PHONY:print
print:
	@echo $(CFLAGS)