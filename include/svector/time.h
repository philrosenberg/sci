#pragma once

#include <ctime>
#include "Units.h"
#include<limits>
#include<string>
#include<iomanip>
#include<sstream>


namespace sci
{
	namespace internal
	{
		//day of the year that each month starts on -1. Note year day starts at 0 and month day starts at 1
		const int monthStartDay[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
		//day of the week that each year starts on starting at 1st jan 1900
		const std::time_t yearStartDay[400] = { 1, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0, 1, 3, 4, 5, 6, 1, 2, 3, 4, 6, 0, 1, 2, 4, 5, 6, 0, 2, 3, 4, 5, 0, 1, 2, 3, 5, 6, 0 };
		//cumulative number of days in years starting from 1900. This is a repeating pattern every 400 years
		const int cumulativeDays[400] = { 0, 365, 730, 1095, 1460, 1826, 2191, 2556, 2921, 3287, 3652, 4017, 4382, 4748, 5113, 5478, 5843, 6209, 6574, 6939, 7304, 7670, 8035, 8400, 8765, 9131, 9496, 9861, 10226, 10592, 10957, 11322, 11687, 12053, 12418, 12783, 13148, 13514, 13879, 14244, 14609, 14975, 15340, 15705, 16070, 16436, 16801, 17166, 17531, 17897, 18262, 18627, 18992, 19358, 19723, 20088, 20453, 20819, 21184, 21549, 21914, 22280, 22645, 23010, 23375, 23741, 24106, 24471, 24836, 25202, 25567, 25932, 26297, 26663, 27028, 27393, 27758, 28124, 28489, 28854, 29219, 29585, 29950, 30315, 30680, 31046, 31411, 31776, 32141, 32507, 32872, 33237, 33602, 33968, 34333, 34698, 35063, 35429, 35794, 36159, 36524, 36890, 37255, 37620, 37985, 38351, 38716, 39081, 39446, 39812, 40177, 40542, 40907, 41273, 41638, 42003, 42368, 42734, 43099, 43464, 43829, 44195, 44560, 44925, 45290, 45656, 46021, 46386, 46751, 47117, 47482, 47847, 48212, 48578, 48943, 49308, 49673, 50039, 50404, 50769, 51134, 51500, 51865, 52230, 52595, 52961, 53326, 53691, 54056, 54422, 54787, 55152, 55517, 55883, 56248, 56613, 56978, 57344, 57709, 58074, 58439, 58805, 59170, 59535, 59900, 60266, 60631, 60996, 61361, 61727, 62092, 62457, 62822, 63188, 63553, 63918, 64283, 64649, 65014, 65379, 65744, 66110, 66475, 66840, 67205, 67571, 67936, 68301, 68666, 69032, 69397, 69762, 70127, 70493, 70858, 71223, 71588, 71954, 72319, 72684, 73049, 73414, 73779, 74144, 74509, 74875, 75240, 75605, 75970, 76336, 76701, 77066, 77431, 77797, 78162, 78527, 78892, 79258, 79623, 79988, 80353, 80719, 81084, 81449, 81814, 82180, 82545, 82910, 83275, 83641, 84006, 84371, 84736, 85102, 85467, 85832, 86197, 86563, 86928, 87293, 87658, 88024, 88389, 88754, 89119, 89485, 89850, 90215, 90580, 90946, 91311, 91676, 92041, 92407, 92772, 93137, 93502, 93868, 94233, 94598, 94963, 95329, 95694, 96059, 96424, 96790, 97155, 97520, 97885, 98251, 98616, 98981, 99346, 99712, 100077, 100442, 100807, 101173, 101538, 101903, 102268, 102634, 102999, 103364, 103729, 104095, 104460, 104825, 105190, 105556, 105921, 106286, 106651, 107017, 107382, 107747, 108112, 108478, 108843, 109208, 109573, 109938, 110303, 110668, 111033, 111399, 111764, 112129, 112494, 112860, 113225, 113590, 113955, 114321, 114686, 115051, 115416, 115782, 116147, 116512, 116877, 117243, 117608, 117973, 118338, 118704, 119069, 119434, 119799, 120165, 120530, 120895, 121260, 121626, 121991, 122356, 122721, 123087, 123452, 123817, 124182, 124548, 124913, 125278, 125643, 126009, 126374, 126739, 127104, 127470, 127835, 128200, 128565, 128931, 129296, 129661, 130026, 130392, 130757, 131122, 131487, 131853, 132218, 132583, 132948, 133314, 133679, 134044, 134409, 134775, 135140, 135505, 135870, 136236, 136601, 136966, 137331, 137697, 138062, 138427, 138792, 139158, 139523, 139888, 140253, 140619, 140984, 141349, 141714, 142080, 142445, 142810, 143175, 143541, 143906, 144271, 144636, 145002, 145367, 145732 };
		const int daysIn400Years = 146097;
		const std::time_t secondsPerDay = 24 * 60 * 60;
		const std::time_t secondsPerHour = 60 * 60;
	}
	class UtcTime;
	typedef sci::Physical<Second<>, double> TimeInterval;

	constexpr sci::TimeInterval operator-(const sci::UtcTime &t1, const sci::UtcTime &t2);
	constexpr sci::UtcTime operator-(const sci::UtcTime &time, const sci::TimeInterval &interval);
	constexpr sci::UtcTime operator+(const sci::UtcTime &time, const sci::TimeInterval &interval);
	constexpr sci::UtcTime operator+(const sci::TimeInterval &interval, const sci::UtcTime &time);
	constexpr sci::UtcTime& operator-=(sci::UtcTime &time, const sci::TimeInterval &interval);
	constexpr sci::UtcTime& operator+=(sci::UtcTime &time, const sci::TimeInterval &interval);
	constexpr bool operator<(const sci::UtcTime &t1, const sci::UtcTime &t2);
	constexpr bool operator>(const sci::UtcTime &t1, const sci::UtcTime &t2);
	constexpr bool operator<=(const sci::UtcTime &t1, const sci::UtcTime &t2);
	constexpr bool operator>=(const sci::UtcTime &t1, const sci::UtcTime &t2);
	constexpr bool operator==(const sci::UtcTime &t1, const sci::UtcTime &t2);
	constexpr bool operator!=(const sci::UtcTime &t1, const sci::UtcTime &t2);

	class UtcTime
	{
		friend constexpr TimeInterval(operator-)(const UtcTime &, const UtcTime &);
		friend constexpr UtcTime(operator-)(const sci::UtcTime &time, const sci::TimeInterval &interval);
		friend constexpr UtcTime(operator+)(const sci::UtcTime &time, const sci::TimeInterval &interval);
		friend constexpr bool(operator<)(const sci::UtcTime &t1, const sci::UtcTime &t2);
		friend constexpr bool(operator>)(const sci::UtcTime &t1, const sci::UtcTime &t2);
		friend constexpr bool(operator<=)(const sci::UtcTime &t1, const sci::UtcTime &t2);
		friend constexpr bool(operator>=)(const sci::UtcTime &t1, const sci::UtcTime &t2);
		friend constexpr bool(operator==)(const sci::UtcTime &t1, const sci::UtcTime &t2);
		friend constexpr bool(operator!=)(const sci::UtcTime &t1, const sci::UtcTime &t2);
		friend class std::numeric_limits<sci::UtcTime>;
	public:
		constexpr UtcTime()
			: m_secondFraction(0.0), m_secsAfterPosixEpoch(0), m_cTime{0, 0, 0, 1, 0, 70, 4, 0}
		{
		}
		constexpr UtcTime(int year, unsigned int month, unsigned int dayOfMonth, unsigned int hour, unsigned int minute, double second)
			: m_secondFraction(0.0), m_secsAfterPosixEpoch(0)
		{
			set(year, month, dayOfMonth, hour, minute, second);
		}
		constexpr TimeInterval getUnixTime() const
		{
			return sci::TimeInterval(m_secsAfterPosixEpoch + m_secondFraction);
		}
		constexpr void set(int year, unsigned int month, unsigned int dayOfMonth, unsigned int hour, unsigned int minute, double second)
		{
			m_cTime.tm_year = year - 1900;
			m_cTime.tm_mon = month - 1;
			m_cTime.tm_mday = dayOfMonth;
			m_cTime.tm_hour = hour;
			m_cTime.tm_min = minute;
			m_cTime.tm_sec = int(second);
			if (second < 0)
				second--;
			m_cTime.tm_isdst = 0;
			m_secondFraction = second - m_cTime.tm_sec;
			m_secsAfterPosixEpoch = mkgmtime(m_cTime);
		}
		constexpr void setTime(unsigned int hour, unsigned int minute, double second)
		{
			m_cTime.tm_hour = hour;
			m_cTime.tm_min = minute;
			m_cTime.tm_sec = int(second);
			if (second < 0)
				second--;
			m_cTime.tm_isdst = 0;
			m_secondFraction = second - m_cTime.tm_sec;
			m_secsAfterPosixEpoch = mkgmtime(m_cTime);
		}
		constexpr void setDate(int year, unsigned int month, unsigned int dayOfMonth)
		{
			m_cTime.tm_year = year - 1900;
			m_cTime.tm_mon = month - 1;
			m_cTime.tm_mday = dayOfMonth;
			m_secsAfterPosixEpoch = mkgmtime(m_cTime);
		}
		constexpr int getYear() const
		{
			return m_cTime.tm_year + 1900;
		}
		constexpr unsigned int getMonth() const
		{
			return m_cTime.tm_mon + 1;
		}
		constexpr unsigned int getDayOfMonth() const
		{
			return m_cTime.tm_mday;
		}
		constexpr unsigned int getHour() const
		{
			return m_cTime.tm_hour;
		}
		constexpr unsigned int getMinute() const
		{
			return m_cTime.tm_min;
		}
		constexpr double getSecond() const
		{
			return double(m_cTime.tm_sec) + m_secondFraction;
		}
		static UtcTime now();
		constexpr static UtcTime getPosixEpoch()
		{
			return UtcTime();
		}
		static constexpr bool isLeapYear(int year)
		{
			if (year % 4 != 0)
				return false;
			if (year % 100 != 0)
				return true; //years divisible by 4, but not by 100
			if (year % 400 == 0)
				return true; //years divisible by 4, and by 100 and by 400
			return false; //years divisible by 4 and 100 and not 400
		}
		std::string getIso8601String(int secondDecimalPlaces = 0, bool omitSeparators = false, bool omitTimePrefix = false, bool omitTimeZoneSuffix = false) const
		{
			std::stringstream stream;
			std::string dateSeparator = omitSeparators ? "" : "-";
			std::string timeSeparator = omitSeparators ? "" : ":";
			std::string timePrefix = omitTimePrefix ? "" : "T";
			std::string timeSuffix = omitTimeZoneSuffix ? "" : "Z";

			stream << std::setw(4) << std::setfill('0') << getYear()
				<< dateSeparator
				<< std::setw(2) << std::setfill('0') << getMonth()
				<< dateSeparator
				<< std::setw(2) << std::setfill('0') << getDayOfMonth()
				<< timePrefix
				<< std::setw(2) << std::setfill('0') << getHour()
				<< timeSeparator
				<< std::setw(2) << std::setfill('0') << getMinute()
				<< timeSeparator
				<< std::fixed << std::setw(secondDecimalPlaces + (secondDecimalPlaces > 0 ? 1 : 0) + 2) << std::setfill('0') << std::setprecision(secondDecimalPlaces) << getSecond()
				<< timeSuffix;
			return stream.str();
		}
	private:
		std::tm m_cTime;
		std::time_t m_secsAfterPosixEpoch;
		double m_secondFraction;
		constexpr UtcTime(std::tm time, double secondFraction)
			: m_secondFraction(0.0), m_secsAfterPosixEpoch(0)
		{
			set(time, secondFraction);
		}
		constexpr UtcTime(std::time_t secondInteger, double secondFraction)
			: m_secondFraction(secondFraction), m_secsAfterPosixEpoch(secondInteger), m_cTime(gmtime(secondInteger))
		{
		}
		constexpr void set(std::tm time, double secondFraction)
		{
			m_cTime = time;
			m_secondFraction = secondFraction;
			m_secsAfterPosixEpoch = mkgmtime(m_cTime);
		}

		static constexpr std::time_t mkgmtime(std::tm& time)
		{
			time.tm_yday = internal::monthStartDay[time.tm_mon] + time.tm_mday - 1; // -1 because month day starts at 1 and year day starts at 0
			if (time.tm_mon > 1 && isLeapYear(time.tm_year + 1900))
				time.tm_yday += 1;

			int yearStartIndex = time.tm_year % 400;
			if (yearStartIndex < 0)
				yearStartIndex += 400;
			time.tm_wday = (internal::yearStartDay[yearStartIndex] + time.tm_yday) % 7;

			std::time_t repeatingPeriods = time.tm_year / 400;
			if (repeatingPeriods < 0)
				repeatingPeriods -= 1;
			int partYears = (time.tm_year - repeatingPeriods * 400) % 400;


			std::time_t result = (repeatingPeriods * internal::daysIn400Years + internal::cumulativeDays[partYears] - internal::cumulativeDays[70] + time.tm_yday) * internal::secondsPerDay; //this is all the whole days
			result += time.tm_hour * internal::secondsPerHour;
			result += time.tm_min * 60;
			result += time.tm_sec;

			return result;
		}

		static constexpr std::tm gmtime(std::time_t time)
		{
			std::tm timeStruct{};

			//reference to 1900 rather than 1970
			time += internal::cumulativeDays[70] * internal::secondsPerDay;

			const std::time_t secondsIn400Years = (internal::daysIn400Years * internal::secondsPerDay);

			//now reference to a 400 year multiple before the date in question
			std::time_t reference400YearChunk = time / secondsIn400Years;
			if (reference400YearChunk < 0)
				reference400YearChunk -= 1;

			std::time_t remainingSeconds = time - reference400YearChunk * secondsIn400Years; // this will always be positive, because we ensured referenceYear is before time.
			int remainingDays = int(remainingSeconds / internal::secondsPerDay);
			remainingSeconds -= remainingDays * internal::secondsPerDay;
			int year = remainingDays / 365;
			if (year == 400 || internal::cumulativeDays[year] > remainingDays)
				--year;
			remainingDays -= int(internal::cumulativeDays[year]);

			timeStruct.tm_year = int(reference400YearChunk * 400 + year);
			timeStruct.tm_yday = remainingDays;
			timeStruct.tm_wday = (internal::yearStartDay[year] + remainingDays) % 7;
			timeStruct.tm_hour = int(remainingSeconds) / internal::secondsPerHour;
			remainingSeconds = remainingSeconds % internal::secondsPerHour;
			timeStruct.tm_min = int(remainingSeconds) / 60;
			timeStruct.tm_sec = remainingSeconds % 60;
			timeStruct.tm_isdst = 0;
			//work out month and day of month
			bool isLeapYear = UtcTime::isLeapYear(timeStruct.tm_year + 1900);
			if (remainingDays < 31)
			{
				timeStruct.tm_mon = 0;
				timeStruct.tm_mday = remainingDays + 1;
			}
			else if (remainingDays < 59)
			{
				timeStruct.tm_mon = 1;
				timeStruct.tm_mday = remainingDays - 30;
			}
			else if (isLeapYear && remainingDays == 59)
			{
				timeStruct.tm_mon = 1;
				timeStruct.tm_mday = 29;
			}
			else
			{
				if (isLeapYear)
					--remainingDays;
				int month = remainingDays / 29;
				if (month == 12 || internal::monthStartDay[month] > remainingDays)
					--month;
				timeStruct.tm_mon = month;
				timeStruct.tm_mday = remainingDays - internal::monthStartDay[month] + 1;
			}

			return timeStruct;
		}
	};


	constexpr sci::TimeInterval operator-(const sci::UtcTime& t1, const sci::UtcTime& t2)
	{
		//set up this way so that it should avoid rounding errors for very small time differences a long way from 1st Jan 1970
		return sci::TimeInterval(double(t1.m_secsAfterPosixEpoch - t2.m_secsAfterPosixEpoch)) + sci::TimeInterval(t1.m_secondFraction - t2.m_secondFraction);
	}
	constexpr sci::UtcTime operator-(const sci::UtcTime& time, const sci::TimeInterval& interval)
	{
		time_t integerSeconds = time.m_secsAfterPosixEpoch;
		time_t integerInterval = (time_t)interval.value<sci::Second<>>();
		if (integerInterval < 0)
			--integerInterval;
		integerSeconds -= integerInterval;
		double fraction = time.m_secondFraction - (interval.value<sci::Second<>>() - (double)integerInterval);
		if (fraction < 0.0)
		{
			--integerSeconds;
			fraction += 1.0;
		}

		return sci::UtcTime(integerSeconds, fraction);
	}
	constexpr sci::UtcTime operator+(const sci::UtcTime& time, const sci::TimeInterval& interval)
	{
		time_t integerSeconds = time.m_secsAfterPosixEpoch;
		time_t integerInterval = (time_t)interval.value<sci::Second<>>();
		if (integerInterval < 0)
			--integerInterval;
		integerSeconds += integerInterval;
		double fraction = time.m_secondFraction + (interval.value<sci::Second<>>() - (double)integerInterval);
		if (fraction >= 1.0)
		{
			++integerSeconds;
			fraction -= 1.0;
		}

		return sci::UtcTime(integerSeconds, fraction);
	}
	constexpr sci::UtcTime operator+(const sci::TimeInterval& interval, const sci::UtcTime& time)
	{
		return time + interval;
	}
	constexpr sci::UtcTime& operator-=(sci::UtcTime& time, const sci::TimeInterval& interval)
	{
		time = time - interval;
		return time;
	}
	constexpr sci::UtcTime& operator+=(sci::UtcTime& time, const sci::TimeInterval& interval)
	{
		time = time + interval;
		return time;
	}
	constexpr bool operator<(const sci::UtcTime& t1, const sci::UtcTime& t2)
	{
		if (t1.m_secondFraction != t1.m_secondFraction || t2.m_secondFraction != t2.m_secondFraction)
			return false;
		if (t2.m_secondFraction == std::numeric_limits<double>::infinity() && t1.m_secondFraction != std::numeric_limits<double>::infinity())
			return true;
		if (t1.m_secondFraction == -std::numeric_limits<double>::infinity() && t2.m_secondFraction != -std::numeric_limits<double>::infinity())
			return true;
		if (t1.m_secsAfterPosixEpoch == t2.m_secsAfterPosixEpoch)
			return (t1.m_secondFraction < t2.m_secondFraction);
		return (t1.m_secsAfterPosixEpoch < t2.m_secsAfterPosixEpoch);
	}
	constexpr bool operator>(const sci::UtcTime& t1, const sci::UtcTime& t2)
	{
		if (t1.m_secondFraction != t1.m_secondFraction || t2.m_secondFraction != t2.m_secondFraction)
			return false;
		if (t1.m_secondFraction == std::numeric_limits<double>::infinity() && t2.m_secondFraction != std::numeric_limits<double>::infinity())
			return true;
		if (t2.m_secondFraction == -std::numeric_limits<double>::infinity() && t1.m_secondFraction != -std::numeric_limits<double>::infinity())
			return true;
		if (t1.m_secsAfterPosixEpoch == t2.m_secsAfterPosixEpoch)
			return (t1.m_secondFraction > t2.m_secondFraction);
		return (t1.m_secsAfterPosixEpoch > t2.m_secsAfterPosixEpoch);
	}
	constexpr bool operator<=(const sci::UtcTime& t1, const sci::UtcTime& t2)
	{
		if (t1.m_secondFraction != t1.m_secondFraction || t2.m_secondFraction != t2.m_secondFraction)
			return false;
		return !(t1 > t2);
	}
	constexpr bool operator>=(const sci::UtcTime& t1, const sci::UtcTime& t2)
	{
		if (t1.m_secondFraction != t1.m_secondFraction || t2.m_secondFraction != t2.m_secondFraction)
			return false;
		return !(t1 < t2);
	}
	constexpr bool operator==(const sci::UtcTime& t1, const sci::UtcTime& t2)
	{
		if (t1.m_secondFraction == std::numeric_limits<double>::infinity() && t2.m_secondFraction == std::numeric_limits<double>::infinity())
			return true;
		if (t1.m_secondFraction == -std::numeric_limits<double>::infinity() && t2.m_secondFraction == -std::numeric_limits<double>::infinity())
			return true;
		return t1.m_secsAfterPosixEpoch == t2.m_secsAfterPosixEpoch && t1.m_secondFraction == t2.m_secondFraction;
	}
	constexpr bool operator!=(const sci::UtcTime& t1, const sci::UtcTime& t2)
	{
		if (t1.m_secondFraction == std::numeric_limits<double>::infinity() && t2.m_secondFraction == std::numeric_limits<double>::infinity())
			return false;
		if (t1.m_secondFraction == -std::numeric_limits<double>::infinity() && t2.m_secondFraction == -std::numeric_limits<double>::infinity())
			return false;
		return t1.m_secsAfterPosixEpoch != t2.m_secsAfterPosixEpoch || t1.m_secondFraction != t2.m_secondFraction;
	}
}


//include this between min or max and the opening parenthesis
//and the macro expansion of this define will avoid macro
//expansion of windows.h min and max
#define AVOIDWINDOWSMINMAX

namespace std
{
	template <>
	class numeric_limits<sci::UtcTime> : public numeric_limits<sci::TimeInterval>
	{
	public:
		static constexpr sci::UtcTime(min)() noexcept
		{
			return sci::UtcTime(std::numeric_limits<int>::min AVOIDWINDOWSMINMAX (), 0, 0, 0, 0, 0.0);
		}
		static constexpr sci::UtcTime(max)() noexcept
		{
			return sci::UtcTime(std::numeric_limits<int>::max AVOIDWINDOWSMINMAX (), 11, 31, 23, 59, 1.0 - std::numeric_limits<double>::epsilon());
		}

		static constexpr sci::UtcTime lowest() noexcept
		{
			return std::numeric_limits<sci::UtcTime>::min AVOIDWINDOWSMINMAX ();
		}

		static constexpr sci::UtcTime infinity() noexcept
		{
			sci::UtcTime result;
			result.m_secondFraction = std::numeric_limits<double>::infinity();
			return result;
		}

		static constexpr sci::UtcTime quiet_NaN() noexcept
		{
			sci::UtcTime result;
			result.m_secondFraction = std::numeric_limits<double>::quiet_NaN();
			return result;
		}

		static constexpr sci::UtcTime signaling_NaN() noexcept
		{
			sci::UtcTime result;
			result.m_secondFraction = std::numeric_limits<double>::signaling_NaN();
			return result;
		}
	};

	template <>
	class numeric_limits<const sci::UtcTime> : public numeric_limits<sci::UtcTime>
	{
	};
	template <>
	class numeric_limits<volatile sci::UtcTime> : public numeric_limits<sci::UtcTime>
	{
	};
	template <>
	class numeric_limits<const volatile sci::UtcTime> : public numeric_limits<sci::UtcTime>
	{
	};

	inline bool isnan(const sci::UtcTime &time)
	{
		return isnan(time.getSecond());
	}
	inline bool isinf(const sci::UtcTime& time)
	{
		return isinf(time.getSecond());
	}
	inline bool isfinite(const sci::UtcTime& time)
	{
		return isfinite(time.getSecond());
	}
	inline bool isnormal(const sci::UtcTime& time)
	{
		return isnormal(time.getSecond());
	}
}

#undef AVOIDWINDOWSMINMAX