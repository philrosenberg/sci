#include <iostream>
#include <Units.h>
#include<vector>

typedef sci::Physical<sci::Metre<1,sci::kilo>, double> kilometre;
typedef sci::Physical<sci::Mile<>, double> mile;
typedef sci::Physical<sci::Hour<>, double> hour;
typedef sci::Physical<sci::Second<>, double> second;
typedef decltype(kilometre() / hour()) kilometrePerHour;

int main()
{
	kilometre europeanTrackLength(4.3);
	mile ukTrackLength(2.8);
	std::vector<int> carNumber{ 61, 66, 17 };
	std::vector<second> ukLapTimes{ second(130.0), second(128.4), second(142.6) };
	std::vector<second> europeanLapTimes{ second(134.1), second(126.1), second(140.8) };

	std::cout << "car\tspeed-uk\tspeed-Europe\n";
	std::cout << "number\t" << kilometrePerHour::getShortUnitString<std::string>();
	std::cout << "\t\t" << kilometrePerHour::getShortUnitString<std::string>() << "\n";
	for (size_t i = 0; i < 3; ++i)
	{
		std::cout << carNumber[i] << "\t";
		std::cout << (ukTrackLength / ukLapTimes[i]).value<kilometrePerHour>() << "\t\t";
		std::cout << (europeanTrackLength / europeanLapTimes[i]).value<kilometrePerHour>() << "\n";
	}
	
	return 0;
}
