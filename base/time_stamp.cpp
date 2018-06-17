#include "time_stamp.h"

#include <sys/time.h>
#include <unistd.h>

TimeStamp::TimeStamp():microSec_(0)
{

}

TimeStamp::TimeStamp(int64_t microSec):microSec_(microSec)
{

}

TimeStamp::~TimeStamp()
{

}

int64_t TimeStamp::now(){
	struct timeval tv;
	gettimeofday(&tv, NULL);

	return 1000000 * tv.tv_sec + tv.tv_usec;
}

void TimeStamp::addTime(double interval)
{
	microSec_ += interval * 1000000;
}


