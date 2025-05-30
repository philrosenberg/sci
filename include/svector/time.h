#ifndef SCI_TIME_H
#define SCI_TIME_H

#include <ctime>
#include<chrono>
#include "Units.h"
#include<limits>
#include<string>
#include<iomanip>
#include<sstream>
#include<type_traits>


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

		template<std::time_t ...LEAP_SECONDS>
		class LeapSecondCorrector
		{
		public:
			constexpr static void addLeapSeconds(std::time_t& seconds, bool isSixtySeconds)
			{

			}
			constexpr static void subtractLeapSeconds(std::time_t& seconds)
			{
			}
			constexpr static bool isLeapSecond(std::time_t seconds)
			{
				return false;
			}
			constexpr static bool hasLeapSeconds()
			{
				return false;
			}
		};
		template<std::time_t LEAP_SECOND, std::time_t ...LEAP_SECONDS>
		class LeapSecondCorrector<LEAP_SECOND, LEAP_SECONDS...> : LeapSecondCorrector<LEAP_SECONDS...>
		{

		public:
			constexpr static void addLeapSeconds(std::time_t& seconds, bool isSixtySeconds)
			{
				if (seconds > LEAP_SECOND)
				{
					++seconds;
					LeapSecondCorrector<LEAP_SECONDS...>::addLeapSeconds(seconds, isSixtySeconds);
				}
				else if (!isSixtySeconds && seconds == LEAP_SECOND)
					++seconds;

			}
			constexpr static void subtractLeapSeconds(std::time_t& seconds)
			{
				if (seconds > LEAP_SECOND)
				{
					--seconds;
					LeapSecondCorrector<LEAP_SECONDS...>::subtractLeapSeconds(seconds);
				}
			}
			constexpr static bool isLeapSecond(std::time_t seconds)
			{
				return seconds == LEAP_SECOND || LeapSecondCorrector<LEAP_SECONDS...>::isLeapSecond(seconds);
			}
			constexpr static bool hasLeapSeconds(std::time_t seconds)
			{
				return true;
			}

		};
	}
	//class GregorianTime;
	typedef sci::Physical<Second<>, double> TimeInterval;

	/*template<std::time_t ...LEAP_SECONDS1, std::time_t ...LEAP_SECONDS2>
	constexpr sci::TimeInterval operator-(const sci::GregorianTime<LEAP_SECONDS1...>&t1, const sci::GregorianTime<LEAP_SECONDS2...>&t2);
	constexpr sci::GregorianTime operator-(const sci::GregorianTime &time, const sci::TimeInterval &interval);
	constexpr sci::GregorianTime operator+(const sci::GregorianTime &time, const sci::TimeInterval &interval);
	constexpr sci::GregorianTime operator+(const sci::TimeInterval &interval, const sci::GregorianTime &time);
	constexpr sci::GregorianTime& operator-=(sci::GregorianTime &time, const sci::TimeInterval &interval);
	constexpr sci::GregorianTime& operator+=(sci::GregorianTime &time, const sci::TimeInterval &interval);
	constexpr bool operator<(const sci::GregorianTime &t1, const sci::GregorianTime &t2);
	constexpr bool operator>(const sci::GregorianTime &t1, const sci::GregorianTime &t2);
	constexpr bool operator<=(const sci::GregorianTime &t1, const sci::GregorianTime &t2);
	constexpr bool operator>=(const sci::GregorianTime &t1, const sci::GregorianTime &t2);
	constexpr bool operator==(const sci::GregorianTime &t1, const sci::GregorianTime &t2);
	constexpr bool operator!=(const sci::GregorianTime &t1, const sci::GregorianTime &t2);*/

	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	class GregorianTime
	{
		template <std::time_t OTHER_BASE_OFFSET, std::time_t ...OTHER_LEAP_SECONDS>
		friend class GregorianTime;

		template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
		friend constexpr sci::TimeInterval operator-(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>& t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>& t2);
		
		template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1>
		friend constexpr GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>(operator-)(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>& time, const sci::TimeInterval& interval);
		
		template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1>
		friend constexpr sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...> operator+(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>& time, const sci::TimeInterval& interval);

		template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
		friend constexpr bool(operator<)(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>&t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>&t2);

		template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
		friend constexpr bool(operator>)(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>&t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>&t2);

		template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
		friend constexpr bool(operator<=)(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>&t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>&t2);

		template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
		friend constexpr bool(operator>=)(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>&t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>&t2);

		template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
		friend constexpr bool(operator==)(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>&t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>&t2);

		template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
		friend constexpr bool(operator!=)(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>&t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>&t2);
		
		friend class std::numeric_limits<sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>>;
	public:
		constexpr GregorianTime()
			: m_secondFraction(0.0), m_secsAfterPosixEpoch(0), m_cTime(gmtime(0))
		{
		}
		template<std::time_t OTHER_BASE_OFFSET, std::time_t ...OTHER_LEAP_SECONDS>
		constexpr GregorianTime(const GregorianTime<OTHER_BASE_OFFSET, OTHER_LEAP_SECONDS...>& other)
			: m_secondFraction(other.m_secondFraction), m_secsAfterPosixEpoch(other.m_secsAfterPosixEpoch), m_cTime(gmtime(other.m_secsAfterPosixEpoch))
		{
		}
		constexpr GregorianTime(int year, unsigned int month, unsigned int dayOfMonth, unsigned int hour, unsigned int minute, double second)
		{
			set(year, month, dayOfMonth, hour, minute, second);
		}

		constexpr GregorianTime(std::tm time, double secondFraction)
		{
			set(time, secondFraction);
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
		//parse a date/time the date must be in the order year, month, day of month, followed by the time as hours, minutes seconds.
		//Each part of the date must be separated by the dateSeparator. Each part of the time must be separated by the timeSeparator.
		//The date and time must be separated by the dateTimeSeparator. Two digit years are not permitted, but leading zeros on any
		//value can be omitted. Any text following the second is ignored as per the std::stod function
		//seconds without checking for a valid decimal number.
		void set(const std::string& dateTimeString, std::string dateSeparator = "-", std::string timeSeparator = ":", std::string dateTimeSeparator = "T")
		{
			std::string part;
			size_t partBegin = 0;

			size_t partEnd = dateTimeString.find(dateSeparator, partBegin);
			int year = std::atoi(dateTimeString.substr(partBegin, partEnd - partBegin).c_str());
			partBegin = partEnd + dateSeparator.length();

			partEnd = dateTimeString.find(dateSeparator, partBegin);
			int month = std::atoi(dateTimeString.substr(partBegin, partEnd - partBegin).c_str());
			partBegin = partEnd + dateSeparator.length();

			partEnd = dateTimeString.find(dateTimeSeparator, partBegin);
			int day = std::atoi(dateTimeString.substr(partBegin, partEnd - partBegin).c_str());
			partBegin = partEnd + dateTimeSeparator.length();

			partEnd = dateTimeString.find(timeSeparator, partBegin);
			int hour = std::atoi(dateTimeString.substr(partBegin, partEnd - partBegin).c_str());
			partBegin = partEnd + timeSeparator.length();

			partEnd = dateTimeString.find(timeSeparator, partBegin);
			int minute = std::atoi(dateTimeString.substr(partBegin, partEnd - partBegin).c_str());
			partBegin = partEnd + timeSeparator.length();

			double second = std::stod(dateTimeString.substr(partBegin));

			set(year, month, day, hour, minute, second);
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
		static GregorianTime<BASE_OFFSET, LEAP_SECONDS...> now()
		{
			auto time = std::chrono::system_clock::now();
			decltype(time) epoch = std::chrono::system_clock::from_time_t(0); //system_clock does not include leap seconds
			typename std::chrono::template duration<float> secondsSinceEpoch = time - epoch;
			GregorianTime<BASE_OFFSET, LEAP_SECONDS...> result = GregorianTime(1970, 1, 1, 0, 0, 0.0) + sci::TimeInterval((double)secondsSinceEpoch.count());
			return result;
		}
		constexpr static GregorianTime<BASE_OFFSET, LEAP_SECONDS...> getPosixEpoch()
		{
			return GregorianTime<BASE_OFFSET, LEAP_SECONDS...>();
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
		std::time_t m_secsAfterPosixEpoch; //this is actual whole seconds (including leap seconds) since 00:00:00 1st Jan 1970
		double m_secondFraction;
		//time is seconds after 00:00:00 1st Jan 1970 UTC
		//note that secondFraction must always be a positive value in the range 0 <= secondFraction < 1, this is not checked
		//this is the case even if secondInteger is negative, i.e. - 3.8 seconds would be represented by -4, 0.2
		constexpr GregorianTime(std::time_t secondInteger, double secondFraction)
			: m_secondFraction(secondFraction), m_secsAfterPosixEpoch(secondInteger), m_cTime(gmtime(secondInteger))
		{
		}
		constexpr void set(std::tm time, double secondFraction)
		{
			m_cTime = time;
			m_secondFraction = secondFraction;
			m_secsAfterPosixEpoch = mkgmtime(m_cTime);
		}

		//The input is the representation of time as per this time type.
		//The output will be the number of seconds since 00:00:00 1st Jan 1970 UTC. 
		//The number of seconds includes any leap seconds. If BASE_OFFSET is 0 and
		//there are no leap seconds then the output is equivalent to posix time.
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
			if (time.tm_year < 0 && time.tm_year % 400 != 0)
				repeatingPeriods -= 1;
			int partYears = (time.tm_year - repeatingPeriods * 400) % 400;


			std::time_t result = (repeatingPeriods * internal::daysIn400Years + internal::cumulativeDays[partYears] - internal::cumulativeDays[70] + time.tm_yday) * internal::secondsPerDay; //this is all the whole days
			result += time.tm_hour * internal::secondsPerHour;
			result += time.tm_min * 60;
			result += time.tm_sec;

			sci::internal::LeapSecondCorrector<LEAP_SECONDS...>::addLeapSeconds(result, time.tm_sec==60);
			result -= BASE_OFFSET;

			return result;
		}

		//pass in the actual number of seconds since 00:00:00 1st Jan 1970 UTC. The number of
		//seconds includes any leap seconds. If BASE_OFFSET is 0 and there are no leap seconds then
		//the input is equivalent to posix time.
		//The output will be the representation of time as per this time type.
		static constexpr std::tm gmtime(std::time_t time)
		{
			//check if we are on a leap second - if so, subtract 1 from the time
			//then increment the seconds from 59 to 60 at the end
			bool isLeapSecond = sci::internal::LeapSecondCorrector<LEAP_SECONDS...>::isLeapSecond(time);
			if (isLeapSecond)
				--time;
			time += BASE_OFFSET;
			sci::internal::LeapSecondCorrector<LEAP_SECONDS...>::subtractLeapSeconds(time);

			std::tm timeStruct{};

			//reference to 1900 rather than 1970
			time += internal::cumulativeDays[70] * internal::secondsPerDay;

			const std::time_t secondsIn400Years = (internal::daysIn400Years * internal::secondsPerDay);

			//now reference to a 400 year multiple before the date in question
			std::time_t reference400YearChunk = time / secondsIn400Years;
			if (time < 0 && time % secondsIn400Years != 0)
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
			bool isLeapYear = GregorianTime::isLeapYear(timeStruct.tm_year + 1900);
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

			if (isLeapSecond)
				++timeStruct.tm_sec;
			return timeStruct;
		}
	};


	template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
	constexpr sci::TimeInterval operator-(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>& t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>& t2)
	{
		std::time_t integer = t1.m_secsAfterPosixEpoch - t2.m_secsAfterPosixEpoch;
		double fraction = t1.m_secondFraction - t2.m_secondFraction;
		return TimeInterval(double(integer) + fraction);
	}

	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	constexpr sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>(operator-)(const sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>& time, const sci::TimeInterval& interval)
	{
		time_t integerSeconds = time.m_secsAfterPosixEpoch;
		time_t integerInterval = (time_t)interval.value<sci::Second<>>();
		if (integerInterval < 0)
			--integerInterval; //this makes it equivalent to floor;
		integerSeconds -= integerInterval;
		double fraction = time.m_secondFraction - (interval.value<sci::Second<>>() - (double)integerInterval);
		if (fraction < 0.0)
		{
			--integerSeconds;
			fraction += 1.0;
		}

		return sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>(integerSeconds, fraction);
	}

	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	constexpr sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...> operator+(const sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>& time, const sci::TimeInterval& interval)
	{
		time_t integerSeconds = time.m_secsAfterPosixEpoch;
		time_t integerInterval = (time_t)std::floor(interval.value<sci::Second<>>());
		integerSeconds += integerInterval;
		double fraction = time.m_secondFraction + (interval.value<sci::Second<>>() - (double)integerInterval);
		if (fraction >= 1.0)
		{
			++integerSeconds;
			fraction -= 1.0;
		}

		return sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>(integerSeconds, fraction);
	}

	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	constexpr sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...> operator+(const sci::TimeInterval& interval, const sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>& time)
	{
		return time + interval;
	}

	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	constexpr sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>& operator-=(sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>& time, const sci::TimeInterval& interval)
	{
		time = time - interval;
		return time;
	}

	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	constexpr sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>& operator+=(sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>& time, const sci::TimeInterval& interval)
	{
		time = time + interval;
		return time;
	}

	template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
	constexpr bool operator<(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>& t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>& t2)
	{
		//check for nans
		if (t1.m_secondFraction != t1.m_secondFraction || t2.m_secondFraction != t2.m_secondFraction)
			return false;
		//check for infinities
		if (t1.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t1.m_secondFraction == -std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == -std::numeric_limits<double>::infinity())
			return t1.m_secondFraction < t2.m_secondFraction;

		return t1.m_secsAfterPosixEpoch < t2.m_secsAfterPosixEpoch
			|| (t1.m_secsAfterPosixEpoch == t2.m_secsAfterPosixEpoch && t1.m_secondFraction < t2.m_secondFraction);
	}

	template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
	constexpr bool operator>(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>& t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>& t2)
	{
		//check for nans
		if (t1.m_secondFraction != t1.m_secondFraction || t2.m_secondFraction != t2.m_secondFraction)
			return false;
		//check for infinities
		if (t1.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t1.m_secondFraction == -std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == -std::numeric_limits<double>::infinity())
			return t1.m_secondFraction > t2.m_secondFraction;

		return t1.m_secsAfterPosixEpoch > t2.m_secsAfterPosixEpoch
			|| (t1.m_secsAfterPosixEpoch == t2.m_secsAfterPosixEpoch && t1.m_secondFraction > t2.m_secondFraction);
	}

	template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
	constexpr bool operator<=(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>& t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>& t2)
	{
		//check for nans
		if (t1.m_secondFraction != t1.m_secondFraction || t2.m_secondFraction != t2.m_secondFraction)
			return false;
		//check for infinities
		if (t1.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t1.m_secondFraction == -std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == -std::numeric_limits<double>::infinity())
			return t1.m_secondFraction <= t2.m_secondFraction;

		return t1.m_secsAfterPosixEpoch < t2.m_secsAfterPosixEpoch
			|| (t1.m_secsAfterPosixEpoch == t2.m_secsAfterPosixEpoch && t1.m_secondFraction <= t2.m_secondFraction);
	}

	template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
	constexpr bool operator>=(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>& t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>& t2)
	{
		//check for nans
		if (t1.m_secondFraction != t1.m_secondFraction || t2.m_secondFraction != t2.m_secondFraction)
			return false;
		//check for infinities
		if (t1.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t1.m_secondFraction == -std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == -std::numeric_limits<double>::infinity())
			return t1.m_secondFraction >= t2.m_secondFraction;

		return t1.m_secsAfterPosixEpoch > t2.m_secsAfterPosixEpoch
			|| (t1.m_secsAfterPosixEpoch == t2.m_secsAfterPosixEpoch && t1.m_secondFraction >= t2.m_secondFraction);
	}

	template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
	constexpr bool operator==(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>& t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>& t2)
	{
		//check for infinities
		if (t1.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t1.m_secondFraction == -std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == -std::numeric_limits<double>::infinity())
			return t1.m_secondFraction == t2.m_secondFraction;
		return t1.m_secsAfterPosixEpoch == t2.m_secsAfterPosixEpoch && t1.m_secondFraction == t2.m_secondFraction;
	}

	template<std::time_t BASE_OFFSET1, std::time_t ...LEAP_SECONDS1, std::time_t BASE_OFFSET2, std::time_t ...LEAP_SECONDS2>
	constexpr bool operator!=(const sci::GregorianTime<BASE_OFFSET1, LEAP_SECONDS1...>& t1, const sci::GregorianTime<BASE_OFFSET2, LEAP_SECONDS2...>& t2)
	{
		//check for infinities
		if (t1.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t1.m_secondFraction == -std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == std::numeric_limits<double>::infinity()
			|| t2.m_secondFraction == -std::numeric_limits<double>::infinity())
			return t1.m_secondFraction != t2.m_secondFraction;
		return t1.m_secsAfterPosixEpoch != t2.m_secsAfterPosixEpoch || t1.m_secondFraction != t2.m_secondFraction;
	}

	typedef GregorianTime<0, 78796800, 94694401, 126230402, 157766403, 189302404, 220924805, 252460806, 283996807, 315532808, 362793609, 394329610, 425865611, 489024012, 567993613, 631152014, 662688015, 709948816, 741484817, 773020818, 820454419, 867715220, 915148821, 1136073622, 1230768023, 1341100824, 1435708825, 1483228826> UtcTime;
	typedef GregorianTime<-9> GpsTime;
	typedef GregorianTime<10> TaiTime;
	typedef GregorianTime<0> LoranTime;
}


//include this between min or max and the opening parenthesis
//and the macro expansion of this define will avoid macro
//expansion of windows.h min and max
#define AVOIDWINDOWSMINMAX

namespace std
{
	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	class numeric_limits<sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>> : public numeric_limits<sci::TimeInterval>
	{
	public:
		static constexpr sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>(min)() noexcept
		{
			return sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>(std::numeric_limits<std::time_t>::min AVOIDWINDOWSMINMAX (), 0.0);
		}

		static constexpr sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...> (max)() noexcept
		{
			return sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>(std::numeric_limits<std::time_t>::max AVOIDWINDOWSMINMAX (), 1.0 - std::numeric_limits<double>::epsilon());
		}

		static constexpr sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...> lowest() noexcept
		{
			return std::numeric_limits<sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>>::min AVOIDWINDOWSMINMAX ();
		}

		static constexpr sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...> infinity() noexcept
		{
			return sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>()+std::numeric_limits<sci::TimeInterval>::infinity();
		}

		static constexpr sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...> quiet_NaN() noexcept
		{
			return sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>() + std::numeric_limits<sci::TimeInterval>::quiet_NaN();
		}

		static constexpr sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...> signaling_NaN() noexcept
		{
			return sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>() + std::numeric_limits<sci::TimeInterval>::signaling_NaN();
		}
	};

	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	class numeric_limits<const sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>> : public numeric_limits<sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>>
	{
	};
	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	class numeric_limits<volatile sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>> : public numeric_limits<sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>>
	{
	};
	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	class numeric_limits<const volatile sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>> : public numeric_limits<sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>>
	{
	};
	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	inline bool isnan(const sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>&time)
	{
		return isnan(time.getSecond());
	}
	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	inline bool isinf(const sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>& time)
	{
		return isinf(time.getSecond());
	}
	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	inline bool isfinite(const sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>& time)
	{
		return isfinite(time.getSecond());
	}
	template<std::time_t BASE_OFFSET, std::time_t ...LEAP_SECONDS>
	inline bool isnormal(const sci::GregorianTime<BASE_OFFSET, LEAP_SECONDS...>& time)
	{
		return isnormal(time.getSecond());
	}
}

#undef AVOIDWINDOWSMINMAX
#endif