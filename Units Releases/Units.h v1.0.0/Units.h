#pragma once
//Copyright 2021 Philip Rosenberg https://science.cplusplus.engineering
//This source code is provided under the Science.cplusplus.engineering Code License v1.
//If you wish to get hold of additional copies of this code then you should do so 
//via my Patreon page at https://www.patreon.com/sciencecplusplusengineering.

//version 1.0.0

//if you intend utilising windows codepage text (i.e. not unicode), then you may
//wish to #define the following in your code before #invlude<units.h>
//#define ALTERNATE_MICRO "u"
//CP-1253 "Western Latin" characters for unicode symbols
//you may wish to create your own versions of these #defines
//if you are not in a region using CP-1253 or you want to
//use your own unicode alternatives
#if (defined _WIN32 && !defined UNITS_H_NOT_CP1253) || defined UNITS_H_FORCE_CP1253
#define ALTERNATE_MICRO "\xe6"
#define ALTERNATE_OMEGA "\xea"
#define ALTERNATE_PER_MILLE "0/00"
#define ALTERNATE_BASIS_POINT "0/000"
#define ALTERNATE_DEGREE "\xf8"
#define ALTERNATE_ARCMINUTE "'"
#define ALTERNATE_ARCSECOND "\""
#define ALTERNATE_ANGSTROM_SHORT "\x8e"
#define ALTERNATE_ANGSTROM_LONG "\x8e" "ngstrom"
#define ALTERNATE_RANKINE "\xf8" "Ra"
#endif

#include<vector>
#include<limits>
#include<string>
#include"Traits.h"
namespace sci
{
	//declarations and definitions of prefixes
	const int64_t yotta = 24;
	const int64_t zetta = 21;
	const int64_t exa = 18;
	const int64_t peta = 15;
	const int64_t tera = 12;
	const int64_t giga = 9;
	const int64_t mega = 6;
	const int64_t kilo = 3;
	const int64_t hecto = 2;
	const int64_t deca = 1;
	const int64_t deci = -1;
	const int64_t centi = -2;
	const int64_t milli = -3;
	const int64_t micro = -6;
	const int64_t nano = -9;
	const int64_t pico = -12;
	const int64_t femto = -15;
	const int64_t atto = -18;
	const int64_t zepto = -21;
	const int64_t yocto = -24;

	namespace unitsPrivate
	{
		template<int64_t EXPONENT>
		constexpr inline bool isMetricExponent()
		{
			return (EXPONENT > -4 && EXPONENT < 4) || (EXPONENT > -25 && EXPONENT < 25 && EXPONENT % 3 == 0);
		}
	}
	template<int64_t EXPONENT>
	struct ExponentTraits
	{
		constexpr static bool validSi = false;
		//we can't just put false in the static_assert. Some compilers see there is no dependence on the
		//template parameter and expand the static_assert even when this template is never instantiated
		template<class STRING> static STRING getName(){ static_assert(unitsPrivate::isMetricExponent<EXPONENT>(), "Cannot get a string for an exponent not represented by the standard metric prefixes"); }
		template<class STRING> static STRING getPrefix(){ static_assert(unitsPrivate::isMetricExponent<EXPONENT>(), "Cannot get a string for an exponent not represented by the standard metric prefixes"); }
	};

#define MAKE_EXPONENT_TRAITS(VALUE, LONG_NAME, ABBREVIATION)\
template<>\
struct ExponentTraits<VALUE>\
{\
	constexpr static bool validSi = true;\
	template<class STRING> static STRING getName(){ static_assert(false, "sci::ExponentTraits<VALUE>::getName<STRING> must have STRING be a std::string, std::wstring, std::basic_string<char8>, std::u16string or std::u32string"); }\
	template<> static std::string getName<std::string>() { return LONG_NAME; }\
	template<> static std::wstring getName<std::wstring>() { return L##LONG_NAME; }\
	template<> static std::basic_string<char8_t> getName<std::basic_string<char8_t>>() { return u8##LONG_NAME; }\
	template<> static std::u16string getName<std::u16string>() { return u##LONG_NAME; }\
	template<> static std::u32string getName<std::u32string>() { return U#LONG_NAME; }\
	template<class STRING> static STRING getPrefix(){ static_assert(false, "sci::ExponentTraits<VALUE>::getPrefix<STRING> must have STRING be a std::string, std::wstring, std::basic_string<char8>, std::u16string or std::u32string");}\
	template<> static std::string getPrefix<std::string>() { return ABBREVIATION; }\
	template<> static std::wstring getPrefix<std::wstring>() { return L##ABBREVIATION; }\
	template<> static std::basic_string<char8_t> getPrefix<std::basic_string<char8_t>>() { return u8##ABBREVIATION; }\
	template<> static std::u16string getPrefix<std::u16string>() { return u##ABBREVIATION; }\
	template<> static std::u32string getPrefix<std::u32string>() { return U##ABBREVIATION; }\
};
	//this allows expansion of the alternate abbreviation before ##,. e.g giving L"u" rather than LALTERNATE_MICRO
#define MAKE_ALTERNATE_EXPONENT_TRAITS(VALUE, LONG_NAME, ABBREVIATION) MAKE_EXPONENT_TRAITS(-6, "micro", ABBREVIATION)
	MAKE_EXPONENT_TRAITS(24, "yotta", "Y");
	MAKE_EXPONENT_TRAITS(21, "zetta", "Z");
	MAKE_EXPONENT_TRAITS(18, "exa", "E");
	MAKE_EXPONENT_TRAITS(15, "peta", "P");
	MAKE_EXPONENT_TRAITS(12, "tera", "T");
	MAKE_EXPONENT_TRAITS(9, "giga", "G");
	MAKE_EXPONENT_TRAITS(6, "mega", "M");
	MAKE_EXPONENT_TRAITS(3, "kilo", "k");
	MAKE_EXPONENT_TRAITS(2, "hecto", "h");
	MAKE_EXPONENT_TRAITS(1, "deca", "da");
	MAKE_EXPONENT_TRAITS(0, "", "");
	MAKE_EXPONENT_TRAITS(-1, "deci", "d");
	MAKE_EXPONENT_TRAITS(-2, "centi", "c");
	MAKE_EXPONENT_TRAITS(-3, "milli", "m");
#ifdef ALTERNATE_MICRO
	MAKE_ALTERNATE_EXPONENT_TRAITS(-6, "micro", ALTERNATE_MICRO);
#else
	MAKE_EXPONENT_TRAITS(-6, "micro", "\u03bc");
#endif
	MAKE_EXPONENT_TRAITS(-9, "nano", "n");
	MAKE_EXPONENT_TRAITS(-12, "pico", "p");
	MAKE_EXPONENT_TRAITS(-15, "femto", "f");
	MAKE_EXPONENT_TRAITS(-18, "atto", "a");
	MAKE_EXPONENT_TRAITS(-21, "zepto", "z");
	MAKE_EXPONENT_TRAITS(-24, "yocto", "y");

	namespace unitsPrivate
	{
		template <int8_t N1, int8_t N2>
		int8_t constexpr greatestCommonDenominator8()
		{

			if constexpr (N1 < 0)
				return greatestCommonDenominator8<-N1, N2>();
			if constexpr (N2 < 0)
				return greatestCommonDenominator8<N1, -N2>();
			if constexpr (N1 < N2)
				return greatestCommonDenominator8<N2, N1>();

			if constexpr (N2 == 0)
				return N1;
			else
				return greatestCommonDenominator8<N2, N1% N2>();
		}

		template <int64_t N1, int64_t N2>
		int64_t constexpr greatestCommonDenominator64()
		{

			if constexpr (N1 < 0)
				return greatestCommonDenominator64<-N1, N2>();
			if constexpr (N2 < 0)
				return greatestCommonDenominator64<N1, -N2>();
			if constexpr (N1 < N2)
				return greatestCommonDenominator64<N2, N1>();

			if constexpr (N2 == 0)
				return N1;
			else
				return greatestCommonDenominator64<N2, N1% N2>();
		}


		//convert a number to text
		template<class STRING>
		STRING unitNumberToText(int8_t number)
		{
			STRING result;
			if (number < 0)
			{
				result = result + STRING({ STRING::value_type(45) });
				number = -number;
			}
			//max 3 digits for a signed 8 bit number
			if (number < 10)
				result = result + STRING({ STRING::value_type(48 + number) });
			else if (number < 100)
				result = result + STRING({ STRING::value_type(48 + number / 10), STRING::value_type(48 + number % 10) });
			else
				result = result + STRING({ STRING::value_type(48 + number / 100), STRING::value_type(48 + (number % 100) / 10), STRING::value_type(48 + number % 10) });
			return result;

		}

		template<class STRING, int8_t POWER_NUMERATOR, int8_t POWER_DENOMINATOR>
		STRING powerToTextShort()
		{
			//divide through by greatest common denominator
			int8_t commonDenominator = greatestCommonDenominator8<POWER_NUMERATOR, POWER_DENOMINATOR>();
			int8_t powerNumerator = POWER_NUMERATOR / commonDenominator;
			int8_t powerDenominator = POWER_DENOMINATOR / commonDenominator;

			//make sure the denominator is positive
			if (powerDenominator < 0)
			{
				powerDenominator = -powerDenominator;
				powerNumerator = -powerNumerator;
			}
			//the basic ascii characters have the same representation in all sensible encodings
			//so we can use the same function irrespective of the string type
			if (powerDenominator == 1)
				return unitNumberToText<STRING>(powerNumerator);
			return unitNumberToText<STRING>(powerNumerator) + STRING({ STRING::value_type(47) }) + unitNumberToText<STRING>(powerDenominator);
		}
		template<class STRING, int8_t POWER_NUMERATOR, int8_t POWER_DENOMINATOR>
		STRING powerToTextLong()
		{
			if (POWER_NUMERATOR % POWER_DENOMINATOR == 0)
			{
				if (POWER_NUMERATOR / POWER_DENOMINATOR == 2)
				{
					if constexpr (std::is_same<STRING, std::string>::value)
						return " squared";
					if constexpr (std::is_same<STRING, std::wstring>::value)
						return L" squared";
					if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)
						return u8" squared";
					if constexpr (std::is_same<STRING, std::u16string>::value)
						return u" squared";
					if constexpr (std::is_same<STRING, std::u32string>::value)
						return U" squared";
				}
				if (POWER_NUMERATOR / POWER_DENOMINATOR == 3)
				{
					if constexpr (std::is_same<STRING, std::string>::value)
						return " cubed";
					if constexpr (std::is_same<STRING, std::wstring>::value)
						return L" cubed";
					if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)
						return u8" cubed";
					if constexpr (std::is_same<STRING, std::u16string>::value)
						return u" cubed";
					if constexpr (std::is_same<STRING, std::u32string>::value)
						return U" cubed";
				}
			}
			if constexpr (std::is_same<STRING, std::string>::value)
				return " to the power " + powerToTextShort<STRING, POWER_NUMERATOR, POWER_DENOMINATOR>();
			if constexpr (std::is_same<STRING, std::wstring>::value)
				return L" to the power " + powerToTextShort<STRING, POWER_NUMERATOR, POWER_DENOMINATOR>();
			if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)
				return u8" to the power " + powerToTextShort<STRING, POWER_NUMERATOR, POWER_DENOMINATOR>();
			if constexpr (std::is_same<STRING, std::u16string>::value)
				return u" to the power " + powerToTextShort<STRING, POWER_NUMERATOR, POWER_DENOMINATOR>();
			if constexpr (std::is_same<STRING, std::u32string>::value)
				return U" to the power " + powerToTextShort<STRING, POWER_NUMERATOR, POWER_DENOMINATOR>();
		}

		//build a string showing the unit. Note that exponent must be before multiplying by power
		template<int8_t POWER_NUMERATOR, int8_t POWER_DENOMINATOR, int64_t EXPONENT, class STRING>
		STRING makeShortName(const STRING& unit, const STRING& powerPrefix, const STRING& powerSuffix)
		{
			if (POWER_NUMERATOR == 0)
				return STRING();

			if (POWER_NUMERATOR == POWER_DENOMINATOR)
				return ExponentTraits<EXPONENT>::template getPrefix<STRING>() + unit;

			return ExponentTraits<EXPONENT>::template getPrefix<STRING>() + unit + powerPrefix + powerToTextShort<STRING, POWER_NUMERATOR, POWER_DENOMINATOR>() + powerSuffix;
		}

		//build a string showing the unit. Note that exponent must be before multiplying by power
		template<int8_t POWER_NUMERATOR, int8_t POWER_DENOMINATOR, int64_t EXPONENT, class STRING>
		STRING makeLongName(const STRING& unit)
		{
			STRING per = STRING();
			if (POWER_NUMERATOR * POWER_DENOMINATOR < 0)
			{

				if constexpr (std::is_same<STRING, std::string>::value)
					per = "per ";
				if constexpr (std::is_same<STRING, std::wstring>::value)
					per = L"per ";
				if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)
					per = u8"per ";
				if constexpr (std::is_same<STRING, std::u16string>::value)
					per = u"per ";
				if constexpr (std::is_same<STRING, std::u32string>::value)
					per = U"per ";
			}

			if (POWER_NUMERATOR == 0)
				return STRING();

			if (POWER_NUMERATOR == POWER_DENOMINATOR)
				return ExponentTraits<EXPONENT>::template getName<STRING>() + per + unit;
			if (POWER_NUMERATOR < 0 && POWER_DENOMINATOR >= 0)
				return ExponentTraits<EXPONENT>::template getName<STRING>() + per + unit + powerToTextLong<STRING, -POWER_NUMERATOR, POWER_DENOMINATOR>();
			if (POWER_NUMERATOR >= 0 && POWER_DENOMINATOR < 0)
				return ExponentTraits<EXPONENT>::template getName<STRING>() + per + unit + powerToTextLong<STRING, POWER_NUMERATOR, -POWER_DENOMINATOR>();
			if (POWER_NUMERATOR < 0 && POWER_DENOMINATOR < 0)
				return ExponentTraits<EXPONENT>::template getName<STRING>() + per + unit + powerToTextLong<STRING, -POWER_NUMERATOR, -POWER_DENOMINATOR>();
			return ExponentTraits<EXPONENT>::template getName<STRING>() + per + unit + powerToTextLong<STRING, POWER_NUMERATOR, POWER_DENOMINATOR>();
		}



		//This function encodes the power variable for the EncodedUnit class
		template<int8_t POWER, int BYTE>
		constexpr uint64_t encodePower()
		{
			//take the raw bits and cast them to an unsigned 8 bit
			//value, then convert this to a 64 bit value then bitshift
			//it by the appropriate amount
			return uint64_t(uint8_t(POWER)) << (BYTE * 8);
		}

		//This function decodes the power variable for the EncodedUnit class
		template <uint64_t POWER, int BYTE>
		constexpr int8_t decodePower()
		{
			//this should filter out the correct byte, bitshift it to the
			return int8_t((POWER & (uint64_t(0xff) << (BYTE * 8))) >> (BYTE * 8));
		}


		//These functions will reencode the encoded power variables when two units are
		//multiplied. Because the powers are fractions, we need to create a common denominator

		//This funtion calculates the numerators of the new powers when two units are multipled
		template<uint64_t FIRST_NUMERATOR, uint64_t FIRST_DENOMINATOR, uint64_t SECOND_NUMERATOR, uint64_t SECOND_DENOMINATOR>
		constexpr uint64_t multiplyPowersNumerator()
		{

			return
				encodePower<decodePower<FIRST_NUMERATOR, 0>()* decodePower<SECOND_DENOMINATOR, 0>() + decodePower<SECOND_NUMERATOR, 0>() * decodePower<FIRST_DENOMINATOR, 0>(), 0>() |
				encodePower<decodePower<FIRST_NUMERATOR, 1>()* decodePower<SECOND_DENOMINATOR, 1>() + decodePower<SECOND_NUMERATOR, 1>() * decodePower<FIRST_DENOMINATOR, 1>(), 1>() |
				encodePower<decodePower<FIRST_NUMERATOR, 2>()* decodePower<SECOND_DENOMINATOR, 2>() + decodePower<SECOND_NUMERATOR, 2>() * decodePower<FIRST_DENOMINATOR, 2>(), 2>() |
				encodePower<decodePower<FIRST_NUMERATOR, 3>()* decodePower<SECOND_DENOMINATOR, 3>() + decodePower<SECOND_NUMERATOR, 3>() * decodePower<FIRST_DENOMINATOR, 3>(), 3>() |
				encodePower<decodePower<FIRST_NUMERATOR, 4>()* decodePower<SECOND_DENOMINATOR, 4>() + decodePower<SECOND_NUMERATOR, 4>() * decodePower<FIRST_DENOMINATOR, 4>(), 4>() |
				encodePower<decodePower<FIRST_NUMERATOR, 5>()* decodePower<SECOND_DENOMINATOR, 5>() + decodePower<SECOND_NUMERATOR, 5>() * decodePower<FIRST_DENOMINATOR, 5>(), 5>() |
				encodePower<decodePower<FIRST_NUMERATOR, 6>()* decodePower<SECOND_DENOMINATOR, 6>() + decodePower<SECOND_NUMERATOR, 6>() * decodePower<FIRST_DENOMINATOR, 6>(), 6>() |
				encodePower<decodePower<FIRST_NUMERATOR, 7>()* decodePower<SECOND_DENOMINATOR, 7>() + decodePower<SECOND_NUMERATOR, 7>() * decodePower<FIRST_DENOMINATOR, 7>(), 7>();
		}

		//This function calculates the denominators of the new powers when two units are multiplied
		template<uint64_t FIRST_DENOMINATOR, uint64_t SECOND_DENOMINATOR>
		constexpr uint64_t multiplyPowersDenominator()
		{
			return
				encodePower<decodePower<FIRST_DENOMINATOR, 0>()* decodePower<SECOND_DENOMINATOR, 0>(), 0>() |
				encodePower<decodePower<FIRST_DENOMINATOR, 1>()* decodePower<SECOND_DENOMINATOR, 1>(), 1>() |
				encodePower<decodePower<FIRST_DENOMINATOR, 2>()* decodePower<SECOND_DENOMINATOR, 2>(), 2>() |
				encodePower<decodePower<FIRST_DENOMINATOR, 3>()* decodePower<SECOND_DENOMINATOR, 3>(), 3>() |
				encodePower<decodePower<FIRST_DENOMINATOR, 4>()* decodePower<SECOND_DENOMINATOR, 4>(), 4>() |
				encodePower<decodePower<FIRST_DENOMINATOR, 5>()* decodePower<SECOND_DENOMINATOR, 5>(), 5>() |
				encodePower<decodePower<FIRST_DENOMINATOR, 6>()* decodePower<SECOND_DENOMINATOR, 6>(), 6>() |
				encodePower<decodePower<FIRST_DENOMINATOR, 7>()* decodePower<SECOND_DENOMINATOR, 7>(), 7>();
		}

		//This function calculates the numerator of the new powers when a unit is raised to a power
		//Pass in the unit and power numerator or the unit and power denominator to get the resulting
		//unit power numerator and denominator
		template<uint64_t VALUE, int POW>
		constexpr uint64_t powPowers()
		{

			return
				encodePower<decodePower<VALUE, 0>()* POW, 0>() |
				encodePower<decodePower<VALUE, 1>()* POW, 1>() |
				encodePower<decodePower<VALUE, 2>()* POW, 2>() |
				encodePower<decodePower<VALUE, 3>()* POW, 3>() |
				encodePower<decodePower<VALUE, 4>()* POW, 4>() |
				encodePower<decodePower<VALUE, 5>()* POW, 5>() |
				encodePower<decodePower<VALUE, 6>()* POW, 6>() |
				encodePower<decodePower<VALUE, 7>()* POW, 7>();
		}

		//These enable raising 10 to the power of integers get
		//determined at compile time as it will be a common operation
		//when adding and subtracting
		template <class VALUE_TYPE>
		constexpr VALUE_TYPE pow10(int64_t pow)
		{
			if (pow == 0)
				return VALUE_TYPE(1.);
			if (pow == 1)
				return VALUE_TYPE(10.);
			if (pow == 2)
				return VALUE_TYPE(100.);
			if (pow == 3)
				return VALUE_TYPE(1000.);
			if (pow == -1)
				return VALUE_TYPE(0.1);
			if (pow == -2)
				return VALUE_TYPE(0.01);
			if (pow == -3)
				return VALUE_TYPE(0.001);

			VALUE_TYPE temp = pow10<VALUE_TYPE>(pow / 2);
			if (pow % 2 == 0)
				return temp * temp;
			if (pow > 0)
				return temp * temp * 10;
			return temp * temp * VALUE_TYPE(0.1);
		}
		template <int64_t pow, class VALUE_TYPE>
		constexpr VALUE_TYPE pow10()
		{
			if (pow == 0)
				return VALUE_TYPE(1);
			return pow10<VALUE_TYPE>(pow);
		}

		template<class VALUE_TYPE>
		constexpr VALUE_TYPE pow(VALUE_TYPE base, int power)
		{
			if (power < 0)
				return VALUE_TYPE(1.0) / pow(base, -power);
			if (power == 0 && base == VALUE_TYPE(0.0))
				return std::numeric_limits<VALUE_TYPE>::quiet_NaN();
			if (power == 0)
				return VALUE_TYPE(1.0);
			if (power == 1)
				return base;
			if (power == 2)
				return base * base;
			if (power == 3)
				return base * (base * base);
			if (power == 4)
				return (base * base) * (base * base);
			return pow(base, power / 2) * pow(base, power - power / 2);
		}

		template<class VALUE_TYPE>
		constexpr VALUE_TYPE sqrt(VALUE_TYPE value)
		{
			if (value < VALUE_TYPE(0.0))
				return std::numeric_limits<VALUE_TYPE>::quiet_NaN();
			if (value == VALUE_TYPE(0.0))
				return VALUE_TYPE(0.0);
			if (value == VALUE_TYPE(1.0))
				return VALUE_TYPE(1.0);
			if (value != value)
				return value;

			VALUE_TYPE twoToTheN = 1;
			VALUE_TYPE a = value;
			while (a > VALUE_TYPE(4))
			{
				a /= VALUE_TYPE(4);
				twoToTheN *= 2;
			}
			while (a < VALUE_TYPE(1))
			{
				a *= VALUE_TYPE(4);
				twoToTheN /= 2;
			}
			VALUE_TYPE currentGuess = (VALUE_TYPE(0.5) + VALUE_TYPE(0.5) * a) * twoToTheN;

			if constexpr (std::is_same<VALUE_TYPE, double>::value && std::numeric_limits<double>::is_iec559)
			{
				//5 iterations is enough to get to the max possible accuracy for an ieee double
				//anoyingly, 4 is enough to get within a factor of a few of epsilon, the fifth
				//is probably overkill, but c'est la vie.
				currentGuess = VALUE_TYPE(0.5) * (currentGuess + value / currentGuess);
				currentGuess = VALUE_TYPE(0.5) * (currentGuess + value / currentGuess);
				currentGuess = VALUE_TYPE(0.5) * (currentGuess + value / currentGuess);
				currentGuess = VALUE_TYPE(0.5) * (currentGuess + value / currentGuess);
				currentGuess = VALUE_TYPE(0.5) * (currentGuess + value / currentGuess);
				return currentGuess;
			}
			else if constexpr (std::is_same<VALUE_TYPE, float>::value && std::numeric_limits<float>::is_iec559)
			{
				//3 iterations is enough to get to the max possible accuracy for an ieee float
				currentGuess = VALUE_TYPE(0.5) * (currentGuess + value / currentGuess);
				currentGuess = VALUE_TYPE(0.5) * (currentGuess + value / currentGuess);
				currentGuess = VALUE_TYPE(0.5) * (currentGuess + value / currentGuess);
				return currentGuess;
			}
			else
			{
				//if we are not using ieee floating point numbers then we set an
				//accepatable error based on epsilon
				VALUE_TYPE nextGuess = VALUE_TYPE(0.5) * (currentGuess + value / currentGuess);
				VALUE_TYPE err;
				if (currentGuess > nextGuess)
					err = currentGuess / nextGuess - VALUE_TYPE(1.0);
				else
					err = nextGuess / currentGuess - VALUE_TYPE(1.0);
				while (err > VALUE_TYPE(2.0) * std::numeric_limits<VALUE_TYPE>::epsilon())
				{
					currentGuess = nextGuess;
					nextGuess = VALUE_TYPE(0.5) * (currentGuess + value / currentGuess);
					if (currentGuess > nextGuess)
						err = currentGuess / nextGuess - VALUE_TYPE(1.0);
					else
						err = nextGuess / currentGuess - VALUE_TYPE(1.0);
				}
				return nextGuess;
			}
		}

		template<class VALUE_TYPE>
		constexpr VALUE_TYPE root(VALUE_TYPE value, int root)
		{
			if (value < VALUE_TYPE(0.0) && root % 2 == 0)
				return std::numeric_limits<VALUE_TYPE>::quiet_NaN();
			if (root == 0)
				return std::numeric_limits<VALUE_TYPE>::quiet_NaN();
			if (value < VALUE_TYPE(0.0))
				return -unitsPrivate::root(-value, root);
			if (value == VALUE_TYPE(0.0))
				return VALUE_TYPE(0.0);
			if (value == VALUE_TYPE(1.0))
				return VALUE_TYPE(1.0);
			if (value != value)
				return value;
			if (root == 1)
				return value;
			if (root < 0)
				return VALUE_TYPE(1) / unitsPrivate::root(value, -root);
			if (root == 0)
				return std::numeric_limits<VALUE_TYPE>::quiet_NaN();
			VALUE_TYPE twoToTheN = 1;
			VALUE_TYPE a = value;
			VALUE_TYPE maxVal = pow(VALUE_TYPE(2), root);
			while (a > maxVal)
			{
				a /= maxVal;
				twoToTheN *= 2;
			}
			while (a < VALUE_TYPE(1))
			{
				a *= maxVal;
				twoToTheN /= 2;
			}

			VALUE_TYPE currentGuess = (VALUE_TYPE(1) + (a - VALUE_TYPE(1)) / (maxVal - VALUE_TYPE(1))) * twoToTheN;
			VALUE_TYPE nextGuess = (VALUE_TYPE(1.0) / VALUE_TYPE(root)) * (currentGuess * (root - 1) + value / pow(currentGuess, root - 1));
			VALUE_TYPE err = currentGuess > nextGuess ? currentGuess / nextGuess - VALUE_TYPE(1.0) : nextGuess / currentGuess - VALUE_TYPE(1.0);

			while (err > VALUE_TYPE(2.0) * std::numeric_limits<VALUE_TYPE>::epsilon())
			{
				currentGuess = nextGuess;
				nextGuess = (VALUE_TYPE(1.0) / VALUE_TYPE(root)) * (currentGuess * (root - 1) + value / pow(currentGuess, root - 1));
				if (currentGuess > nextGuess)
					err = currentGuess / nextGuess - VALUE_TYPE(1.0);
				else
					err = nextGuess / currentGuess - VALUE_TYPE(1.0);
			}
			return nextGuess;
		}

		//Functions to check the compatibility of units

		template< int64_t NUM1, int64_t DEN1, int64_t NUM2, int64_t DEN2>
		constexpr bool fractionsEquivalent()
		{
			//multiplying each numerator by the other denominator scales the fractions to a common denominator
			return (NUM1 * DEN2) == (NUM2 * DEN1);
		}
		template<class UNIT1, class UNIT2>
		constexpr bool unitCompatible()
		{
			return
				fractionsEquivalent<decodePower< UNIT1::unit::basePowersNumerators, 0>(), decodePower< UNIT1::unit::basePowersDenominators, 0>(), decodePower< UNIT2::unit::basePowersNumerators, 0>(), decodePower< UNIT2::unit::basePowersDenominators, 0>()>() &&
				fractionsEquivalent<decodePower< UNIT1::unit::basePowersNumerators, 1>(), decodePower< UNIT1::unit::basePowersDenominators, 1>(), decodePower< UNIT2::unit::basePowersNumerators, 1>(), decodePower< UNIT2::unit::basePowersDenominators, 1>()>() &&
				fractionsEquivalent<decodePower< UNIT1::unit::basePowersNumerators, 2>(), decodePower< UNIT1::unit::basePowersDenominators, 2>(), decodePower< UNIT2::unit::basePowersNumerators, 2>(), decodePower< UNIT2::unit::basePowersDenominators, 2>()>() &&
				fractionsEquivalent<decodePower< UNIT1::unit::basePowersNumerators, 3>(), decodePower< UNIT1::unit::basePowersDenominators, 3>(), decodePower< UNIT2::unit::basePowersNumerators, 3>(), decodePower< UNIT2::unit::basePowersDenominators, 3>()>() &&
				fractionsEquivalent<decodePower< UNIT1::unit::basePowersNumerators, 4>(), decodePower< UNIT1::unit::basePowersDenominators, 4>(), decodePower< UNIT2::unit::basePowersNumerators, 4>(), decodePower< UNIT2::unit::basePowersDenominators, 4>()>() &&
				fractionsEquivalent<decodePower< UNIT1::unit::basePowersNumerators, 5>(), decodePower< UNIT1::unit::basePowersDenominators, 5>(), decodePower< UNIT2::unit::basePowersNumerators, 5>(), decodePower< UNIT2::unit::basePowersDenominators, 5>()>() &&
				fractionsEquivalent<decodePower< UNIT1::unit::basePowersNumerators, 6>(), decodePower< UNIT1::unit::basePowersDenominators, 6>(), decodePower< UNIT2::unit::basePowersNumerators, 6>(), decodePower< UNIT2::unit::basePowersDenominators, 6>()>() &&
				fractionsEquivalent<decodePower< UNIT1::unit::basePowersNumerators, 7>(), decodePower< UNIT1::unit::basePowersDenominators, 7>(), decodePower< UNIT2::unit::basePowersNumerators, 7>(), decodePower< UNIT2::unit::basePowersDenominators, 7>()>();

		}
		template<class UNIT1, class UNIT2>
		constexpr bool unitSameExponent()
		{
			//scale powers to a common denominator and compare - basically multiply each numerator by the other unit's denominator
			return fractionsEquivalent < UNIT1::unit::exponentNumerator, UNIT1::unit::exponentDenominator, UNIT2::unit::exponentNumerator, UNIT2::unit::exponentDenominator>();
		}
		template<class UNIT1, class UNIT2>
		constexpr bool unitIdentical()
		{
			return unitCompatible<UNIT1, UNIT2>() && unitSameExponent<UNIT1, UNIT2>();
		}

		template<int64_t SOURCE_EXPONENT_NUMERATOR, int64_t SOURCE_EXPONENT_DENOMINATOR, int64_t DEST_EXPONENT_NUMERATOR, int64_t DEST_EXPONENT_DENOMINATOR, class VALUE_TYPE>
		constexpr VALUE_TYPE applyExponents(VALUE_TYPE value)
		{
			if constexpr (fractionsEquivalent<SOURCE_EXPONENT_NUMERATOR, SOURCE_EXPONENT_DENOMINATOR, DEST_EXPONENT_NUMERATOR, DEST_EXPONENT_DENOMINATOR>())
				return value;
			constexpr uint64_t num = SOURCE_EXPONENT_NUMERATOR * DEST_EXPONENT_DENOMINATOR - DEST_EXPONENT_NUMERATOR * SOURCE_EXPONENT_DENOMINATOR;
			constexpr uint64_t den = DEST_EXPONENT_DENOMINATOR * SOURCE_EXPONENT_DENOMINATOR;
			if constexpr (num % den == 0)
				return value * pow10<num / den, VALUE_TYPE>();
			constexpr int64_t gcd = greatestCommonDenominator64<num, den>();
			return value * root(pow10<num / gcd, VALUE_TYPE>(), den / gcd);
		}

		//This is the basic class that represents a unit. It takes two templated parameters
		//The first is the powers. This is an encoded 64 bit number where the first byte is
		//the power in the first dimension, the second byte is the power in the second
		//dimension, etc.
		//The second is the exponent the unit would have ONCE THE POWER HAS BEEN MULTIPLIED OUT.
		//For example for a millimetre it would be -3, for a millimetre squared it would be -6, etc
		template<uint64_t BASE_POWERS_NUMERATORS, uint64_t BASE_POWERS_DENOMINATORS, int64_t EXPONENT_NUMERATOR, int64_t EXPONENT_DENOMINATOR, bool NEEDS_SCALING>
		struct EncodedUnit
		{
			static constexpr bool needsScaling = NEEDS_SCALING;
			static constexpr uint64_t basePowersNumerators = BASE_POWERS_NUMERATORS;
			static constexpr uint64_t basePowersDenominators = BASE_POWERS_DENOMINATORS;
			static constexpr int64_t exponentNumerator = EXPONENT_NUMERATOR;
			static constexpr int64_t exponentDenominator = EXPONENT_DENOMINATOR;
			typedef EncodedUnit< basePowersNumerators, basePowersDenominators, exponentNumerator, exponentDenominator, needsScaling> unit;
			typedef unit baseClass;

			template<class VALUE_TYPE>
			struct Converter //We have to put all these functions into a struct to avoid having to partial specialize them
			{
				template <class DESTINATION_UNIT>
				static constexpr VALUE_TYPE convertTo(VALUE_TYPE value)
				{
					static_assert(unitsPrivate::unitCompatible<unit, DESTINATION_UNIT>(), "Cannot convert between units with different powers or dimensions.");
					//If the destination unit is identical to this unit we can do nothing
					//not sure if this really optimises anything, but we might as well do it
					//Note that we can't just check equivalence of the base powers and exponent
					//as the DESTINATION_UNIT might be a scaled unit.
					if constexpr (std::is_same<unit, DESTINATION_UNIT>::value)
						return value;
					//This class is always the base type for the DESTINATION_UNIT, but with potentially different exponents
					//therefore to convert from this type, to the DESTINATION_UNIT we can use the DESTINATION_UNIT::convertFromBase
					//function, with the alternate exponent provided from here. This ends the recursion.
					return DESTINATION_UNIT::template Converter<VALUE_TYPE>::template convertFromBase<EXPONENT_NUMERATOR, EXPONENT_DENOMINATOR>(value);
				}
				//convert value to this type, from the base type, but with the base type having the base exponent numerator and denominator as given
				template <int64_t BASE_EXPONENT_NUMERATOR, int64_t BASE_EXPONENT_DENOMINATOR>
				static constexpr VALUE_TYPE convertFromBase(VALUE_TYPE value)
				{
					//for this type its base is itself, but with a different exponent
					//so all we need to do is apply the difference in the exponent
					if constexpr (unitsPrivate::fractionsEquivalent<EXPONENT_NUMERATOR, EXPONENT_DENOMINATOR, BASE_EXPONENT_NUMERATOR, BASE_EXPONENT_DENOMINATOR>())
						return value;
					//check if we can use the templated recursive pow10
					constexpr int64_t NUM = BASE_EXPONENT_NUMERATOR * EXPONENT_DENOMINATOR - EXPONENT_NUMERATOR * BASE_EXPONENT_DENOMINATOR;
					constexpr int64_t DEN = BASE_EXPONENT_DENOMINATOR * EXPONENT_DENOMINATOR;
					if constexpr (NUM % DEN == 0)
						return value * sci::unitsPrivate::pow10<NUM / DEN, VALUE_TYPE>();
					return value * std::pow(VALUE_TYPE(10.0), VALUE_TYPE(NUM) / VALUE_TYPE(DEN));
				}
			};
			static constexpr bool isUnitless()
			{
				return basePowersNumerators == 0;
			}
			template<class OTHER_UNIT>
			static constexpr bool compatibleWith()
			{
				return unitsPrivate::unitCompatible<unit, OTHER_UNIT>();
			}
		};

		template<uint64_t BASE_POWERS, int64_t EXPONENT, bool NEEDS_SCALING>
		struct EncodedUnitWholePower : public EncodedUnit<BASE_POWERS, 0x0101010101010101, EXPONENT, 1, NEEDS_SCALING>
		{
			static constexpr bool needsScaling = NEEDS_SCALING;
		};
	}

	//This is a class that represents an encoded unit to a power. It is still an EncodedUnit
	//by inheritance, so you can multiply, divide or raise to the power many units
	template<class ENCODEDUNIT, int8_t POW_NUMERATOR, int8_t POW_DENOMINATOR=1>
	struct PoweredUnit : public unitsPrivate::EncodedUnit<unitsPrivate::powPowers<ENCODEDUNIT::basePowersNumerators, POW_NUMERATOR>(), unitsPrivate::powPowers<ENCODEDUNIT::basePowersDenominators, POW_DENOMINATOR>(), ENCODEDUNIT::exponentNumerator * POW_NUMERATOR, ENCODEDUNIT::exponentDenominator * POW_DENOMINATOR, ENCODEDUNIT::needsScaling>
	{
		static constexpr bool needsScaling = ENCODEDUNIT::needsScaling;
		typedef PoweredUnit< ENCODEDUNIT, POW_NUMERATOR, POW_DENOMINATOR> unit;
		typedef unitsPrivate::EncodedUnit<unitsPrivate::powPowers<ENCODEDUNIT::basePowersNumerators, POW_NUMERATOR>(), unitsPrivate::powPowers<ENCODEDUNIT::basePowersDenominators, POW_DENOMINATOR>(), ENCODEDUNIT::exponentNumerator * POW_NUMERATOR, ENCODEDUNIT::exponentDenominator * POW_DENOMINATOR, ENCODEDUNIT::needsScaling> encodedUnitClass;
		template<class STRING>
		static STRING getShortRepresentation(const STRING& exponentPrefix = STRING(), const STRING& exponentSuffix = STRING())
		{
			return ENCODEDUNIT::getShortRepresentation<POW_NUMERATOR, POW_DENOMINATOR>(exponentPrefix, exponentSuffix);
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getShortRepresentation(const STRING& exponentPrefix, const STRING& exponentSuffix)
		{
			return ENCODEDUNIT::getShortRepresentation<POW_NUMERATOR * EXTRA_POWER_NUMERATOR, POW_DENOMINATOR * EXTRA_POWER_DENOMINATOR>(exponentPrefix, exponentSuffix);
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix = STRING(), const STRING& exponentSuffix = STRING())
		{
			return ENCODEDUNIT::template getLongRepresentation<POW_NUMERATOR, POW_DENOMINATOR, STRING>();
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getLongRepresentation()
		{
			return ENCODEDUNIT::template getLongRepresentation<POW_NUMERATOR* EXTRA_POWER_NUMERATOR, POW_DENOMINATOR* EXTRA_POWER_DENOMINATOR, STRING>();
		}
		template<class VALUE_TYPE>
		struct Converter //We have to put all these functions into a struct to avoid having to partial specialize them
		{
			template <class DESTINATION_UNIT>
			static constexpr VALUE_TYPE convertTo(VALUE_TYPE value)
			{
				static_assert(unitsPrivate::unitCompatible<unit, DESTINATION_UNIT>(), "Cannot convert between units with different powers or dimensions.");
				if constexpr (!needsScaling && !DESTINATION_UNIT::needsScaling)
					return unitsPrivate::applyExponents<encodedUnitClass::exponentNumerator, encodedUnitClass::exponentDenominator, DESTINATION_UNIT::exponentNumerator, DESTINATION_UNIT::exponentDenominator>(value);
					
				//This unit and the DESTINATION_UNIT must share a common base type
				//but with a possibly different exponent. So to do the conversion I convert the
				//value to the base unit, then convert that to the destination unit using its
				//convertFromBase method but with the alternative exponents. This breaks the recursion.
				//
				//convert the value to the base type, we cascade the conversion down to the unit we are raising to the power,
				//then apply the power
				constexpr int8_t maxCommonDenominator = unitsPrivate::greatestCommonDenominator8<POW_NUMERATOR, POW_DENOMINATOR>();
				constexpr VALUE_TYPE multiplier = unitsPrivate::root(unitsPrivate::pow(ENCODEDUNIT:: template Converter<VALUE_TYPE>::template convertTo<typename ENCODEDUNIT::baseClass>(VALUE_TYPE(1.0)), POW_NUMERATOR / maxCommonDenominator),POW_DENOMINATOR / maxCommonDenominator);
				VALUE_TYPE baseValue = value * multiplier;
				//now convert from the base unit to the destination unit, with the different exponents.
				return DESTINATION_UNIT::template Converter<VALUE_TYPE>::template convertFromBase<encodedUnitClass::exponentNumerator, encodedUnitClass::exponentDenominator>(baseValue);
				
			}
			template <int64_t BASE_EXPONENT_NUMERATOR, int64_t BASE_EXPONENT_DENOMINATOR>
			static constexpr VALUE_TYPE convertFromBase(VALUE_TYPE value)
			{
				//cascade the conversion down to the unit we are raising to the power, then apply the power
				constexpr int8_t maxCommonDenominator = unitsPrivate::greatestCommonDenominator8<POW_NUMERATOR, POW_DENOMINATOR>();
				constexpr VALUE_TYPE scaleMultiplier = unitsPrivate::root(unitsPrivate::pow(ENCODEDUNIT::template Converter<VALUE_TYPE>::template convertFromBase<BASE_EXPONENT_NUMERATOR, BASE_EXPONENT_DENOMINATOR>(VALUE_TYPE(1.0)), POW_NUMERATOR / maxCommonDenominator), POW_DENOMINATOR / maxCommonDenominator);

				return value * scaleMultiplier;
			}
		};
	};

	//This is a class that represents an encoded unit rooted. It is still an EncodedUnit
	//by inheritance, so you can multiply, divide or raise to the power many units
	template<class ENCODEDUNIT, int ROOT>
	struct RootedUnit : public PoweredUnit<ENCODEDUNIT, 1, ROOT>
	{
	};

	//This is a class that represents two encoded units multiplied together. It is still an EncodedUnit
	//by inheritance, so you can multiply, divide or raise to the power many units.
	template<class ENCODEDUNIT1, class ENCODEDUNIT2>
	struct MultipliedUnit : public unitsPrivate::EncodedUnit<unitsPrivate::multiplyPowersNumerator<ENCODEDUNIT1::basePowersNumerators, ENCODEDUNIT1::basePowersDenominators, ENCODEDUNIT2::basePowersNumerators, ENCODEDUNIT2::basePowersDenominators>(), unitsPrivate::multiplyPowersDenominator<ENCODEDUNIT1::basePowersDenominators, ENCODEDUNIT2::basePowersDenominators>(), ENCODEDUNIT1::exponentNumerator*ENCODEDUNIT2::exponentDenominator + ENCODEDUNIT2::exponentNumerator * ENCODEDUNIT1::exponentDenominator, ENCODEDUNIT1::exponentDenominator*ENCODEDUNIT2::exponentDenominator, ENCODEDUNIT1::needsScaling || ENCODEDUNIT2::needsScaling>
	{
		static constexpr bool needsScaling = ENCODEDUNIT1::needsScaling || ENCODEDUNIT2::needsScaling;
		typedef MultipliedUnit< ENCODEDUNIT1, ENCODEDUNIT2> unit;
		typedef unitsPrivate::EncodedUnit<unitsPrivate::multiplyPowersNumerator<ENCODEDUNIT1::basePowersNumerators, ENCODEDUNIT1::basePowersDenominators, ENCODEDUNIT2::basePowersNumerators, ENCODEDUNIT2::basePowersDenominators>(), unitsPrivate::multiplyPowersDenominator<ENCODEDUNIT1::basePowersDenominators, ENCODEDUNIT2::basePowersDenominators>(), ENCODEDUNIT1::exponentNumerator* ENCODEDUNIT2::exponentDenominator + ENCODEDUNIT2::exponentNumerator * ENCODEDUNIT1::exponentDenominator, ENCODEDUNIT1::exponentDenominator* ENCODEDUNIT2::exponentDenominator, ENCODEDUNIT1::needsScaling || ENCODEDUNIT2::needsScaling> encodedUnitClass;
		typedef MultipliedUnit<typename ENCODEDUNIT1::baseClass, typename ENCODEDUNIT2::baseClass> baseClass;
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			//for all sensible encodings a space is represented by value of 32, so we don't need to do any encoding conversion
			return ENCODEDUNIT1::getShortRepresentation(exponentPrefix, exponentSuffix) + STRING{STRING::value_type(32)} + ENCODEDUNIT2::getShortRepresentation(exponentPrefix, exponentSuffix);
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getShortRepresentation(const STRING& exponentPrefix, const STRING& exponentSuffix)
		{
			return ENCODEDUNIT1::getShortRepresentation<EXTRA_POWER_NUMERATOR, EXTRA_POWER_DENOMINATOR>(exponentPrefix, exponentSuffix) + STRING{ STRING::value_type(32) } + ENCODEDUNIT2::getShortRepresentation<EXTRA_POWER_NUMERATOR, EXTRA_POWER_DENOMINATOR>(exponentPrefix, exponentSuffix);
		}
		template<class STRING>
		static STRING getLongRepresentation()
		{
			//for all sensible encodings a space is represented by value of 32, so we don't need to do any encoding conversion
			return ENCODEDUNIT1::template getLongRepresentation<STRING>() + STRING{ STRING::value_type(32) } + ENCODEDUNIT2::template getLongRepresentation<STRING>();
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getLongRepresentation()
		{
			return ENCODEDUNIT1::template getLongRepresentation<EXTRA_POWER_NUMERATOR, EXTRA_POWER_DENOMINATOR,STRING>() + STRING{ STRING::value_type(32) } + ENCODEDUNIT2::template getLongRepresentation<EXTRA_POWER_NUMERATOR, EXTRA_POWER_DENOMINATOR, STRING>();
		}

		template<class VALUE_TYPE>
		struct Converter //We have to put all these functions into a struct to avoid having to partial specialize them
		{
			template <class DESTINATION_UNIT>
			static constexpr VALUE_TYPE convertTo(VALUE_TYPE value)
			{
				static_assert(unitsPrivate::unitCompatible<unit, DESTINATION_UNIT>(), "Cannot convert between units with different powers or dimensions.");
				if constexpr (!needsScaling && !DESTINATION_UNIT::needsScaling)
					return unitsPrivate::applyExponents<encodedUnitClass::exponentNumerator, encodedUnitClass::exponentDenominator, DESTINATION_UNIT::exponentNumerator, DESTINATION_UNIT::exponentDenominator>(value);

				//This unit and the DESTINATION_UNIT must share a common base type
				//but with a possibly different exponent. So to do the conversion I convert the
				//value to the base unit, then convert that to the destination unit using its
				//convertFromBase method but with the alternative exponents. This breaks the recursion.
				//
				//convert the value to the base type, we cascade the conversion down to the units we are
				//multiplying, then apply this to the value
				constexpr VALUE_TYPE multiplier1 = ENCODEDUNIT1:: template Converter<VALUE_TYPE>::template convertTo<typename ENCODEDUNIT1::baseClass>(VALUE_TYPE(1.0));
				constexpr VALUE_TYPE multiplier2 = ENCODEDUNIT2:: template Converter<VALUE_TYPE>::template convertTo<typename ENCODEDUNIT2::baseClass>(VALUE_TYPE(1.0));
				VALUE_TYPE baseValue = value * multiplier1 * multiplier2;
				//now convert from the base unit to the destination unit, with the different exponents.
				return DESTINATION_UNIT::template Converter<VALUE_TYPE>::template convertFromBase<encodedUnitClass::exponentNumerator, encodedUnitClass::exponentDenominator>(baseValue);
			}
			template <int64_t BASE_EXPONENT_NUMERATOR, int64_t BASE_EXPONENT_DENOMINATOR>
			static constexpr VALUE_TYPE convertFromBase(VALUE_TYPE value)
			{
				//Use each encodedUnit's convert from base function. The first one uses the exponent and the value, the second uses 1
				return ENCODEDUNIT1::template Converter<VALUE_TYPE>::template convertFromBase<BASE_EXPONENT_NUMERATOR, BASE_EXPONENT_DENOMINATOR>(value)*ENCODEDUNIT2::template Converter<VALUE_TYPE>::template convertFromBase<0,1>(1.0);
			}
		};
	};

	//This is a class that represents two encoded units divided by each other. It is still an EncodedUnit
	//by inheritance, so you can multiply, divide or raise to the power many units
	template<class ENCODEDUNIT1, class ENCODEDUNIT2>
	struct DividedUnit : public MultipliedUnit<ENCODEDUNIT1, PoweredUnit<ENCODEDUNIT2, -1> >
	{

	};

	//The base SI units, plus a couple of derived ones
	//Note I have gram, which can have exponent, but kilogram
	//cannot - would be ridiculouse to have a millikilogram
	//FOR ALL THESE THE EXPONENT IS NOT PREMULTIPLIED as it is in EncodedUnit - it's as you would write it
	//so for a mm^2 you would use metre<2,-3>

	//This is a macro used to shorthand the definition of functions in each of the SI units
#define NAMEDEF(SHORTNAME, LONGNAME)\
	template<class STRING>\
	static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())\
	{\
		return getShortRepresentation<1,1>(exponentPrefix, exponentSuffix);\
	}\
	template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>\
	static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix)\
	{\
		return unitsPrivate::makeShortName<POWER*EXTRA_POWER_NUMERATOR, EXTRA_POWER_DENOMINATOR, EXPONENT>(getShortName<STRING>(), exponentPrefix, exponentSuffix);\
	}\
	template<class STRING>\
	static STRING getLongRepresentation()\
	{\
		return getLongRepresentation<1,1, STRING>();\
	}\
	template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>\
	static STRING getLongRepresentation()\
	{\
		return unitsPrivate::makeLongName<POWER*EXTRA_POWER_NUMERATOR, EXTRA_POWER_DENOMINATOR, EXPONENT>(getLongName<STRING>());\
	}\
	template<class STRING>\
	static STRING getShortName()\
	{\
		if constexpr (std::is_same<STRING, std::string>::value)\
			return SHORTNAME;\
		if constexpr (std::is_same<STRING, std::wstring>::value)\
			return L##SHORTNAME;\
		if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)\
			return u8##SHORTNAME;\
		if constexpr (std::is_same<STRING, std::u16string>::value)\
			return u##SHORTNAME;\
		if constexpr (std::is_same<STRING, std::u32string>::value)\
			return U##SHORTNAME;\
	}\
	template<class STRING>\
	static STRING getLongName()\
	{\
		if constexpr (std::is_same<STRING, std::string>::value)\
			return LONGNAME;\
		if constexpr (std::is_same<STRING, std::wstring>::value)\
			return L##LONGNAME;\
		if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)\
			return u8##LONGNAME;\
		if constexpr (std::is_same<STRING, std::u16string>::value)\
			return u##LONGNAME;\
		if constexpr (std::is_same<STRING, std::u32string>::value)\
			return U##LONGNAME;\
	}

	//to allow macro expansion of the names before passing them to NAMEDEF where ## is applied
#define ALTERNATE_NAMEDEF(SHORTNAME, LONGNAME) NAMEDEF(SHORTNAME, LONGNAME)

	struct Unitless : public unitsPrivate::EncodedUnitWholePower<0, 0, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = 0;
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			return getShortRepresentation<1,1>(exponentPrefix, exponentSuffix); 
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix)
		{
			return getShortName<STRING>();
		}
		template<class STRING>
		static STRING getLongRepresentation()
		{
			return getLongRepresentation<1,1, STRING>();
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getLongRepresentation()
		{
			return getLongName<STRING>();
		}
		template<class STRING>
		static STRING getShortName()
		{
			return STRING(); 
		}
		template<class STRING>
		static STRING getLongName()
		{
			return STRING();
		}
	};

	struct Percent : public unitsPrivate::EncodedUnitWholePower<0, -2, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = 0;
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			return getShortRepresentation<1,1>(exponentPrefix, exponentSuffix);
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix)
		{
			return getShortName<STRING>();
		}
		template<class STRING>
		static STRING getLongRepresentation()
		{
			return getLongRepresentation<1,1, STRING>();
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getLongRepresentation()
		{
			return getLongName<STRING>();
		}
		template<class STRING>
		static STRING getShortName()
		{
			if constexpr (std::is_same<STRING, std::string>::value)
				return "%"; 
			if constexpr (std::is_same<STRING, std::wstring>::value)
				return L"%"; 
			if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)
				return u8"%";
			if constexpr (std::is_same<STRING, std::u16string>::value)
				return u"%";
			if constexpr (std::is_same<STRING, std::u32string>::value)
				return U"%";
		}
		template<class STRING>
		static STRING getLongName()
		{
			if constexpr (std::is_same<STRING, std::string>::value)
				return "percent";
			if constexpr (std::is_same<STRING, std::wstring>::value)
				return L"percent";
			if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)
				return u8"percent";
			if constexpr (std::is_same<STRING, std::u16string>::value)
				return u"percent";
			if constexpr (std::is_same<STRING, std::u32string>::value)
				return U"percent";
		}
	};

	struct PerMille : public unitsPrivate::EncodedUnitWholePower<0, -3, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = 0;
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			return getShortRepresentation<1,1>(exponentPrefix, exponentSuffix);
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix)
		{
			return getShortName<STRING>();
		}
		template<class STRING>
		static STRING getLongRepresentation()
		{
			return getLongRepresentation<1,1,STRING>();
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getLongRepresentation()
		{
			return getLongName<STRING>();
		}
		template<class STRING>
		static STRING getShortName()
		{
			if constexpr (std::is_same<STRING, std::string>::value)
#ifdef ALTERNATE_PER_MILLE
				return ALTERNATE_PER_MILLE;
#else
				return "\u2030";
#endif
			if constexpr (std::is_same<STRING, std::wstring>::value)
				return L"\u2030";
			if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)
				return u8"\u2030";
			if constexpr (std::is_same<STRING, std::u16string>::value)
				return u"\u2030";
			if constexpr (std::is_same<STRING, std::u32string>::value)
				return U"\u2030";
		}
		template<class STRING>
		static STRING getLongName()
		{
			if constexpr (std::is_same<STRING, std::string>::value)
				return "per mille";
			if constexpr (std::is_same<STRING, std::wstring>::value)
				return L"per mille";
			if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)
				return u8"per mille";
			if constexpr (std::is_same<STRING, std::u16string>::value)
				return u"per mille";
			if constexpr (std::is_same<STRING, std::u32string>::value)
				return U"per mille";
		}
	};

	struct BasisPoint : public unitsPrivate::EncodedUnitWholePower<0, -4, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = 0;
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			return getShortRepresentation<1,1>(exponentPrefix, exponentSuffix);
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix)
		{
			return getShortName<STRING>();
		}
		template<class STRING>
		static STRING getLongRepresentation()
		{
			return getLongRepresentation<1,1,STRING>();
		}
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>
		static STRING getLongRepresentation()
		{
			return getLongName<STRING>();
		}
		template<class STRING>
		static STRING getShortName()
		{
			if constexpr (std::is_same<STRING, std::string>::value)
#ifdef ALTERNATE_BASIS_POINT
				return ALTERNATE_BASIS_POINT;
#else
				return "\u2031";
#endif
			if constexpr (std::is_same<STRING, std::wstring>::value)
				return L"\u2031";
			if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)
				return u8"\u2031";
			if constexpr (std::is_same<STRING, std::u16string>::value)
				return u"\u2031";
			if constexpr (std::is_same<STRING, std::u32string>::value)
				return U"\u2031";
		}
		template<class STRING>
		static STRING getLongName()
		{
			if constexpr (std::is_same<STRING, std::string>::value)
				return "basis point";
			if constexpr (std::is_same<STRING, std::wstring>::value)
				return L"basis point";
			if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)
				return u8"basis point";
			if constexpr (std::is_same<STRING, std::u16string>::value)
				return u"basis point";
			if constexpr (std::is_same<STRING, std::u32string>::value)
				return U"basis point";
		}
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Ampere : public unitsPrivate::EncodedUnitWholePower<unitsPrivate::encodePower<POWER, 0>(), EXPONENT*POWER, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = POWER;
		NAMEDEF("A", "ampere")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Kelvin : public unitsPrivate::EncodedUnitWholePower<unitsPrivate::encodePower<POWER, 1>(), EXPONENT*POWER, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = POWER;
		NAMEDEF("K", "kelvin")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Second : public unitsPrivate::EncodedUnitWholePower<unitsPrivate::encodePower<POWER, 2>(), EXPONENT*POWER, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = POWER;
		NAMEDEF("s", "second")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Metre : public unitsPrivate::EncodedUnitWholePower<unitsPrivate::encodePower<POWER, 3>(), EXPONENT*POWER, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = POWER;
		NAMEDEF("m", "metre")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Gram : public unitsPrivate::EncodedUnitWholePower<unitsPrivate::encodePower<POWER, 4>(), EXPONENT*POWER, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = POWER;
		NAMEDEF("g", "gram")
	};

	template<int8_t POWER = 1>
	struct Kilogram : public Gram<POWER, 3>
	{
		static constexpr int8_t power = POWER;
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Candela : public unitsPrivate::EncodedUnitWholePower<unitsPrivate::encodePower<POWER, 5>(), EXPONENT*POWER, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = POWER;
		NAMEDEF("cd", "candela")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Mole : public unitsPrivate::EncodedUnitWholePower<unitsPrivate::encodePower<POWER, 6>(), EXPONENT*POWER, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = POWER;
		NAMEDEF("mol", "mole")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Radian : public unitsPrivate::EncodedUnitWholePower<unitsPrivate::encodePower<POWER, 7>(), EXPONENT*POWER, false>
	{
		static constexpr bool needsScaling = false;
		static constexpr int8_t power = POWER;
		NAMEDEF("rad", "radian")
	};

	//When defining these derived units we simply pass the exponent to a
	//single linearly dependent base unit and the multiplication by the power will happen at
	//the end


	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Steradian : public MultipliedUnit<Radian<POWER>, Radian<POWER, EXPONENT>> //Note we don't have any linear base units so we split the rd^2 into rd and rd and just pass the exponent through one
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("sr", "steradian")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Hertz : public Second<-1 * POWER, -EXPONENT>//note the negative exponent because 1 mHz= 1ks-1 i.e 1/1ks
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("Hz", "hertz")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Newton : public MultipliedUnit<MultipliedUnit<Kilogram<POWER>, Metre<POWER, EXPONENT>>, Second<-2 * POWER>>
	{
		static constexpr bool needsScaling = MultipliedUnit<MultipliedUnit<Kilogram<POWER>, Metre<POWER, EXPONENT>>, Second<-2 * POWER>>::needsScaling;
		static constexpr int8_t power = POWER;
		NAMEDEF("N", "newton")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Pascal : public MultipliedUnit<Newton<POWER, EXPONENT>, Metre<-2 * POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("Pa", "pascal")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Joule : public MultipliedUnit<Newton<POWER, EXPONENT>, Metre<POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("J", "joule")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Watt : public MultipliedUnit<Joule<POWER, EXPONENT>, Second<-POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("W", "watt")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Coulomb : public MultipliedUnit<Ampere<POWER, EXPONENT>, Second<POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("C", "coulomb")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Volt : public MultipliedUnit<Watt<POWER, EXPONENT>, Ampere<-POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("V", "volt")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Farad : public MultipliedUnit<Coulomb<POWER, EXPONENT>, Volt<-POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("F", "farad")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Ohm : public MultipliedUnit<Volt<POWER, EXPONENT>, Ampere<-POWER>>
	{
		static constexpr int8_t power = POWER;
#ifdef ALTERNATE_OMEGA
		ALTERNATE_NAMEDEF(ALTERNATE_OMEGA, "ohm")
#else
		NAMEDEF("\u2126", "ohm")
#endif
	};


	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Seimens : public Ohm<-1 * POWER, -EXPONENT>//note the negative exponent because 1 mS= 1kOhm-1, i.e 1/1kOhm
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("S", "seimens")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Weber : public MultipliedUnit<Volt<POWER, EXPONENT>, Second<POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("Wb", "weber")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Tesla : public MultipliedUnit<Weber<POWER, EXPONENT>, Metre<-2 * POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("T", "tesla")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Henry : public MultipliedUnit<Weber<POWER, EXPONENT>, Ampere<-POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("H", "henry")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Lumen : public MultipliedUnit<Candela<POWER, EXPONENT>, Steradian<-POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("lm", "lumen")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Lux : public MultipliedUnit<Lumen<POWER, EXPONENT>, Metre<-2 * POWER>>::unit
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("lx", "lux")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Becquerel : public Second<-POWER, EXPONENT>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("Bq", "baquerel")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Gray : public MultipliedUnit<Joule<POWER, EXPONENT>, Kilogram<-POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("Gy", "grey")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Seivert : public MultipliedUnit<Joule<POWER, EXPONENT>, Kilogram<-POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("Sv", "seivert")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Katal : public MultipliedUnit<Mole<POWER, EXPONENT>, Second<-POWER>>
	{
		static constexpr int8_t power = POWER;
		NAMEDEF("kat", "katal")
	};


	//A separate class for other units that are scaled versions of EncodedUnits.
	//Note that these don't derive from EncodedUnits, but they conform to the same
	//template as EncodedUnit, so can be used by Physical as a template parameter.

#define MAKE_SCALED_UNIT(CLASS_NAME, BASE_CLASS, BASE_CLASS_POWER, BASE_TO_SCALED_DIVIDER, SHORTNAME, LONGNAME)\
	template<int8_t POWER = 1, int64_t EXPONENT = 0>\
	class CLASS_NAME\
	{\
	public:\
		typedef BASE_CLASS<POWER*BASE_CLASS_POWER,EXPONENT> baseClass;\
		typedef CLASS_NAME< POWER, EXPONENT > unit;\
		static constexpr bool needsScaling = true;\
		static constexpr uint64_t basePowersNumerators = baseClass::basePowersNumerators;\
		static constexpr uint64_t basePowersDenominators = baseClass::basePowersDenominators;\
		static constexpr int64_t baseExponentNumerator = baseClass::exponentNumerator;\
		static constexpr int64_t baseExponentDenominator = baseClass::exponentDenominator;\
		static constexpr int64_t exponentNumerator = EXPONENT;\
		static constexpr int64_t exponentDenominator = 1;\
		static constexpr int8_t power = POWER;\
		template <class VALUE_TYPE>\
		struct Converter\
		{\
			template <class DESTINATION_UNIT>\
			static constexpr VALUE_TYPE convertTo(VALUE_TYPE value)\
			{\
				static_assert(unitsPrivate::unitCompatible<unit, DESTINATION_UNIT>(), "Cannot convert between units with different powers or dimensions.");\
				if (std::is_same<DESTINATION_UNIT, unit>::value)\
					return value;\
				if (std::is_same<DESTINATION_UNIT, baseClass>::value)\
					return value * VALUE_TYPE(BASE_TO_SCALED_DIVIDER);\
				return DESTINATION_UNIT::template Converter<VALUE_TYPE>::template convertFromBase<baseExponentNumerator, baseExponentDenominator>(convertTo<baseClass>(value));\
			}\
			template <int64_t BASE_EXPONENT_NUMERATOR, int64_t BASE_EXPONENT_DENOMINATOR>\
			static constexpr VALUE_TYPE convertFromBase(VALUE_TYPE value)\
			{\
				if constexpr (unitsPrivate::fractionsEquivalent<exponentNumerator, exponentDenominator, BASE_EXPONENT_NUMERATOR, BASE_EXPONENT_DENOMINATOR>())\
				return value / VALUE_TYPE(BASE_TO_SCALED_DIVIDER);\
				constexpr int64_t NUM = BASE_EXPONENT_NUMERATOR * exponentDenominator - exponentNumerator * BASE_EXPONENT_DENOMINATOR;\
				constexpr int64_t DEN = BASE_EXPONENT_DENOMINATOR * exponentDenominator;\
				if constexpr (NUM % DEN == 0)\
					return value * sci::unitsPrivate::pow10<NUM / DEN, VALUE_TYPE>() / VALUE_TYPE(BASE_TO_SCALED_DIVIDER);\
				return value * std::pow(VALUE_TYPE(10.0), VALUE_TYPE(NUM) / VALUE_TYPE(DEN)) / VALUE_TYPE(BASE_TO_SCALED_DIVIDER);\
			}\
		};\
		template<class STRING>\
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())\
		{\
			return getShortRepresentation<1,1>(exponentPrefix, exponentSuffix);\
		}\
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>\
		static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix)\
		{\
			return unitsPrivate::makeShortName<POWER * EXTRA_POWER_NUMERATOR, EXTRA_POWER_DENOMINATOR, EXPONENT>(getShortName<STRING>(), exponentPrefix, exponentSuffix);\
		}\
		template<class STRING>\
		static STRING getLongRepresentation()\
		{\
			return getLongRepresentation<1,1,STRING>();\
		}\
		template<int8_t EXTRA_POWER_NUMERATOR, int8_t EXTRA_POWER_DENOMINATOR, class STRING>\
		static STRING getLongRepresentation()\
		{\
			return unitsPrivate::makeLongName<POWER * EXTRA_POWER_NUMERATOR, EXTRA_POWER_DENOMINATOR, EXPONENT>(getLongName<STRING>());\
		}\
		template<class STRING>\
		static STRING getShortName()\
		{\
			if constexpr (std::is_same<STRING, std::string>::value)\
				return SHORTNAME;\
			if constexpr (std::is_same<STRING, std::wstring>::value)\
				return L##SHORTNAME;\
			if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)\
				return u8##SHORTNAME;\
			if constexpr (std::is_same<STRING, std::u16string>::value)\
				return u##SHORTNAME;\
			if constexpr (std::is_same<STRING, std::u32string>::value)\
				return U##SHORTNAME;\
		}\
		template<class STRING>\
		static STRING getLongName()\
		{\
			if constexpr (std::is_same<STRING, std::string>::value)\
				return LONGNAME;\
			if constexpr (std::is_same<STRING, std::wstring>::value)\
				return L##LONGNAME;\
			if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)\
				return u8##LONGNAME;\
			if constexpr (std::is_same<STRING, std::u16string>::value)\
				return u##LONGNAME;\
			if constexpr (std::is_same<STRING, std::u32string>::value)\
				return U##LONGNAME;\
		}\
		static constexpr bool isUnitless()\
		{\
			return basePowersNumerators == 0;\
		}\
		template<class OTHER_UNIT>\
		static constexpr bool compatibleWith()\
		{\
			return unitsPrivate::unitCompatible<unit, OTHER_UNIT>();\
		}\
	};

	//needed to avoid ## being applied before alternate shortname is macro expanded
	#define MAKE_ALTERNATE_SCALED_UNIT(CLASS_NAME, BASE_CLASS, BASE_CLASS_POWER, BASE_TO_SCALED_MULTIPLIER, SHORTNAME, LONGNAME) MAKE_SCALED_UNIT(CLASS_NAME, BASE_CLASS, BASE_CLASS_POWER, BASE_TO_SCALED_MULTIPLIER, SHORTNAME, LONGNAME)

	namespace unitsPrivate
	{
#ifdef M_PI
		constexpr double pi = M_PI;
#else 
		constexpr double pi = 3.14159265358979323846;
#endif
	}

	//angle units
#ifdef ALTERNATE_DEGREE
	MAKE_ALTERNATE_SCALED_UNIT(Degree, Radian, 1, unitsPrivate::pi / 180.0 , ALTERNATE_DEGREE, "degree")
#else
	MAKE_SCALED_UNIT(Degree, Radian, 1, unitsPrivate::pi / 180.0, "\u00b0", "degree")
#endif
#ifdef ALTERNATE_ARCMINUTE
	MAKE_ALTERNATE_SCALED_UNIT(ArcMinute, Radian, 1, unitsPrivate::pi / 10800.0 , ALTERNATE_ARCMINUTE, "arcminute")
#else
	MAKE_SCALED_UNIT(ArcMinute, Radian, 1, unitsPrivate::pi / 10800.0, "\u8242", "arcminute")
#endif
#ifdef ALTERNATE_ARCSECOND
		MAKE_ALTERNATE_SCALED_UNIT(ArcSecond, Radian, 1, unitsPrivate::pi / 648000.0, ALTERNATE_ARCSECOND, "arcsecond")
#else
	MAKE_SCALED_UNIT(ArcSecond, Radian, 1, unitsPrivate::pi / 648000.0, "\u8243", "arcsecond")
#endif
	MAKE_SCALED_UNIT(Turn, Radian, 1, unitsPrivate::pi / 0.5, "tr", "turn")
	MAKE_SCALED_UNIT(Quadrant, Radian, 1, unitsPrivate::pi / 2.0, "quadrant", "quadrant")
	MAKE_SCALED_UNIT(Sextant, Radian, 1, unitsPrivate::pi / 3.0, "sextant", "sextant")
	MAKE_SCALED_UNIT(Hexacontade, Radian, 1, unitsPrivate::pi / 30.0, "hexacontade", "hexacontade")
	MAKE_SCALED_UNIT(BinaryDegree, Radian, 1, unitsPrivate::pi / 128.0, "binary degree", "binary degree")
	MAKE_SCALED_UNIT(Gradian, Radian, 1, unitsPrivate::pi / 200.0, "gon", "gradian")

	//Farenheit equivalent of kelvin unit
#ifdef ALTERNATE_RANKINE
	MAKE_ALTERNATE_SCALED_UNIT(Rankine, Kelvin, 1, 1.0 / 1.8, ALTERNATE_RANKINE, "rankine")
#else
	MAKE_SCALED_UNIT(Rankine, Kelvin, 1, 1.0 / 1.8, "\u00b0Ra", "rankine")
#endif

	//alternative metric and scientific units
#ifdef ALTERNATE_ANGSTROM_SHORT
#ifdef ALTERNATE_ANGSTROM_LONG
		MAKE_ALTERNATE_SCALED_UNIT(Angstrom, Metre, 1, 1e-10, ALTERNATE_ANGSTROM_SHORT, ALTERNATE_ANGSTROM_LONG)
#else
		MAKE_ALTERNATE_SCALED_UNIT(Angstrom, Metre, 1, 1e-10, ALTERNATE_ANGSTROM_SHORT, "\u00e5ngstrom")
#endif
#else
#ifdef ALTERNATE_ANGSTROM_LONG
		MAKE_ALTERNATE_SCALED_UNIT(Angstrom, Metre, 1, 1e-10, "\u212B", ALTERNATE_ANGSTROM_LONG)
#else
	MAKE_SCALED_UNIT(Angstrom, Metre, 1, 1e-10, "\u212B", "\u00e5ngstrom")
#endif
#endif
	MAKE_SCALED_UNIT(AstronomicalUnit, Metre, 1, 149597870700.0, "AU", "astronomical unit")
	MAKE_SCALED_UNIT(LightYear, Metre, 1, 9460730472580800.0, "ly", "lightyear")
	MAKE_SCALED_UNIT(Parsec, Metre, 1, 3.085677581e16, "pc", "parsec")
	MAKE_SCALED_UNIT(NauticalMile, Metre, 1, 1852.0, "NM", "nautical mile")
	MAKE_SCALED_UNIT(Hectare, Metre, 2, 1e-4, "ha", "hectare")
	MAKE_SCALED_UNIT(Tonne, Gram, 1, 1e6, "t", "tonne")
	MAKE_SCALED_UNIT(Litre, Metre, 3, 0.001, "L", "litre")
	MAKE_SCALED_UNIT(AtomicMassUnit, Gram, 1, 1.66053904e-24, "AMU", "atomic mass unit")
	MAKE_SCALED_UNIT(SiderealDay, Second, 1, 86164.09053083288, "sidereal day", "sidereal day")
	MAKE_SCALED_UNIT(ElementaryCharge, Coulomb, 1, 1.602176634e-19, "e", "elementary charge")
	MAKE_SCALED_UNIT(Electronvolt, Joule, 1, 1.602176634e-19, "eV", "electron volt")

	//time units
	MAKE_SCALED_UNIT(Minute, Second, 1, 60.0, "min", "minute")
	MAKE_SCALED_UNIT(Hour, Second, 1, 3600.0, "hr", "hour")
	MAKE_SCALED_UNIT(Day, Second, 1, 86400.0, "day", "day")

	//imperial units
	//length units are all based on the international yard which is exactly 0.9144 m
	MAKE_SCALED_UNIT(Mile, Metre, 1, 1760.0 * 0.9144, "mi", "mile")
	MAKE_SCALED_UNIT(Furlong, Metre, 1, 220.0 * 0.9144, "fur", "furlong")
	MAKE_SCALED_UNIT(Chain, Metre, 1, 22.0 * 0.9144, "ch", "chain")
	MAKE_SCALED_UNIT(Rod, Metre, 1, 5.5 * 0.9144, "rd", "rod")
	MAKE_SCALED_UNIT(Fathom, Metre, 1, 2.0 * 0.9144, "ftm", "fathom")
	MAKE_SCALED_UNIT(Yard, Metre, 1, 0.9144, "yd", "yard")
	MAKE_SCALED_UNIT(Foot, Metre, 1, 0.9144 / 3.0, "'", "foot")
	MAKE_SCALED_UNIT(Inch, Metre, 1, 0.9144 / 36.0, "\"", "inch")
	MAKE_SCALED_UNIT(Acre, Metre, 2, 4840 * 0.9144 * 0.9144, "ac", "acre")
	MAKE_SCALED_UNIT(GallonImperial, Metre, 3, 0.00454609, "imp gal", "imperial gallon")
	MAKE_SCALED_UNIT(GallonUs, Metre, 3, 0.9144 * 0.9144 * 0.9144 * 231.0 / 46656.0, "US gal", "US gallon")
	MAKE_SCALED_UNIT(FluidOunceImperial, Metre, 3, 2.84130625e-5, "imp fl oz", "imperial fluid ounce")
	MAKE_SCALED_UNIT(FluidOunceUs, Metre, 3, 0.9144 * 0.9144 * 0.9144 * 231.0 / 46656.0 / 160.0, "US fl oz", "US fluid ounce")
	MAKE_SCALED_UNIT(PintImperial, Metre, 3, 0.00454609 / 8.0, "imp pt", "imperial pint")
	MAKE_SCALED_UNIT(PintUs, Metre, 3, 0.9144 * 0.9144 * 0.9144 * 231.0 / 46656.0 / 8.0, "US pt", "US pint")
	//weight units are defined from the definition of a pound being 453.59237 g
	MAKE_SCALED_UNIT(Ton, Gram, 1, 2240.0 * 453.59237, "t", "ton")
	MAKE_SCALED_UNIT(Hundredweight, Gram, 1, 112.0 * 453.59237, "cwt", "hundredweight")
	MAKE_SCALED_UNIT(Stone, Gram, 1, 14.0 * 453.59237, "st", "stone")
	MAKE_SCALED_UNIT(Pound, Gram, 1, 453.59237, "lb", "pound")
	MAKE_SCALED_UNIT(Ounce, Gram, 1, 453.59237 / 16.0, "oz", "ounce")
	


	template < class ENCODED_UNIT, class VALUE_TYPE>
	class Physical
	{
	public:
		typedef VALUE_TYPE valueType;

#pragma warning(push)
#pragma warning(disable : 26495)
		constexpr Physical() {}
#pragma warning(pop)
		constexpr explicit Physical(VALUE_TYPE v) : m_v(v) {} //explicit so we cannot accidentally create a Physical from a VALUE_TYPE
		template<class U, class V>
		constexpr Physical(const Physical<U, V> &other)
		{
			static_assert(compatibleWith<Physical<U, V>>(), "We can only convert between units with the same dimensions.");
			m_v = (VALUE_TYPE)other.template value<ENCODED_UNIT>();
		}
		template<class U, class V>
		constexpr Physical<ENCODED_UNIT, VALUE_TYPE> &operator=(const Physical<U, V>& other)
		{
			static_assert(compatibleWith<Physical<U, V>>(), "We can only convert between units with the same dimensions.");
			m_v = other.template value<ENCODED_UNIT>();
			return *this;
		}

		//+= operator
		template <class U, class V>
		constexpr Physical<ENCODED_UNIT, VALUE_TYPE> &operator+=(const Physical<U, V> &second)
		{
			static_assert(compatibleWith<Physical<U, V>>(), "We can only add assign units with the same dimensions.");
			m_v += second.template value<ENCODED_UNIT>();
			return *this;
		}

		//-= operator
		template <class U, class V>
		constexpr Physical<ENCODED_UNIT, VALUE_TYPE> &operator-=(const Physical<U, V> &second)
		{
			static_assert(compatibleWith<Physical<U, V>>(), "We can only minus assign units with the same dimensions.");
			m_v -= second.template value<ENCODED_UNIT>();
			return *this;
		}

		// /= operator
		template <class U, class V>
		constexpr Physical<ENCODED_UNIT, VALUE_TYPE> &operator/=(const Physical<U, V> &second)
		{
			static_assert(U::isUnitless(), "Can only divide assign with a dimensionless quantity.");
			m_v /= second.template value<Unitless>();
			return *this;
		}

		//*= operator
		template <class U, class V>
		constexpr Physical<ENCODED_UNIT, VALUE_TYPE> &operator*=(const Physical<U, V> &second)
		{
			static_assert(U::isUnitless(), "Can only multiply assign with a dimensionless quantity.");
			m_v *= second.template value<Unitless>();
			return *this;
		}
		//although trying to set a Physical with a VALUE_TYPE causes a compile error with just
		//the above, the error message is pretty cryptic. This should give something more
		//sensible
		Physical<ENCODED_UNIT, VALUE_TYPE> &operator=(const VALUE_TYPE& other)
		{
			//use std::is_same to ensure we always fail the assert, but avoid a constant false that can be
			//seen by some compilers as a reason to throw an error without the template being instantiated
			static_assert(std::is_same<ENCODED_UNIT, VALUE_TYPE>::value, "Cannot assign a physical value from a raw value type. Use the Physical constructor to create a Physical first.");
		}

		template<class STRING>
		static STRING getShortUnitString(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			return ENCODED_UNIT::getShortRepresentation(exponentPrefix, exponentSuffix);
		}

		template<class STRING>
		static STRING getLongUnitString(const STRING& exponentPrefix = STRING(), const STRING& exponentSuffix = STRING())
		{
			return ENCODED_UNIT:: template getLongRepresentation<STRING>();
		}

		typedef ENCODED_UNIT unit;
		template <class REQUIRED>
		constexpr VALUE_TYPE value() const
		{
			if (std::is_same< REQUIRED, ENCODED_UNIT>::value)
				return m_v;
			if (std::is_same< REQUIRED, Physical<ENCODED_UNIT, VALUE_TYPE>>::value)
				return m_v;
			//this should work whether REQUIRED is a Physical or an Encoded Unit
			return ENCODED_UNIT::template Converter<VALUE_TYPE>::template convertTo<typename REQUIRED::unit>(m_v);
		}

		template<class OTHER>
		static constexpr bool compatibleWith()
		{
			return unitsPrivate::unitCompatible<ENCODED_UNIT, OTHER::unit>();
		}
	private:
		VALUE_TYPE m_v;
	};

	namespace unitsPrivate
	{
		//This is used to get the value of a Physical with a different prefix, e.g. if you have
		//a physical as mm, you can use it to get the value in km
		template<class REQUIRED_UNIT, class IN_PHYSICAL, class OUT_VALUE_TYPE>
		OUT_VALUE_TYPE physicalsToValues(const IN_PHYSICAL &physical, const OUT_VALUE_TYPE &dummy)
		{
			return (OUT_VALUE_TYPE)physical.template value<REQUIRED_UNIT>();
		}
		//This is used to get the value of a vector of Physical with a different prefix, e.g. if you have
		//a vector of Physical as mm, you can use it to get the values in km
		template<class REQUIRED_UNIT, class U, class OUT_VALUE_TYPE>
		auto physicalsToValues(const std::vector<U> &physicals, const OUT_VALUE_TYPE &dummy) -> std::vector<decltype(physicalsToValues<REQUIRED_UNIT>(physicals[0], OUT_VALUE_TYPE()))>
		{
			std::vector<decltype(physicalsToValues<REQUIRED_UNIT>(physicals[0], OUT_VALUE_TYPE()))> result(physicals.size());
			for (size_t i = 0; i < result.size(); ++i)
				result[i] = physicalsToValues<REQUIRED_UNIT>(physicals[i], dummy);
			return result;
		}
	}

	//This is used to get the value of a Physical with a different prefix, e.g. if you have
	//a physical as mm, you can use it to get the value in km
	template<class REQUIRED_UNIT, class IN_PHYSICAL, class OUT_VALUE_TYPE = typename IN_PHYSICAL::valueType>
	OUT_VALUE_TYPE physicalsToValues(const IN_PHYSICAL &physical)
	{
		return (OUT_VALUE_TYPE)physical.template value<REQUIRED_UNIT>();
	}
	//This is used to get the value of a vector of Physical with a different prefix, e.g. if you have
	//a vector of Physical as mm, you can use it to get the values in km
	template<class REQUIRED_UNIT, class U, class OUT_VALUE_TYPE = typename sci::VectorTraits<U>::baseType::valueType>
	auto physicalsToValues(const std::vector<U> &physicals) -> std::vector<decltype(unitsPrivate::physicalsToValues<REQUIRED_UNIT>(physicals[0], OUT_VALUE_TYPE()))>
	{
		std::vector<decltype(unitsPrivate::physicalsToValues<REQUIRED_UNIT>(physicals[0], OUT_VALUE_TYPE()))> result(physicals.size());
		OUT_VALUE_TYPE dummy;
		for (size_t i = 0; i < result.size(); ++i)
			result[i] = unitsPrivate::physicalsToValues<REQUIRED_UNIT>(physicals[i], dummy);
		return result;
	}

	//* operator
	template <class T, class U, class V, class W>
	constexpr Physical<MultipliedUnit<T, U>, typename sci::Promoted<V, W>::type> operator*(const Physical<T, V> &first, const Physical<U, W> &second)
	{
		return Physical<MultipliedUnit<T, U>, typename sci::Promoted<V, W>::type>(first.template value<T>()*second.template value<U>());
	}

	// / operator
	template <class T, class U, class V, class W>
	constexpr Physical<DividedUnit<T, U>, typename sci::Promoted<V, W>::type> operator/(const Physical<T, V> &first, const Physical<U, W> &second)
	{
		return Physical <DividedUnit<T, U>, typename sci::Promoted<V, W>::type>(first.template value<T>() / second.template value<U>());
	}

	//+ operator
	template <class T, class U, class V, class W>
	constexpr Physical<T, typename sci::Promoted<V, W>::type> operator+(const Physical<T, V> &first, const Physical<U, W> &second)
	{
		return Physical<T, typename sci::Promoted<V, W>::type>(first.template value<T>() + second.template value<T>());
	}

	//- operator
	template <class T, class U, class V, class W>
	constexpr Physical<T, typename sci::Promoted<V, W>::type> operator-(const Physical<T, V> &first, const Physical<U, W> &second)
	{
		return Physical<T, typename sci::Promoted<V, W>::type>(first.template value<T>() - second.template value<T>());
	}

	//uniary- operator
	template <class T, class V>
	constexpr Physical<T, V> operator-(const Physical<T, V> &val)
	{
		return Physical<T, V>(-val.template value<T>());
	}

	//> operator
	template <class T, class U, class V, class W>
	constexpr bool operator>(const Physical<T, V> &first, const Physical<U, W> &second)
	{
		return first.template value<T>() > second.template value<T>();
	}

	//< operator
	template <class T, class U, class V, class W>
	constexpr bool operator<(const Physical<T, V> &first, const Physical<U, W> &second)
	{
		return first.template value<T>() < second.template value<T>();
	}

	//== operator
	template <class T, class U, class V, class W>
	constexpr bool operator==(const Physical<T, V> &first, const Physical<U, W> &second)
	{
		return first.template value<T>() == second.template value<T>();
	}

	//!= operator
	template <class T, class U, class V, class W>
	constexpr bool operator!=(const Physical<T, V> &first, const Physical<U, W> &second)
	{
		return first.template value<T>() != second.template value<T>();
	}

	//>= operator
	template <class T, class U, class V, class W>
	constexpr bool operator>=(const Physical<T, V> &first, const Physical<U, W> &second)
	{
		return first.template value<T>() >= second.template value<T>();
	}

	//<= operator
	template <class T, class U, class V, class W>
	constexpr bool operator<=(const Physical<T, V> &first, const Physical<U, W> &second)
	{
		return first.template value<T>() <= second.template value<T>();
	}

	// power - this case deals with dimensionless exponents and bases
	template <class T, class U, class V, class W>
	constexpr Physical<T, typename sci::Promoted<V,W>::type> pow(const Physical<T, V> &base, const Physical<U, W> &power)
	{
		static_assert(U::isUnitless(), "We can only raise a physical value to the power of a dimensionless quantity.");
		static_assert(T::isUnitless(), "We can only raise a physical value to a non-integer power if it is dimensionless, try pow<POWER>(base) instead.");
		//We don't need to play with the units at all and the impact of the exponent of the
		//power is put in the result value - so it has the same exponent as the base
		typedef sci::Promoted<V, W>::type promotedType;
		promotedType powerVal = power.template value<Unitless>();
		promotedType baseVal = base.template value<T>();
		return Physical<T, promotedType>(std::pow(baseVal, powerVal) * std::pow(promotedType(10), promotedType(T::exponentNumerator)/ promotedType(T::exponentDenominator)* (powerVal - promotedType(1.0))));
	}

	// power - this case deals with raising to the power of an integer.
	//Note is has a templated argument otherwise we would not know at
	//compile time what the return type would be.
	template <int POWER, class T, class V>
	constexpr Physical<PoweredUnit<T, POWER>, V> pow(const Physical<T, V> &base)
	{
		static_assert(!T::isUnitless(), "When raising a unitless quantity to a power, please explicitly cast it to sci::Physical<sci::Unitless, VALUE_TYPE>, where VALUE_TYPE is some value type. This ensures the output is not Unitless to some power, which makes no physical sense. You may also use sci::Percent, sci::PerMille and sci::BasisPoint, which will be converted to sci::Unitless for you.");
		return Physical<PoweredUnit<T, POWER>, V>(std::pow(base.template value<T>(), POWER));
	}
	//same but for Unitless - we can't have a Physical<PoweredUnit<Unitless, POWER>>
	template <int POWER, class V>
	constexpr Physical<Unitless, V> pow(const Physical<Unitless, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), POWER));
	}
	//same but for percent
	template <int POWER, class V>
	constexpr Physical<Unitless, V> pow(const Physical<Percent, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), POWER));
	}
	//same but for per mille
	template <int POWER, class V>
	constexpr Physical<Unitless, V> pow(const Physical<PerMille, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), POWER));
	}
	//same but for basis point
	template <int POWER, class V>
	constexpr Physical<Unitless, V> pow(const Physical<BasisPoint, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), POWER));
	}
	

	// root - this case deals with geting an integer root.
	//Note is has a templated argument otherwise we would not know at
	//compile time what the return type would be.
	template <int ROOT, class T, class V>
	constexpr Physical<RootedUnit<T, ROOT>, V> root(const Physical<T, V> &base)
	{
		return Physical<RootedUnit<T, ROOT>, V>(std::pow(base.template value<T>(), V(1.0)/V(ROOT)));
	}
	//root a Unitless - we can't have a Physical<PoweredUnit<Unitless, POWER>>
	template <int ROOT, class V>
	constexpr Physical<Unitless, V> root(const Physical<Unitless, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), V(1.0) / V(ROOT)));
	}
	//root a percent, note we convert to unitless
	template <int ROOT, class V>
	constexpr Physical<Unitless, V> root(const Physical<Percent, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), V(1.0) / V(ROOT)));
	}
	//root a per mille, note we convert to unitless
	template <int ROOT, class V>
	constexpr Physical<Unitless, V> root(const Physical<PerMille, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), V(1.0) / V(ROOT)));
	}
	//root a basis point, note we convert to unitless
	template <int ROOT, class V>
	constexpr Physical<Unitless, V> root(const Physical<BasisPoint, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), V(1.0) / V(ROOT)));
	}
	//sqrt
	template<class T, class V>
	constexpr Physical<RootedUnit<T, 2>, V> sqrt(const Physical<T, V> &base)
	{
		return Physical<RootedUnit<T, 2>, V>(std::sqrt(base.template value<T>()));
	}

	template <class T, class V>
	constexpr Physical<Unitless, V> log(const Physical<T, V> &value)
	{
		static_assert(T::isUnitless(), "We can only log a dimensionless quantity.");
		return Physical<Unitless, V>(std::log(value.template value<Unitless>()));
	}

	template <class T, class V>
	constexpr Physical<Unitless, V> log10(const Physical<T, V> &value)
	{
		static_assert(T::isUnitless(), "We can only log a dimensionless quantity.");
		return Physical<Unitless, V>(std::log10(value.template value<T>()) + T::exponent); // make use of log laws to split this
	}

	template <class T, class V>
	constexpr Physical<Unitless, V> log2(const Physical<T, V> &value)
	{
		static_assert(T::isUnitless(), "We can only log a dimensionless quantity.");
		return Physical<Unitless, V>(std::log2(value.template value<Unitless>()));
	}

	template <class T, class V>
	constexpr Physical<Unitless, V> ln(const Physical<T, V> &value)
	{
		return log(value);
	}

	template <class T, class V>
	constexpr Physical<Unitless, V> exp(const Physical<T, V> &value)
	{
		static_assert(T::isUnitless(), "We can only take the exponent of a dimensionless quantity.");
		return Physical<Unitless, V>(std::exp(value.template value<Unitless>()));
	}


	template <class T, class V>
	constexpr Physical<Radian<>, V> asin(const Physical<T, V> &value)
	{
		static_assert(T::isUnitless(), "We can only asin a dimensionless quantity.");
		return Physical<Radian<>, V>(std::asin(value.template value<Unitless>()));
	}

	template <class T, class V>
	constexpr Physical<Radian<>, V> acos(const Physical<T, V> &value)
	{
		static_assert(T::isUnitless(), "We can only acos a dimensionless quantity.");
		return Physical<Radian<>, V>(std::acos(value.template value<Unitless>()));
	}

	template <class T, class V>
	constexpr Physical<Radian<>, V> atan(const Physical<T, V> &value)
	{
		static_assert(T::isUnitless(), "We can only atan a dimensionless quantity.");
		return Physical<Radian<>, V>(std::atan(value.template value<Unitless>()));
	}

	template <class T, class U, class V, class W>
	Physical<Radian<>, typename sci::Promoted<V, W>::type> atan2(const Physical<T, V> &y, const Physical<U, W> &x)
	{
		static_assert(decltype(y/x)::isUnitless(), "We can only atan a dimensionless quantity.");
		//note that when we get the value we use the unit of y in both cases. This is because
		//although the two values have the same dimension, they may have different exponents
		return Physical<Radian<>, typename sci::Promoted<V, W>::type>(std::atan2(y.template value<T>(), x.template value<T>()));
	}

	template <class T, class V>
	Physical<Radian<>, V> atan2(const Physical<T, V> &y, const Physical<T, V> &x)
	{
		//we need this version with both parameters identical types in case we have 
		// another specialization <template<class T> sci::atan2(const T&y, const T&x)
		return Physical<Radian<>, V>(std::atan2(y.template value<T>(), x.template value<T>()));
	}

	template <class T, class V>
	Physical<Unitless, V> sin(const Physical<T, V> &value)
	{
		return Physical<Unitless, V>(std::sin(value.template value<Radian<>>()));
	}

	template <class T, class V>
	Physical<Unitless, V> cos(const Physical<T, V> &value)
	{
		return Physical<Unitless, V>(std::cos(value.template value<Radian<>>()));
	}

	template <class T, class V>
	Physical<Unitless, V> tan(const Physical<T, V> &value)
	{
		return Physical<Unitless, V>(std::atan(value.template value<Radian<>>()));
	}

	template <class T, class V>
	Physical<T, V> abs(const Physical<T, V> &value)
	{
		return Physical<T, V>(std::abs(value.template value<T>()));
	}

	//floor a Physical - the first template parameter is the unit that
	//you want the Physical to be converted to before doing the floor.
	//The result will be in this unit.
	template< class UNIT, class T, class V>
	Physical<UNIT, V> floor(const Physical<T, V>& value)
	{
		return Physical<UNIT::unit, V>(std::floor(value.template value<UNIT::unit>()));
	}

	//ceil a Physical - the first template parameter is the unit that
	//you want the Physical to be converted to before doing the ceil.
	//The result will be in this unit.
	template< class UNIT, class T, class V>
	Physical<UNIT, V> ceil(const Physical<T, V>& value)
	{
		return Physical<UNIT::unit, V>(std::ceil(value.template value<UNIT::unit>()));
	}


	//declare a TypeTraits for Physicals we can use this to get a unitless type for dividing when averaging
	//in templated functions

	//pre declaration of the struct defined in svector.h
	template<class T>
	struct TypeTraits;

	//the typetraits for Physicals
	template<class T, class V>
	struct TypeTraits<Physical<T, V>>
	{
		typedef Physical<Unitless, V> unitlessType;
		static constexpr unitlessType unitless(size_t v) { return unitlessType(V(v)); }
		static constexpr unitlessType unitless(double v) { return unitlessType(V(v)); }
		static constexpr auto sqrt(const Physical<T, V> &v) ->decltype(sqrt(v)) { return sci::sqrt(v); }
		static const Physical<T, V> unity;
		static const Physical<T, V> zero;
	};
	template<class T, class V>
	const Physical<T, V> TypeTraits<Physical<T, V>>::unity = Physical<T, V>(V(1));
	template<class T, class V>
	const Physical<T, V> TypeTraits<Physical<T, V>>::zero = Physical<T, V>(V(0));

	//This is used by averaging algorithms where simply casting size_t to the
	//same type as the type we are averaging doesn't work
	template <class T, class V>
	Physical<T, V> PhysicalDivide(const Physical<T, V> &numerator, size_t denominator)
	{
		return numerator / Physical<Unitless, V>((V)denominator);
	}

	//this struct can be used to get the unitless type
	//for either a Physical or another value type (e.g.
	//a double or float). This is useful for averaging
	//where we may will need to divide by a unitless
	//count. 

	template<class T>
	struct UnitlessType
	{
		typedef T type;
	};
	template<class T, class U>
	struct UnitlessType<Physical<T, U>>
	{
		typedef Physical<Unitless, U> type;
	};

}

template<class T, class V, class CHAR_TYPE>
std::basic_ostream<CHAR_TYPE>& operator<< (std::basic_ostream<CHAR_TYPE>& stream, const sci::Physical<T, V>& physical)
{
	stream << physical.template value<T>() << " " << T::template getShortRepresentation<std::basic_string<CHAR_TYPE>>();
	return stream;
}

template<class T, class V, class CHAR_TYPE>
std::basic_istream<CHAR_TYPE> & operator>> (std::basic_istream<CHAR_TYPE> &stream, sci::Physical<T, V> &physical)
{
	V temp;
	stream >> temp;
	physical = sci::Physical<T, V>(temp);
	return stream;
}

namespace std
{
	template <class T, class V>
	class numeric_limits<sci::Physical<T, V>> : public numeric_limits<V>
	{
	public:
		static constexpr sci::Physical<T, V>(min)() noexcept
		{
			return sci::Physical<T, V>(numeric_limits<V>::min());
		}

		static constexpr sci::Physical<T, V>(max)() noexcept
		{
			return sci::Physical<T, V>(numeric_limits<V>::max());
		}

		static constexpr sci::Physical<T, V> lowest() noexcept
		{
			return sci::Physical<T, V>(numeric_limits<V>::lowest());
		}

		static constexpr sci::Physical<T, V> epsilon() noexcept
		{
			return sci::Physical<T, V>(numeric_limits<V>::epsilon());
		}

		static constexpr sci::Physical<T, V> round_error() noexcept
		{
			return sci::Physical<T, V>(numeric_limits<V>::round_error());
		}

		static constexpr sci::Physical<T, V> denorm_min() noexcept
		{
			return sci::Physical<T, V>(numeric_limits<V>::denorm_min());
		}

		static constexpr sci::Physical<T, V> infinity() noexcept
		{
			return sci::Physical<T, V>(numeric_limits<V>::infinity());
		}

		static constexpr sci::Physical<T, V> quiet_NaN() noexcept
		{
			return sci::Physical<T, V>(numeric_limits<V>::quiet_NaN());
		}

		static constexpr sci::Physical<T, V> signaling_NaN() noexcept
		{
			return sci::Physical<T, V>(numeric_limits<V>::signaling_NaN());
		}
	};

	template <class T, class V>
	class numeric_limits<const sci::Physical<T, V>> : public numeric_limits<sci::Physical<T, V>>
	{
	};
	template <class T, class V>
	class numeric_limits<volatile sci::Physical<T, V>> : public numeric_limits<sci::Physical<T, V>>
	{
	};
	template <class T, class V>
	class numeric_limits<const volatile sci::Physical<T, V>> : public numeric_limits<sci::Physical<T, V>>
	{
	};

	template <class T, class V>
	inline bool isnan(sci::Physical<T, V> physical)
	{
		return isnan(physical.template value<T>());
	}
	template <class T, class V>
	inline bool isinf(sci::Physical<T, V> physical)
	{
		return isinf(physical.template value<T>());
	}
	template <class T, class V>
	inline bool isfinite(sci::Physical<T, V> physical)
	{
		return isfinite(physical.template value<T>());
	}
	template <class T, class V>
	inline bool isnormal(sci::Physical<T, V> physical)
	{
		return isnormal(physical.template value<T>());
	}
}
