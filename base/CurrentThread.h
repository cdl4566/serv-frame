#ifndef BASE_CURRENT_THREAD_H
#define BASE_CURRENT_THREAD_H

#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace CurrentThread {


pid_t gettid();
void cacheTid();

int tid(); //can not be inline ?

}


#endif

