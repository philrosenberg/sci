#include <iostream>
#include <Units.h>

typedef sci::Physical<sci::Metre<>, double> metre;
typedef sci::Physical<sci::Second<>, double> second;
typedef sci::Physical<sci::Unitless, double> unitless;
typedef sci::Physical<sci::Radian<>, double> radian;
typedef decltype(radian() / second()) radianPerSecond;

int main()
{
	metre length(21.0);

	second time = length; //error1

	auto badResult = time + length; //error2

	length = 5.0; //error3
	//length = metre(5.0)

	length = length / 2.0; //error4
	//length = length / unitless(2.0);

	auto logLength = sci::log10(length); //error5
	//auto logLength = sci::log10(length / metre(1.0));

	auto waveAmplitude = sci::cos(time); //error6
	//radianPerSecond angularFrequency(2.0);
	//auto waveAmplitude = sci::cos(angularFrequency * time);

	return 0;
}
