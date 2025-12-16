#include <iostream>
#include"../include/scieng/Units.h"
#include"../include/scieng/Time.h"
#include<assert.h>

typedef sci::Physical<sci::Metre<>, double> dMetre;
typedef sci::Physical<sci::Second<>, double> dSecond;
typedef sci::Physical<sci::Metre<1, -2>, double> dCentimetre;
typedef sci::Physical<sci::Metre<2, -2>, double> dCentimetreSquared;
typedef sci::Physical<sci::Second<2>, double> dSecondSquared;
typedef sci::Physical<sci::Metre<1, 6>, double> dMegaMetre;
typedef sci::Physical< sci::PoweredUnit<sci::Metre<>,2>, double> dMetreSquared;
typedef sci::Physical<sci::PoweredUnit<sci::Metre<1, 6>, 2>, double> dMegametreSquared;
typedef sci::Physical<sci::PoweredUnit<sci::Metre<1,3>,2>, double> dKilometreSquared;
typedef sci::Physical< sci::Metre<2>, double> dMetreSquaredOther;
typedef sci::Physical<sci::Metre<2, 6>, double> dMegametreSquaredOther;
typedef sci::Physical<sci::Metre<2, 3>, double> dKilometreSquaredOther;
typedef sci::Physical<sci::Second<1,-9>, double> dnanoSecond;
//typedef sci::Physical<sci::Degree<>, double> dDegree;
//typedef sci::Physical<sci::Degree<1, -6>, double> dmicroDegree;
typedef sci::Physical<sci::Unitless, double> dUnitless;
typedef sci::Physical<sci::Newton<>, double> dNewton;
typedef sci::Physical < sci::MultipliedUnit<sci::Newton<>, sci::Metre<>>, double> dnewtonMetre;
typedef sci::Physical < sci::Gram<>, double> dGram;
typedef sci::Physical < sci::Gram<1, 3>, double> dKilogram;
typedef sci::Physical < sci::DividedUnit<sci::Gram<>, sci::Gram<1, 3>>, double> dGramPerKilogram;
typedef sci::Physical<sci::Inch<>, double> dInch;

#include<array>
template<int ...INTS>
class SomeInts
{
};

template< int INT, int ...INTS>
class SomeInts<INT, INTS...> : SomeInts<INTS...>
{
public:
	SomeInts()
		:SomeInts<INTS...>(), thisInt(INT)
	{}
private:
	const int thisInt;
};

constexpr float sumsci()
{
	float sum = 0.0;
	for (size_t i = 0; i < 10000000; ++i)
		sum += sci::unitsPrivate::sqrt((float)i);
	return sum;
}
float sumstd()
{
	float sum = 0.0;
	for (size_t i = 0; i < 10000000; ++i)
		sum += std::sqrt((float)i);
	return sum;
}

int main()
{
	static_assert(std::is_standard_layout_v<dMetre> && std::is_trivial_v<dMetre>, "sci::Physicals should be pod");


	static_assert(sci::is_physical_v<dMetre>); //dMetre is a physical
	static_assert(!sci::is_physical_v<double>); //double is not a physical
	static_assert(!sci::is_unit<dMetre>::value); //dMetre is not a unit
	static_assert(sci::is_unit<sci::Metre<>>::value); //sci::Metre is a unit
	constexpr auto shouldBeUnitlessPhysical = dMetre(4) / dMetre(2);
	static_assert(shouldBeUnitlessPhysical == dUnitless(2)); // 4 metres / 2 metres = 2 
	static_assert(sci::is_unitless_physical_v<decltype(shouldBeUnitlessPhysical)>); // metre/metre is a unitless
	static_assert(sci::is_unitless_physical_v<decltype(dMetre(5) / dMetre(2))>); //metre/ metre is a unitless

	sci::pow(shouldBeUnitlessPhysical, shouldBeUnitlessPhysical);//a unitless to the power of unitless compiles




	static_assert(sci::IsValidSiExponent<0>); //0 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<1>); //1 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<-1>); //-1 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<2>); //2 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<-2>); //-2 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<3>); //3 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<-3>); //-3 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<6>); //6 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<-6>); //-6 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<9>); //9 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<-9>); //-9 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<12>); //12 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<-12>); //-12 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<15>); //15 is a valid SI exponent
	static_assert(sci::IsValidSiExponent<-15>); //-15 is a valid SI exponent
	static_assert(!sci::IsValidSiExponent<4>); //4 is not a valid SI exponent

	std::string milliString = sci::getExponentName<sci::milli, std::string>();
	//std::string invalidString = sci::ExponentTraits<4>::getName<std::string>(); //should generate error
	//int invalidString2 = sci::ExponentTraits<-3>::getName<int>(); //should generate error


	//Check conversion between units with powers and divides
	using shouldBeMegaMetre = decltype(sci::pow<2>(dMegaMetre()) / dMegaMetre());
	static_assert(dMegaMetre(1.0).value<shouldBeMegaMetre>() == 1.0);
	static_assert((dMegaMetre(1.0)*dMegaMetre(1.0)).value<decltype(sci::pow<2>(dMegaMetre()))>() == 1.0);
	static_assert(dMegaMetre::unit::exponentNumerator == 6);
	static_assert(shouldBeMegaMetre::unit::exponentNumerator == 6);

	//check conversion between scaled units
	dInch inch(1);
	dCentimetre anInchInCm = inch;
	dCentimetreSquared aSquareInchInSquareCm = sci::pow<2>(inch);
	auto shouldBeOneAgain = (inch / inch);
	shouldBeOneAgain.value<dUnitless>();


	using MMConverter = typename dMegaMetre::unit::template Converter<double>;
	auto shouldBeOne = MMConverter::template convertTo<typename shouldBeMegaMetre::unit>(1.0);


	dGram weight1(1);
	dKilogram weight2 = weight1;
	dKilogram weight3(2);
	dGram weight4 = weight3;
	std::wcout << weight1 << " = " << weight2 << "\n";
	std::wcout << weight3 << " = " << weight4 << "\n";
	dUnitless unitlessValue(0.001);
	dGramPerKilogram gramPerKilogramValue = unitlessValue;
	std::wcout << unitlessValue << L" = " << gramPerKilogramValue << "\n";

	dMetre negativeDistance(-5);
	dMetre positiveDistance(5);
	dMetre abs_negativeDistance = sci::abs(negativeDistance);
	dMetre abs_positiveDistance = sci::abs(positiveDistance);

	dMetreSquared bigArea(2e12);
	dMegametreSquared bigArea2 = bigArea; //test conversion of a powered unit from base type to type with exponent
	dMetreSquared bigArea3 = bigArea2; //test conversion of a powered unit from a type with an exponent to a base type
	dKilometreSquared bigArea4 = bigArea2; //test conversion of a powered unit from a type with an exponent to a type with an exponent
	std::wcout << bigArea << " = " << bigArea2 << " = " << bigArea3 << " = " << bigArea4 << "\n";

	dMetreSquared bigAreaOther(2e12);
	dMegametreSquared bigAreaOther2 = bigAreaOther; //test conversion of a unit with a power parameter from base type to type with exponent
	dMetreSquared bigAreaOther3 = bigAreaOther2; //test conversion of a unit with a power parameter from a type with an exponent to a base type
	dKilometreSquared bigAreaOther4 = bigAreaOther2; //test conversion of a unit with a power parameter from a type with an exponent to a type with an exponent
	std::wcout << bigAreaOther << " = " << bigAreaOther2 << " = " << bigAreaOther3 << " = " << bigAreaOther4 << "\n";

	std::wcout << "2e+12 m2 = 2 Mm2 = 2e+12 m2 = 2e+06 km2\n";
	std::wcout << "Above 3 lines must match";



	int seed;
	std::cout << "double epsilon " << std::numeric_limits<double>::epsilon() << "\n";
	std::cout << "float epsilon " << std::numeric_limits<float>::epsilon() << "\n";
	std::cout << "sqrt 10 accuracy subtract d " << sci::unitsPrivate::sqrt(10.0) - std::sqrt(10.0) << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "sqrt 10 accuracy subtract f " << sci::unitsPrivate::sqrt(10.0f) - std::sqrt(10.0f) << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "sqrt 10 accuracy ratio d " << sci::unitsPrivate::sqrt(10.0) / std::sqrt(10.0) - 1.0 << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "sqrt 10 accuracy ratio f " << sci::unitsPrivate::sqrt(10.0f) / std::sqrt(10.0f) - 1.0f << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "sqrt 1000 accuracy ratio d " << sci::unitsPrivate::sqrt(1000.0) / std::sqrt(1000.0) - 1.0 << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "sqrt 1000 accuracy ratio f " << sci::unitsPrivate::sqrt(1000.0f) / std::sqrt(1000.0f) - 1.0f << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "sqrt 100000 accuracy ratio d " << sci::unitsPrivate::sqrt(100000.0) / std::sqrt(100000.0) - 1.0 << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "sqrt 100000 accuracy ratio f " << sci::unitsPrivate::sqrt(100000.0f) / std::sqrt(100000.0f) - 1.0f << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "sqrt 0.1 accuracy ratio d " << sci::unitsPrivate::sqrt(0.1) / std::sqrt(0.1) - 1.0 << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "sqrt 0.1 accuracy ratio f " << sci::unitsPrivate::sqrt(0.1f) / std::sqrt(0.1f) - 1.0f << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	
	std::cout << "root3 10 accuracy ratio d " << sci::unitsPrivate::root(10.0, 3) / std::pow(10.0, 1.0 / 3.0) - 1.0 << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "root3 10 accuracy ratio f " << sci::unitsPrivate::root(10.0f, 3) / std::pow(10.0f, 1.0f / 3.0f) - 1.0f << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "root3 1000 accuracy ratio d " << sci::unitsPrivate::root(1000.0, 3) / std::pow(1000.0, 1.0 / 3.0) - 1.0 << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "root3 1000 accuracy ratio f " << sci::unitsPrivate::root(1000.0f, 3) / std::pow(1000.0f, 1.0f / 3.0f) - 1.0f << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "root3 100000 accuracy ratio d " << sci::unitsPrivate::root(100000.0, 3) / std::pow(100000.0, 1.0 / 3.0) - 1.0 << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "root3 100000 accuracy ratio f " << sci::unitsPrivate::root(100000.0f, 3) / std::pow(100000.0f, 1.0f / 3.0f) - 1.0f << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "root3 0.1 accuracy ratio d " << sci::unitsPrivate::root(0.1, 3) / std::pow(0.1, 1.0 / 3.0) - 1.0 << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess
	std::cout << "root3 0.1 accuracy ratio f " << sci::unitsPrivate::root(0.1f, 3) / std::pow(0.1f, 1.0f / 3.0f) - 1.0f << "\n"; // 1 and 10 (or 10 * 100^n) give the worst case scenario accuracy for our initial guess

																								   //return 0;
	std::cout << "Enter seed\n";
	std::cin >> seed;
	srand(seed);
	size_t n = 10000000;
	float max = 1e6;

	float sum = 0;
	auto start = clock();
	for (size_t i = 0; i < n; ++i)
		sum += (float)rand() / (float)RAND_MAX * max;
	auto end = clock();
	std::cout << "Sum " << sum << " duration " << end - start << std::endl;

	sum = 0;
	start = clock();
	for (size_t i = 0; i < n; ++i)
		sum += (float)rand() / (float)RAND_MAX * max;
	end = clock();
	std::cout << "Sum " << sum << " duration " << end - start << std::endl;

	sum = 0;
	start = clock();
	for (size_t i = 0; i < n; ++i)
		sum += sci::unitsPrivate::sqrt((float)rand() / (float)RAND_MAX * max);
	end = clock();
	std::cout << "Sum sci::sqrt " << sum << " duration " << end - start << std::endl;

	sum = 0;
	start = clock();
	for (size_t i = 0; i < n; ++i)
		sum += std::sqrt((float)rand() / (float)RAND_MAX * max);
	end = clock();
	std::cout << "Sum std::sqrt " << sum << " duration " << end - start << std::endl;

	//sum = 0;
	//start = clock();
	//if constexpr (sumsci() > 100000.0)
	//{
	//	end = clock();
	//	std::cout << "Sum sci::sqrtfunc first duration " << end - start << std::endl;
	//}
	//else
	//{
	//	end = clock();
	//	std::cout << "Sum sci::sqrtfunc second duration " << end - start << std::endl;
	//}
	//
	//sum = 0;
	//start = clock();
	//for (size_t i = 0; i < n; ++i)
	//	sum += std::sqrt((float)i / (float)RAND_MAX * max);
	//end = clock();
	//std::cout << "Sum std::sqrt func " << sum << " duration " << end - start << std::endl;

	sum = 0;
	start = clock();
	for (size_t i = 0; i < n; ++i)
		sum += sci::unitsPrivate::root((float)rand() / (float)RAND_MAX * max, 3);
	end = clock();
	std::cout << "Sum sci::root 3 " << sum << " duration " << end - start << std::endl;

	sum = 0;
	start = clock();
	for (size_t i = 0; i < n; ++i)
		sum += std::pow((float)rand() / (float)RAND_MAX * max, float(1.0/3.0));
	end = clock();
	std::cout << "Sum std::pow 1/3 " << sum << " duration " << end - start << std::endl;
	sum = 0;
	start = clock();
	for (size_t i = 0; i < n; ++i)
		sum += sci::unitsPrivate::pow((float)rand() / (float)RAND_MAX * max,3);
	end = clock();
	std::cout << "Sum sci::pow 3 " << sum << " duration " << end - start << std::endl;

	sum = 0;
	start = clock();
	for (size_t i = 0; i < n; ++i)
		sum += std::pow((float)rand() / (float)RAND_MAX * max, float(3.0));
	end = clock();
	std::cout << "Sum std::pow 3 " << sum << " duration " << end - start << std::endl;


	//to do
	//check long representations for scaled and SI units
	//check what unit we get when we raise to the power 0
	//check what I get with m2 s-4 all squared for long and short unit text
	//remove long/shor name variables from scaled units. Use literals instead like in metric units
	//test if I can get a fractional power and what happens if so
	//check unit string for unitless types squared
	//check power and root functions work as they should
	//check 20 cm *20 cm = 0.04 m^2
	//check the constexpr functions can actually be determined at compile time by using them in an if constexpr or template parameter

	dMetre length1(1.0);
	dMegaMetre length2(0.1);
	length1 + length2;
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

	std::cout << "power test - length2^2/length2-length2 =" << sci::pow<2>(length2) / length2 - length2 << "\n";
	std::cout << "The above should be zero\n\n";

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

	auto cuberootSpeed = sci::root<3>(dMetre(27)/dSecond(1));
	std::wcout << cuberootSpeed << L" " << cuberootSpeed.getLongUnitString<std::wstring>() << "\n";
	std::cout << cuberootSpeed << " " << cuberootSpeed.getLongUnitString<std::string>() << "\n";
	std::cout << "3 m1/3 s-1/3 metre to the power 1/3 per second to the power 1/3\n\n";
	
	std::wcout << dMetreSquared(dCentimetre(20) * dCentimetre(20)) << "\n";
	std::wcout << dMetreSquared(sci::pow<2>(dCentimetre(20))) << "\n";
	std::cout << "0.04 m2\n\n";

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
	

	//Time


	sci::UtcTime beforeLeap[]{
		sci::UtcTime(1972, 6, 30, 23, 59, 59),
		sci::UtcTime(1972, 12, 31, 23, 59, 59),
		sci::UtcTime(1973, 12, 31, 23, 59, 59),
		sci::UtcTime(1974, 12, 31, 23, 59, 59),
		sci::UtcTime(1975, 12, 31, 23, 59, 59),
		sci::UtcTime(1976, 12, 31, 23, 59, 59),
		sci::UtcTime(1977, 12, 31, 23, 59, 59),
		sci::UtcTime(1978, 12, 31, 23, 59, 59),
		sci::UtcTime(1979, 12, 31, 23, 59, 59),
		sci::UtcTime(1981, 6, 30, 23, 59, 59),
		sci::UtcTime(1982, 6, 30, 23, 59, 59),
		sci::UtcTime(1983, 6, 30, 23, 59, 59),
		sci::UtcTime(1985, 6, 30, 23, 59, 59),
		sci::UtcTime(1987, 12, 31, 23, 59, 59),
		sci::UtcTime(1989, 12, 31, 23, 59, 59),
		sci::UtcTime(1990, 12, 31, 23, 59, 59),
		sci::UtcTime(1992, 6, 30, 23, 59, 59),
		sci::UtcTime(1993, 6, 30, 23, 59, 59),
		sci::UtcTime(1994, 6, 30, 23, 59, 59),
		sci::UtcTime(1995, 12, 31, 23, 59, 59),
		sci::UtcTime(1997, 6, 30, 23, 59, 59),
		sci::UtcTime(1998, 12, 31, 23, 59, 59),
		sci::UtcTime(2005, 12, 31, 23, 59, 59),
		sci::UtcTime(2008, 12, 31, 23, 59, 59),
		sci::UtcTime(2012, 6, 30, 23, 59, 59),
		sci::UtcTime(2015, 6, 30, 23, 59, 59),
		sci::UtcTime(2016, 12, 31, 23, 59, 59)
	};
	sci::UtcTime onLeap[]{
		sci::UtcTime(1972, 6, 30, 23, 59, 60),
		sci::UtcTime(1972, 12, 31, 23, 59, 60),
		sci::UtcTime(1973, 12, 31, 23, 59, 60),
		sci::UtcTime(1974, 12, 31, 23, 59, 60),
		sci::UtcTime(1975, 12, 31, 23, 59, 60),
		sci::UtcTime(1976, 12, 31, 23, 59, 60),
		sci::UtcTime(1977, 12, 31, 23, 59, 60),
		sci::UtcTime(1978, 12, 31, 23, 59, 60),
		sci::UtcTime(1979, 12, 31, 23, 59, 60),
		sci::UtcTime(1981, 6, 30, 23, 59, 60),
		sci::UtcTime(1982, 6, 30, 23, 59, 60),
		sci::UtcTime(1983, 6, 30, 23, 59, 60),
		sci::UtcTime(1985, 6, 30, 23, 59, 60),
		sci::UtcTime(1987, 12, 31, 23, 59, 60),
		sci::UtcTime(1989, 12, 31, 23, 59, 60),
		sci::UtcTime(1990, 12, 31, 23, 59, 60),
		sci::UtcTime(1992, 6, 30, 23, 59, 60),
		sci::UtcTime(1993, 6, 30, 23, 59, 60),
		sci::UtcTime(1994, 6, 30, 23, 59, 60),
		sci::UtcTime(1995, 12, 31, 23, 59, 60),
		sci::UtcTime(1997, 6, 30, 23, 59, 60),
		sci::UtcTime(1998, 12, 31, 23, 59, 60),
		sci::UtcTime(2005, 12, 31, 23, 59, 60),
		sci::UtcTime(2008, 12, 31, 23, 59, 60),
		sci::UtcTime(2012, 6, 30, 23, 59, 60),
		sci::UtcTime(2015, 6, 30, 23, 59, 60),
		sci::UtcTime(2016, 12, 31, 23, 59, 60)
	};
	sci::UtcTime afterLeap[]{
		sci::UtcTime(1972, 7, 1, 0, 0, 0),
		sci::UtcTime(1973, 1, 1, 0, 0, 0),
		sci::UtcTime(1974, 1, 1, 0, 0, 0),
		sci::UtcTime(1975, 1, 1, 0, 0, 0),
		sci::UtcTime(1976, 1, 1, 0, 0, 0),
		sci::UtcTime(1977, 1, 1, 0, 0, 0),
		sci::UtcTime(1978, 1, 1, 0, 0, 0),
		sci::UtcTime(1979, 1, 1, 0, 0, 0),
		sci::UtcTime(1980, 1, 1, 0, 0, 0),
		sci::UtcTime(1981, 7, 1, 0, 0, 0),
		sci::UtcTime(1982, 7, 1, 0, 0, 0),
		sci::UtcTime(1983, 7, 1, 0, 0, 0),
		sci::UtcTime(1985, 7, 1, 0, 0, 0),
		sci::UtcTime(1988, 1, 1, 0, 0, 0),
		sci::UtcTime(1990, 1, 1, 0, 0, 0),
		sci::UtcTime(1991, 1, 1, 0, 0, 0),
		sci::UtcTime(1992, 7, 1, 0, 0, 0),
		sci::UtcTime(1993, 7, 1, 0, 0, 0),
		sci::UtcTime(1994, 7, 1, 0, 0, 0),
		sci::UtcTime(1996, 1, 1, 0, 0, 0),
		sci::UtcTime(1997, 7, 1, 0, 0, 0),
		sci::UtcTime(1999, 1, 1, 0, 0, 0),
		sci::UtcTime(2006, 1, 1, 0, 0, 0),
		sci::UtcTime(2009, 1, 1, 0, 0, 0),
		sci::UtcTime(2012, 7, 1, 0, 0, 0),
		sci::UtcTime(2015, 7, 1, 0, 0, 0),
		sci::UtcTime(2017, 1, 1, 0, 0, 0)
	};
	sci::UtcTime afterAfterLeap[]{
		sci::UtcTime(1972, 7, 1, 0, 0, 1),
		sci::UtcTime(1973, 1, 1, 0, 0, 1),
		sci::UtcTime(1974, 1, 1, 0, 0, 1),
		sci::UtcTime(1975, 1, 1, 0, 0, 1),
		sci::UtcTime(1976, 1, 1, 0, 0, 1),
		sci::UtcTime(1977, 1, 1, 0, 0, 1),
		sci::UtcTime(1978, 1, 1, 0, 0, 1),
		sci::UtcTime(1979, 1, 1, 0, 0, 1),
		sci::UtcTime(1980, 1, 1, 0, 0, 1),
		sci::UtcTime(1981, 7, 1, 0, 0, 1),
		sci::UtcTime(1982, 7, 1, 0, 0, 1),
		sci::UtcTime(1983, 7, 1, 0, 0, 1),
		sci::UtcTime(1985, 7, 1, 0, 0, 1),
		sci::UtcTime(1988, 1, 1, 0, 0, 1),
		sci::UtcTime(1990, 1, 1, 0, 0, 1),
		sci::UtcTime(1991, 1, 1, 0, 0, 1),
		sci::UtcTime(1992, 7, 1, 0, 0, 1),
		sci::UtcTime(1993, 7, 1, 0, 0, 1),
		sci::UtcTime(1994, 7, 1, 0, 0, 1),
		sci::UtcTime(1996, 1, 1, 0, 0, 1),
		sci::UtcTime(1997, 7, 1, 0, 0, 1),
		sci::UtcTime(1999, 1, 1, 0, 0, 1),
		sci::UtcTime(2006, 1, 1, 0, 0, 1),
		sci::UtcTime(2009, 1, 1, 0, 0, 1),
		sci::UtcTime(2012, 7, 1, 0, 0, 1),
		sci::UtcTime(2015, 7, 1, 0, 0, 1),
		sci::UtcTime(2017, 1, 1, 0, 0, 1)
	};
	for(size_t i=0; i<27; ++i)
		std::cout << onLeap[i]-onLeap[i].getPosixEpoch() << " " << onLeap[i] - beforeLeap[i] << " " << afterLeap[i] - beforeLeap[i] << " " << afterAfterLeap[i] - beforeLeap[i] << std::endl;

	//search to find all the leap seconds and display them
	for (size_t i = 0; i < 4000000000; i+=60*60*24) // searching through over 100 years
	{
		for (size_t j = 0; j < 30; ++j)
		{
			sci::UtcTime time = sci::UtcTime(1970, 1, 1, 0, 0, 0) + sci::TimeInterval(sci::TimeInterval::valueType(i+j));
			if (time.getSecond() == 60)
			{
				std::cout << time.getIso8601String() << std::endl;
			}
		}
	}

	//compare UTC, GMT and TAI
	const sci::UtcTime utcTime(2021, 6, 4, 9, 32, 41.4);
	const sci::GpsTime gpsTime(utcTime);
	const sci::TaiTime taiTime(utcTime);
	const sci::LoranTime lorTime(utcTime);
	const sci::GpsTime laterTime = taiTime + sci::TimeInterval(1.0);
	const sci::LoranTime earlierTime = taiTime - sci::TimeInterval(1.0);
	std::cout << "\n\n";
	std::cout << "UTC should be 2021-06-04T09:32:41.4Z - " << utcTime.getIso8601String(1) << utcTime - utcTime << "\n";
	std::cout << "GPS should be 2021-06-04T09:32:59.4Z - " << gpsTime.getIso8601String(1) << gpsTime - utcTime << "\n";
	std::cout << "LOR should be 2021-06-04T09:33:08.4Z - " << lorTime.getIso8601String(1) << lorTime - utcTime << "\n";
	std::cout << "TAI should be 2021-06-04T09:33:18.4Z - " << taiTime.getIso8601String(1) << taiTime - utcTime << "\n";

	if (gpsTime != utcTime)
		std::cout << "problem in not equal\n";
	if (!(gpsTime == utcTime))
		std::cout << "problem in equal\n";
	if (!(gpsTime < laterTime))
		std::cout << "problem in less than\n";
	if (gpsTime < utcTime)
		std::cout << "problem in less than\n";
	if (!(gpsTime > earlierTime))
		std::cout << "problem in greater than\n";
	if (gpsTime > utcTime)
		std::cout << "problem in greater than\n";
	if (!(gpsTime <= laterTime))
		std::cout << "problem in less than or equal\n";
	if (!(gpsTime <= utcTime))
		std::cout << "problem in less than or equal\n";
	if (!(gpsTime >= earlierTime))
		std::cout << "problem in greater than or equal\n";
	if (!(gpsTime >= utcTime))
		std::cout << "problem in greater than or equal\n";
	sci::TaiTime negInf = taiTime - std::numeric_limits<sci::TimeInterval>::infinity();
	sci::TaiTime inf = taiTime + std::numeric_limits<sci::TimeInterval>::infinity();
	if (gpsTime > std::numeric_limits<sci::TaiTime>::infinity())
		std::cout << "problem with gt inf\n";
	if (gpsTime < negInf)
		std::cout << "problem with lt inf\n";
	if (!(gpsTime < std::numeric_limits<sci::TaiTime>::infinity()))
		std::cout << "problem with lt inf\n";
	if (!(gpsTime > negInf))
		std::cout << "problem with gt inf\n";
	if (gpsTime >= std::numeric_limits<sci::TaiTime>::infinity())
		std::cout << "problem with gteq inf\n";
	if (gpsTime <= negInf)
		std::cout << "problem with lteq inf\n";
	if (!(gpsTime <= std::numeric_limits<sci::TaiTime>::infinity()))
		std::cout << "problem with lteq inf\n";
	if (!(gpsTime >= negInf))
		std::cout << "problem with gteq inf\n";
	if (!(inf <= std::numeric_limits<sci::TaiTime>::infinity()))
		std::cout << "problem with lteq inf\n";
	if (!(negInf >= negInf))
		std::cout << "problem with gteq inf\n";
	if (inf != std::numeric_limits<sci::TaiTime>::infinity())
		std::cout << "problem with noteq inf\n";
	if (!(inf == std::numeric_limits<sci::TaiTime>::infinity()))
		std::cout << "problem with eq inf\n";
	if (utcTime == inf)
		std::cout << "problem with eq inf\n";
	if (!(utcTime != inf))
		std::cout << "problem with noteq inf\n";
	if (utcTime == std::numeric_limits<sci::UtcTime>::quiet_NaN())
		std::cout << "problem with eq nan\n";
	if (!(utcTime != std::numeric_limits<sci::UtcTime>::quiet_NaN()))
		std::cout << "problem with noteq nan\n";
	if (gpsTime < std::numeric_limits<sci::TaiTime>::quiet_NaN())
		std::cout << "problem with lt nan\n";
	if (gpsTime > std::numeric_limits<sci::TaiTime>::quiet_NaN())
		std::cout << "problem with gt nan\n";
	if (gpsTime >= std::numeric_limits<sci::TaiTime>::quiet_NaN())
		std::cout << "problem with gteq nan\n";
	if (gpsTime <= std::numeric_limits<sci::TaiTime>::quiet_NaN())
		std::cout << "problem with lteq nan\n";
	
}

