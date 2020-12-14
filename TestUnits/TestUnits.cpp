#include <iostream>
#include"../include/svector/Units.h"
#include<assert.h>

typedef sci::Physical<sci::Metre<>, double> dMetre;
typedef sci::Physical<sci::Second<>, double> dSecond;
typedef sci::Physical<sci::Metre<2>, double> dMetreSquared;
typedef sci::Physical<sci::Second<2>, double> dSecondSquared;
typedef sci::Physical<sci::Metre<1,6>, double> dMegaMetre;
typedef sci::Physical<sci::Second<1,-9>, double> dnanoSecond;
typedef sci::Physical<sci::Degree<>, double> dDegree;
typedef sci::Physical<sci::Degree<1, -6>, double> dmicroDegree;
typedef sci::Physical<sci::Unitless, double> dUnitless;

int main()
{
	//to do
	//check long representations for scaled and SI units
	//check what unit we get when we raise to the power 0

	dMetre length1(1.0);
	dMegaMetre length2(0.1);
	dMetre length3 = length1 + length2;
	std::cout << "length3 = " << length3 << " should be 100001 m\n";
	std::cout << sci::nativeCodepage(length1.getShortUnitString()) << " " << sci::nativeCodepage(length1.getLongUnitString()) << "\n";
	std::cout << sci::nativeCodepage(length2.getShortUnitString()) << " " << sci::nativeCodepage(length2.getLongUnitString()) << "\n";
	dMetre expectedSumLengthMetre(100001.0);
	dMegaMetre expectedSumLengthMegaMetre(0.100001);
	assert(std::abs(length3.value<dMetre>() - expectedSumLengthMetre.value<dMetre>()) < 0.001);
	assert(std::abs(length3.value<dMegaMetre>() - expectedSumLengthMetre.value<dMegaMetre>()) < 0.001);
	assert(std::abs(length3.value<dMetre>() - expectedSumLengthMegaMetre.value<dMetre>()) < 0.001);
	assert(std::abs(length3.value<dMegaMetre>() - expectedSumLengthMegaMetre.value<dMegaMetre>()) < 0.001);

	auto speed = dMetre(200.0) / dSecond(50.0);
	std::cout << "speed = " << speed << "\n";
	std::cout << sci::nativeCodepage(speed.getShortUnitString()) << " " << sci::nativeCodepage(speed.getLongUnitString()) << "\n";
	assert(std::abs(speed.value<decltype(speed)>() - 4.0) < 0.001);

	dSecondSquared squaredTime1 = sci::pow<2>(dnanoSecond(1e10));
	dSecondSquared squaredTime2 = dnanoSecond(1e8) * dSecond(100.0);
	auto squaredTime3 = dnanoSecond(1e8) * dnanoSecond(1e8);
	auto squaredTime4 = sci::pow<2>(dnanoSecond(1e8));  
	std::cout << sci::nativeCodepage(squaredTime1.getShortUnitString()) << " " << sci::nativeCodepage(squaredTime1.getLongUnitString()) << "\n";
	std::cout << sci::nativeCodepage(squaredTime2.getShortUnitString()) << " " << sci::nativeCodepage(squaredTime2.getLongUnitString()) << "\n";
	std::cout << sci::nativeCodepage(squaredTime3.getShortUnitString()) << " " << sci::nativeCodepage(squaredTime3.getLongUnitString()) << "\n";
	std::cout << sci::nativeCodepage(squaredTime4.getShortUnitString()) << " " << sci::nativeCodepage(squaredTime4.getLongUnitString()) << "\n";
	dnanoSecond time1 = sci::sqrt(dSecondSquared(4.0));
	std::cout << sci::nativeCodepage(time1.getShortUnitString()) << " " << sci::nativeCodepage(time1.getLongUnitString()) << "\n";
	dDegree degree1 = dUnitless(100.0) * dmicroDegree(10.0);
	std::cout << sci::nativeCodepage(degree1.getShortUnitString()) << " " << sci::nativeCodepage(degree1.getLongUnitString()) << "\n";
}

