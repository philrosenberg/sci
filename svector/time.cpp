#define __STDC_WANT_LIB_EXT1__
#include"../include/svector/time.h"
#include<cmath>
#include<mutex>
#include"../include/svector/serr.h"





sci::UtcTime sci::UtcTime::now()
{
	auto time = std::chrono::system_clock::now();
	decltype(time) epoch = std::chrono::system_clock::from_time_t(0); //system_clock does not include leap seconds
	std::chrono::duration<float> secondsSinceEpoch = time - epoch;
	UtcTime result = UtcTime(1970, 1, 1, 0, 0, 0.0) + sci::TimeInterval((double)secondsSinceEpoch.count());
	return result;
}
