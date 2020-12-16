#include <iostream>
#include"../include/svector/Units.h"
#include<assert.h>

typedef sci::Physical<sci::Metre<>, double> dMetre;
typedef sci::Physical<sci::Second<>, double> dSecond;
typedef sci::Physical<sci::Metre<2>, double> dMetreSquared;
typedef sci::Physical<sci::Second<2>, double> dSecondSquared;
typedef sci::Physical<sci::Metre<1,6>, double> dMegaMetre;
typedef sci::Physical<sci::Second<1,-9>, double> dnanoSecond;
//typedef sci::Physical<sci::Degree<>, double> dDegree;
//typedef sci::Physical<sci::Degree<1, -6>, double> dmicroDegree;
typedef sci::Physical<sci::Unitless, double> dUnitless;
typedef sci::Physical<sci::Newton<>, double> dNewton;
typedef sci::Physical < sci::MultipliedUnit<sci::Newton<>, sci::Metre<>>, double> dnewtonMetre;

int main()
{
	//to do
	//check long representations for scaled and SI units
	//check what unit we get when we raise to the power 0
	//check what I get with m2 s-4 all squared for long and short unit text
	//remove long/shor name variables from scaled units. Use literals instead like in metric units
	//test if I can get a fractional power and what happens if so
	//check unit string for unitless types squared
	//check power and root functions work as they should

	dMetre length1(1.0);
	dMegaMetre length2(0.1);
	dMetre length3 = length1 + length2;
	std::cout << "length3 = " << length3 << "\n";
	std::wcout << "length3 = " << length3 << "\n";
	std::cout << "length3 = 100001 m\n\n";

	std::cout << length1.getShortUnitString<std::string>() << " " << length1.getLongUnitString<std::string>() << "\n";
	std::wcout << length1.getShortUnitString<std::wstring>() << " " << length1.getLongUnitString<std::wstring>() << "\n";
	std::wcout << "m metre\n\n";

	std::cout << length2.getShortUnitString<std::string>() << " " << length2.getLongUnitString<std::string>() << "\n";
	std::wcout << length2.getShortUnitString<std::wstring>() << " " << length2.getLongUnitString<std::wstring>() << "\n";
	std::cout << "Mm megametre\n\n";

	dMetre expectedSumLengthMetre(100001.0);
	dMegaMetre expectedSumLengthMegaMetre(0.100001);
	assert(std::abs(length3.value<dMetre>() - expectedSumLengthMetre.value<dMetre>()) < 0.001);
	assert(std::abs(length3.value<dMegaMetre>() - expectedSumLengthMetre.value<dMegaMetre>()) < 0.001);
	assert(std::abs(length3.value<dMetre>() - expectedSumLengthMegaMetre.value<dMetre>()) < 0.001);
	assert(std::abs(length3.value<dMegaMetre>() - expectedSumLengthMegaMetre.value<dMegaMetre>()) < 0.001);

	//test dividing two physicals
	auto speed = dMetre(200.0) / dSecond(50.0);
	std::cout << "speed = " << speed << "\n";
	std::wcout << "speed = " << speed << "\n";
	std::cout << "speed = 4 m s-1\n\n";

	//test multiplying two physicals
	auto torque = dNewton(200.0) * dMetre(0.5);
	std::cout << "torque = " << torque << "\n";
	std::wcout << "torque = " << torque << "\n";
	std::cout << "torque = 100 N m\n\n";

	std::wcout << speed.getShortUnitString<std::wstring>() << L" " << speed.getLongUnitString<std::wstring>() << "\n";
	std::cout << speed.getShortUnitString<std::string>() << " " << speed.getLongUnitString<std::string>() << "\n";
	std::cout << "m s-1 metre per second\n\n";
	assert(std::abs(speed.value<decltype(speed)>() - 4.0) < 0.001);

	auto sqrtSpeed = sci::root<2>(speed);
	std::wcout << sqrtSpeed << L" " << sqrtSpeed.getLongUnitString<std::wstring>() << "\n";
	std::cout << sqrtSpeed << " " << sqrtSpeed.getLongUnitString<std::string>() << "\n";
	std::cout << "2 m1/2 s-1/2 metre to the power 1/2 per second to the power 1/2\n\n";

	auto acceleration = dMetre(50) / sci::pow<2>(dSecond(1));
	std::wcout << acceleration.getShortUnitString<std::wstring>() << L" " << acceleration.getLongUnitString<std::wstring>() << "\n";
	std::cout << acceleration.getShortUnitString<std::string>() << " " << acceleration.getLongUnitString<std::string>() << "\n";
	std::cout << "m s-2 metre per second squared\n\n";

	auto rateAcceleration = dMetre(50) / sci::pow<3>(dSecond(1));
	std::wcout << rateAcceleration.getShortUnitString<std::wstring>() << L" " << rateAcceleration.getLongUnitString<std::wstring>() << "\n";
	std::cout << rateAcceleration.getShortUnitString<std::string>() << " " << rateAcceleration.getLongUnitString<std::string>() << "\n";
	std::cout << "m s-3 metre per second cubed\n\n";

	auto rateRateAcceleration = dMetre(50) / sci::pow<4>(dSecond(1));
	std::wcout << rateRateAcceleration.getShortUnitString<std::wstring>() << L" " << rateRateAcceleration.getLongUnitString<std::wstring>() << "\n";
	std::cout << rateRateAcceleration.getShortUnitString<std::string>() << " " << rateRateAcceleration.getLongUnitString<std::string>() << "\n";
	std::cout << "m s-4 metre per second to the power 4\n\n";

	dSecondSquared squaredTime1 = sci::pow<2>(dnanoSecond(1e10));
	dSecondSquared squaredTime2 = dnanoSecond(1e8) * dSecond(100.0);
	auto squaredTime3 = dnanoSecond(1e8) * dnanoSecond(1e8);
	auto squaredTime4 = sci::pow<2>(dnanoSecond(1e8));  
	std::wcout << squaredTime1.getShortUnitString<std::wstring>() << L" " << squaredTime1.getLongUnitString<std::wstring>() << "\n";
	std::wcout << squaredTime2.getShortUnitString<std::wstring>() << L" " << squaredTime2.getLongUnitString<std::wstring>() << "\n";
	std::wcout << squaredTime3.getShortUnitString<std::wstring>() << L" " << squaredTime3.getLongUnitString<std::wstring>() << "\n";
	std::wcout << squaredTime4.getShortUnitString<std::wstring>() << L" " << squaredTime4.getLongUnitString<std::wstring>() << "\n";
	dnanoSecond time1 = sci::sqrt(dSecondSquared(4.0));
	std::wcout << time1.getShortUnitString<std::wstring>() << L" " << time1.getLongUnitString<std::wstring>() << "\n";
	//dDegree degree1 = dUnitless(100.0) * dmicroDegree(10.0);
	//std::wcout << degree1.getShortUnitString<std::wstring>() << L" " << degree1.getLongUnitString<std::wstring>() << "\n";
}

