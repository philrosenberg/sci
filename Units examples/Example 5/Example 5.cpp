#include <iostream>
#include <Units.h>

typedef sci::Physical<sci::Metre<>, double> metre;
typedef sci::Physical<sci::Inch<>, double> inch;
typedef sci::Physical<sci::Kelvin<>, double> kelvin;
typedef sci::Physical<sci::Rankine<>, double> rankine;
typedef sci::Physical<sci::Degree<>, double> degree;
typedef sci::Physical<sci::Unitless, double> unitless;

int main()
{
	metre lengthSi(21.0);
	inch lengthNonSi = lengthSi;

	kelvin temperatureSi(273.15);
	rankine temperatureNonSi = temperatureSi;

	degree angle(90);

	std::cout << lengthSi << " = " << lengthNonSi << "\n";
	std::cout << temperatureSi << " = " << temperatureNonSi << "\n";
	std::cout << "cos of " << angle << " = " << sci::cos(angle);
	return 0;
}
