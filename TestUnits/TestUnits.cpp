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
	dMetre length1(1.0);
	dMegaMetre length2(0.1);
	dMetre length3 = length1 + length2;
	dMetre expectedSumLengthMetre(100001.0);
	dMegaMetre expectedSumLengthMegaMetre(0.100001);
	assert(std::abs(length3.value<dMetre>() - expectedSumLengthMetre.value<dMetre>()) < 0.001);
	assert(std::abs(length3.value<dMegaMetre>() - expectedSumLengthMetre.value<dMegaMetre>()) < 0.001);
	assert(std::abs(length3.value<dMetre>() - expectedSumLengthMegaMetre.value<dMetre>()) < 0.001);
	assert(std::abs(length3.value<dMegaMetre>() - expectedSumLengthMegaMetre.value<dMegaMetre>()) < 0.001);

	auto speed = dMetre(200.0) / dSecond(50.0);
	assert(std::abs(speed.value<decltype(speed)>() - 4.0) < 0.001);

	dSecondSquared squaredTime1 = sci::pow<2>(dnanoSecond(1e10));
	dSecondSquared squaredTime2 = dnanoSecond(1e8) * dSecond(100.0);
	dnanoSecond time1 = sci::sqrt(dSecondSquared(4.0));
	dDegree degree1 = dUnitless(100.0) * dmicroDegree(10.0);
}

