#include"../include/svector/dep/time.h"
#include<cmath>
#include<mutex>

std::mutex g_timeMutex;

sci::UtcTime::UtcTime()
{
	m_cTime.tm_year = 70;
	m_cTime.tm_mon = 0;
	m_cTime.tm_mday = 0;
	m_cTime.tm_hour = 0;
	m_cTime.tm_min = 0;
	m_cTime.tm_sec = 0;
	m_cTime.tm_isdst = 0;
	m_secondFraction = 0.0;
	m_secsAfterPosixEpoch = _mkgmtime(&m_cTime); 
}

sci::UtcTime::UtcTime(int year, unsigned int month, unsigned int dayOfMonth, unsigned int hour, unsigned int minute, double second)
{
	set(year, month, dayOfMonth, hour, minute, second);
}

void sci::UtcTime::set(int year, unsigned int month, unsigned int dayOfMonth, unsigned int hour, unsigned int minute, double second)
{
	m_cTime.tm_year = year-1900;
	m_cTime.tm_mon = month - 1;
	m_cTime.tm_mday = dayOfMonth;
	m_cTime.tm_hour = hour;
	m_cTime.tm_min = minute;
	m_cTime.tm_sec = int(std::floor(second));
	m_cTime.tm_isdst = 0;
	m_secondFraction = second - std::floor(second);
	m_secsAfterPosixEpoch = _mkgmtime(&m_cTime); 
}

void sci::UtcTime::setTime(unsigned int hour, unsigned int minute, double second)
{
	m_cTime.tm_hour = hour;
	m_cTime.tm_min = minute;
	m_cTime.tm_sec = int(std::floor(second));
	m_cTime.tm_isdst = 0;
	m_secondFraction = second - std::floor(second);
	m_secsAfterPosixEpoch = _mkgmtime(&m_cTime);
}

void sci::UtcTime::setDate(int year, unsigned int month, unsigned int dayOfMonth)
{
	m_cTime.tm_year = year - 1900;
	m_cTime.tm_mon = month - 1;
	m_cTime.tm_mday = dayOfMonth;
	m_secsAfterPosixEpoch = _mkgmtime(&m_cTime);
}

int sci::UtcTime::getYear() const
{
	return m_cTime.tm_year+1900;
}
unsigned int sci::UtcTime::getMonth() const
{
	return m_cTime.tm_mon+1;
}
unsigned int sci::UtcTime::getDayOfMonth() const
{
	return m_cTime.tm_mday;
}
unsigned int sci::UtcTime::getHour() const
{
	return m_cTime.tm_hour;
}
unsigned int sci::UtcTime::getMinute() const
{
	return m_cTime.tm_min;
}
double sci::UtcTime::getSecond() const
{
	return sci::TimeInterval(m_cTime.tm_sec)+m_secondFraction;
}

sci::TimeInterval sci::UtcTime::getUnixTime() const
{
	return m_secsAfterPosixEpoch + m_secondFraction;
}

sci::UtcTime::UtcTime(std::tm time, double secondFraction)
{
	set(time, secondFraction);
}

void sci::UtcTime::set(std::tm time, double secondFraction)
{
	m_cTime = time;
	m_secondFraction = secondFraction;
	m_secsAfterPosixEpoch = _mkgmtime(&m_cTime);
}

sci::TimeInterval operator-(const sci::UtcTime &t1, const sci::UtcTime &t2)
{
	//set up this way so that it should avoid rounding errors for very small time differences a long way from 1st Jan 1970
	return sci::TimeInterval(t1.m_secsAfterPosixEpoch - t1.m_secsAfterPosixEpoch) + (t1.m_secondFraction - t1.m_secondFraction);
}

sci::UtcTime operator+(const sci::UtcTime &time, const sci::TimeInterval &interval)
{
	std::tm cTime = time.m_cTime;
	std::time_t integerSeconds = _mkgmtime(&cTime);
	integerSeconds += (time_t)std::floor(interval);
	double fraction = time.m_secondFraction+(interval+std::floor(interval));
	if (fraction >= 1.0)
	{
		++integerSeconds;
		fraction -= 1.0;
	}
	
	std::tm cTimeNew;
	g_timeMutex.lock();
	try
	{
		localtime_s(&cTimeNew, &integerSeconds);
	}
	catch (...)
	{
		g_timeMutex.unlock();
		throw;
	}
	g_timeMutex.unlock();

	cTimeNew.tm_isdst = 0;

	return sci::UtcTime(cTimeNew, fraction);
}

sci::UtcTime operator+(const sci::TimeInterval &interval, const sci::UtcTime &time)
{
	return time + interval;
}