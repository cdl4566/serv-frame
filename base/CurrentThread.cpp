#include "CurrentThread.h"

__thread int t_cachedTid;


pid_t CurrentThread::gettid(){
	return static_cast<pid_t>(::syscall(SYS_gettid));
}
void CurrentThread::cacheTid(){
	if(t_cachedTid == 0){
		t_cachedTid = gettid();
	}
}

int CurrentThread::tid(){
	if(__builtin_expect(t_cachedTid == 0, 0)){ //t_cachedTid is not likely, for compiler optimize
		cacheTid();
	}
	return t_cachedTid;
}

