#ifndef BASE_TIME_STAMP_H
#define BASE_TIME_STAMP_H

#include <stdint.h>

class TimeStamp{
	public:
		TimeStamp();
		TimeStamp(int64_t microSec);
		~TimeStamp();
		static int64_t now();
		void addTime(double interval);
		int64_t microSec() const {return microSec_; }

		static TimeStamp invalid(){
			return TimeStamp();
		}

		bool valid() const {
			return microSec_ > 0;
		}
		
	private:
		int64_t microSec_;
};

inline bool operator<(TimeStamp lhs, TimeStamp rhs)
{
  return lhs.microSec() < rhs.microSec();
}

inline bool operator==(TimeStamp lhs, TimeStamp rhs)
{
  return lhs.microSec() == rhs.microSec();
}





#endif
