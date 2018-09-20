#pragma once

#include <ctime>

namespace sci
{
	class UtcTime;
	typedef double TimeInterval;
}

sci::TimeInterval operator-(const sci::UtcTime &t1, const sci::UtcTime &t2);
sci::UtcTime operator-(const sci::UtcTime &time, const sci::TimeInterval &interval);
sci::UtcTime operator+(const sci::UtcTime &time, const sci::TimeInterval &interval);
sci::UtcTime operator+(const sci::TimeInterval &interval, const sci::UtcTime &time);
sci::UtcTime& operator-=(sci::UtcTime &time, const sci::TimeInterval &interval);
sci::UtcTime& operator+=(sci::UtcTime &time, const sci::TimeInterval &interval);
bool operator<(const sci::UtcTime &t1, const sci::UtcTime &t2);
bool operator>(const sci::UtcTime &t1, const sci::UtcTime &t2);
bool operator<=(const sci::UtcTime &t1, const sci::UtcTime &t2);
bool operator>=(const sci::UtcTime &t1, const sci::UtcTime &t2);
bool operator==(const sci::UtcTime &t1, const sci::UtcTime &t2);

namespace sci
{

	class UtcTime
	{
		friend TimeInterval(::operator-)(const UtcTime &, const UtcTime &);
		friend UtcTime(::operator-)(const sci::UtcTime &time, const sci::TimeInterval &interval);
		friend UtcTime(::operator+)(const sci::UtcTime &time, const sci::TimeInterval &interval);
		friend bool(::operator<)(const sci::UtcTime &t1, const sci::UtcTime &t2);
		friend bool(::operator>)(const sci::UtcTime &t1, const sci::UtcTime &t2);
		friend bool(::operator<=)(const sci::UtcTime &t1, const sci::UtcTime &t2);
		friend bool(::operator>=)(const sci::UtcTime &t1, const sci::UtcTime &t2);
		friend bool(::operator==)(const sci::UtcTime &t1, const sci::UtcTime &t2);
	public:
		UtcTime();
		UtcTime(int year, unsigned int month, unsigned int dayOfMonth, unsigned int hour, unsigned int minute, double second);
		TimeInterval getUnixTime() const;
		void set(int year, unsigned int month, unsigned int dayOfMonth, unsigned int hour, unsigned int minute, double second);
		void setTime(unsigned int hour, unsigned int minute, double second);
		void setDate(int year, unsigned int month, unsigned int dayOfMonth);
		int getYear() const;
		unsigned int getMonth() const;
		unsigned int getDayOfMonth() const;
		unsigned int getHour() const;
		unsigned int getMinute() const;
		double getSecond() const;
		static UtcTime now();
	private:
		std::tm m_cTime;
		std::time_t m_secsAfterPosixEpoch;
		sci::TimeInterval m_secondFraction;
		UtcTime(std::tm time, double secondFraction);
		void set(std::tm time, double secondFraction);
	};
}
