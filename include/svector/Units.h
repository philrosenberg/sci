#pragma once
//Copyright 2020 Philip Rosenberg https://science.cplusplus.engineering
//This source code is provided under the Science.cplusplus.engineering Code license v1.
//A copy of this license should have been provided with this code or can be downloaded
//from https://science.cplusplus.engineering/science-cplusplus-engineering-code-license-v1/

#include<vector>
#include<limits>
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

	template<int64_t EXPONENT>
	constexpr inline bool isMetricExponent()
	{
		return (EXPONENT > -4 && EXPONENT < 4) || (EXPONENT >-25 && EXPONENT < 25 && EXPONENT % 3 == 0 ); 
	} 
	template<int64_t EXPONENT>
	struct ExponentTraits
	{
		const static bool valid = false;
		//we can't just put false in the static_assert. Some compilers see there is no dependence on the
		//template parameter and expand the static_assert even when this template is never instantiated
		static std::string getName() { static_assert(isMetricExponent<EXPONENT>(), "Cannot have an exponent not represented by the standard metric prefixes"); }
		static std::wstring getNameW() { static_assert(isMetricExponent<EXPONENT>(), "Cannot have an exponent not represented by the standard metric prefixes"); }
		static std::basic_string<char8_t> getName8() { static_assert(isMetricExponent<EXPONENT>(), "Cannot have an exponent not represented by the standard metric prefixes"); }
		static std::u16string getName16() { static_assert(isMetricExponent<EXPONENT>(), "Cannot have an exponent not represented by the standard metric prefixes"); }
		static std::u32string getName32() { static_assert(isMetricExponent<EXPONENT>(), "Cannot have an exponent not represented by the standard metric prefixes"); }
		static std::string getPrefix() { static_assert(isMetricExponent<EXPONENT>(), "Cannot have an exponent not represented by the standard metric prefixes"); }
		static std::string getPrefixW() { static_assert(isMetricExponent<EXPONENT>(), "Cannot have an exponent not represented by the standard metric prefixes"); }
		static std::basic_string<char8_t> getPrefix() { static_assert(isMetricExponent<EXPONENT>(), "Cannot have an exponent not represented by the standard metric prefixes"); }
		static std::u16string getPrefix() { static_assert(isMetricExponent<EXPONENT>(), "Cannot have an exponent not represented by the standard metric prefixes"); }
		static std::u32string getPrefix() { static_assert(isMetricExponent<EXPONENT>(), "Cannot have an exponent not represented by the standard metric prefixes"); }
	};

#define MAKE_EXPONENT_TRAITS(VALUE, LONG_NAME, ABBREVIATION)\
template<>\
struct ExponentTraits<VALUE>\
{\
	const static bool valid = true;\
	template<class STRING> static STRING getName(){}\
	template<> static std::string getName<std::string>() { return LONG_NAME; }\
	template<> static std::wstring getName<std::wstring>() { return L##LONG_NAME; }\
	template<> static std::basic_string<char8_t> getName<std::basic_string<char8_t>>() { return u8##LONG_NAME; }\
	template<> static std::u16string getName<std::u16string>() { return u##LONG_NAME; }\
	template<> static std::u32string getName<std::u32string>() { return U#LONG_NAME; }\
	template<class STRING> static STRING getPrefix(){}\
	template<> static std::string getPrefix<std::string>() { return ABBREVIATION; }\
	template<> static std::wstring getPrefix<std::wstring>() { return L##ABBREVIATION; }\
	template<> static std::basic_string<char8_t> getPrefix<std::basic_string<char8_t>>() { return u8##ABBREVIATION; }\
	template<> static std::u16string getPrefix<std::u16string>() { return u##ABBREVIATION; }\
	template<> static std::u32string getPrefix<std::u32string>() { return U##ABBREVIATION; }\
};
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
	MAKE_EXPONENT_TRAITS(-6, "micro", "\u03bc");
	MAKE_EXPONENT_TRAITS(-9, "nano", "n");
	MAKE_EXPONENT_TRAITS(-12, "pico", "p");
	MAKE_EXPONENT_TRAITS(-15, "femto", "f");
	MAKE_EXPONENT_TRAITS(-18, "atto", "a");
	MAKE_EXPONENT_TRAITS(-21, "zepto", "z");
	MAKE_EXPONENT_TRAITS(-24, "yocto", "y");

	template<class STRING>
	STRING powerToTextShort(int8_t power)
	{
		//the basic latin numbers all have the same representation of all sensible encodings
		//so we can use the same function irrespective of the string type
		if (power < 0)
			return STRING({ STRING::value_type(45) }) + powerToTextShort<STRING>(-power);
		if (power < 10)
			return STRING({ STRING::value_type(48 + power) });
		else
			return powerToTextShort<STRING>(power / 10) + STRING({ STRING::value_type(48 + power % 10) });
	}
	template<class STRING>
	STRING powerToTextLong(int8_t power)
	{
		if (power == 2)
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
		if (power == 3)
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
		if constexpr (std::is_same<STRING, std::string>::value)
			return " to the power " + powerToTextShort<STRING>(power);
		if constexpr (std::is_same<STRING, std::wstring>::value)
			return L" to the power " + powerToTextShort<STRING>(power);
		if constexpr (std::is_same<STRING, std::basic_string<char8_t>>::value)
			return u8" to the power " + powerToTextShort<STRING>(power);
		if constexpr (std::is_same<STRING, std::u16string>::value)
			return u" to the power " + powerToTextShort<STRING>(power);
		if constexpr (std::is_same<STRING, std::u32string>::value)
			return U" to the power " + powerToTextShort<STRING>(power);
	}

	//build a string showing the unit. Note that exponent must be before multiplying by power
	template<int64_t EXPONENT, class STRING>
	STRING makeShortName(const STRING &unit, int8_t power, const STRING &powerPrefix, const STRING &powerSuffix)
	{
		if (power == 0)
			return STRING();

		if (power == 1)
			return ExponentTraits<EXPONENT>::template getPrefix<STRING>() + unit;

		return ExponentTraits<EXPONENT>::template getPrefix<STRING>() + unit + powerPrefix + powerToTextShort<STRING>(power) + powerSuffix;
	}

	//build a string showing the unit. Note that exponent must be before multiplying by power
	template<int64_t EXPONENT, class STRING>
	STRING makeLongName(const STRING& unit, int8_t power)
	{
		if (power == 0)
			return STRING();

		if (power == 1)
			return ExponentTraits<EXPONENT>::template getName<STRING>() + unit;
		
		return ExponentTraits<EXPONENT>::template getName<STRING>() + unit + powerToTextLong<STRING>(power);
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


	//This function will reencode the encoded power variables when two units are
	//multiplied. It's not as simple as just adding them as this could result in
	//very wierd overflows
	template<uint64_t FIRST, uint64_t SECOND>
	constexpr uint64_t multiplyPowers()
	{

		return
			encodePower<decodePower<FIRST, 0>() + decodePower<SECOND, 0>(), 0>() |
			encodePower<decodePower<FIRST, 1>() + decodePower<SECOND, 1>(), 1>() |
			encodePower<decodePower<FIRST, 2>() + decodePower<SECOND, 2>(), 2>() |
			encodePower<decodePower<FIRST, 3>() + decodePower<SECOND, 3>(), 3>() |
			encodePower<decodePower<FIRST, 4>() + decodePower<SECOND, 4>(), 4>() |
			encodePower<decodePower<FIRST, 5>() + decodePower<SECOND, 5>(), 5>() |
			encodePower<decodePower<FIRST, 6>() + decodePower<SECOND, 6>(), 6>() |
			encodePower<decodePower<FIRST, 7>() + decodePower<SECOND, 7>(), 7>();
	}

	template<uint64_t FIRST, uint64_t SECOND>
	constexpr uint64_t dividePowers()
	{

		return
			encodePower<decodePower<FIRST, 0>() - decodePower<SECOND, 0>(), 0>() |
			encodePower<decodePower<FIRST, 1>() - decodePower<SECOND, 1>(), 1>() |
			encodePower<decodePower<FIRST, 2>() - decodePower<SECOND, 2>(), 2>() |
			encodePower<decodePower<FIRST, 3>() - decodePower<SECOND, 3>(), 3>() |
			encodePower<decodePower<FIRST, 4>() - decodePower<SECOND, 4>(), 4>() |
			encodePower<decodePower<FIRST, 5>() - decodePower<SECOND, 5>(), 5>() |
			encodePower<decodePower<FIRST, 6>() - decodePower<SECOND, 6>(), 6>() |
			encodePower<decodePower<FIRST, 7>() - decodePower<SECOND, 7>(), 7>();
	}

	template<uint64_t VALUE, int POW>
	constexpr uint64_t powPowers()
	{

		return
			encodePower<decodePower<VALUE, 0>() * POW, 0>() |
			encodePower<decodePower<VALUE, 1>() * POW, 1>() |
			encodePower<decodePower<VALUE, 2>() * POW, 2>() |
			encodePower<decodePower<VALUE, 3>() * POW, 3>() |
			encodePower<decodePower<VALUE, 4>() * POW, 4>() |
			encodePower<decodePower<VALUE, 5>() * POW, 5>() |
			encodePower<decodePower<VALUE, 6>() * POW, 6>() |
			encodePower<decodePower<VALUE, 7>() * POW, 7>();
	}

	template<uint64_t VALUE, int ROOT>
	constexpr uint64_t rootPowers()
	{
		static_assert(decodePower<VALUE, 0>() % ROOT == 0, "When rooting a unit all dimensions must have powers that are multiples of the root");
		static_assert(decodePower<VALUE, 1>() % ROOT == 0, "When rooting a unit all dimensions must have powers that are multiples of the root");
		static_assert(decodePower<VALUE, 2>() % ROOT == 0, "When rooting a unit all dimensions must have powers that are multiples of the root");
		static_assert(decodePower<VALUE, 3>() % ROOT == 0, "When rooting a unit all dimensions must have powers that are multiples of the root");
		static_assert(decodePower<VALUE, 4>() % ROOT == 0, "When rooting a unit all dimensions must have powers that are multiples of the root");
		static_assert(decodePower<VALUE, 5>() % ROOT == 0, "When rooting a unit all dimensions must have powers that are multiples of the root");
		static_assert(decodePower<VALUE, 6>() % ROOT == 0, "When rooting a unit all dimensions must have powers that are multiples of the root");
		static_assert(decodePower<VALUE, 7>() % ROOT == 0, "When rooting a unit all dimensions must have powers that are multiples of the root");
		return
			encodePower<decodePower<VALUE, 0>() / ROOT, 0>() |
			encodePower<decodePower<VALUE, 1>() / ROOT, 1>() |
			encodePower<decodePower<VALUE, 2>() / ROOT, 2>() |
			encodePower<decodePower<VALUE, 3>() / ROOT, 3>() |
			encodePower<decodePower<VALUE, 4>() / ROOT, 4>() |
			encodePower<decodePower<VALUE, 5>() / ROOT, 5>() |
			encodePower<decodePower<VALUE, 6>() / ROOT, 6>() |
			encodePower<decodePower<VALUE, 7>() / ROOT, 7>();
	}

	//These enable raising 10 to the power zero get
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
		if(pow > 0)
			return temp * temp * 10;
		return temp * temp * 0.1;
	}
	template <int64_t pow, class VALUE_TYPE>
	constexpr VALUE_TYPE pow10()
	{
		if (pow == 0)
			return VALUE_TYPE(1);
		return sci::pow10<VALUE_TYPE>(pow);
	}

	//This is the basic class that represents a unit. It takes two templated parameters
	//The first is the powers. This is an encoded 64 bit number where the first byte is
	//the power in the first dimension, the second byte is the power in the second
	//dimension, etc.
	//The second is the exponent the unit would have ONCE THE POWER HAS BEEN MULTIPLIED OUT.
	//For example for a millimetre it would be -3, for a millimetre squared it would be -6, etc
	template<uint64_t BASE_POWERS, int64_t EXPONENT>
	struct EncodedUnit
	{
		static const uint64_t basePowers = BASE_POWERS;
		static const int64_t exponent = EXPONENT;
		typedef EncodedUnit< basePowers, exponent > unit;
		typedef unit baseClass;

		template<class VALUE_TYPE>
		struct Converter //We have to put all these functions into a struct to avoid haveing to partial specialize them
		{
			template <class T>
			static VALUE_TYPE convertTo(VALUE_TYPE value)
			{
				static_assert(basePowers == T::unit::basePowers, "Cannot convert between units with different powers or dimensions.");
				//At this point we know both base powers are the same, but we don't know if the from
				//unit is a derived type. However, it's base type must be this, so we can call its
				//convertFromBase method to to do the actual conversion.
				if (std::is_same<T, unit>::value)
					return value;
				return T::template Converter<VALUE_TYPE>::template convertFromBase<EXPONENT>(value);
			}
			//template <>
			//static VALUE_TYPE convertTo <EncodedUnit<BASE_POWERS, EXPONENT>>(VALUE_TYPE value)
			//{
			//	return value;
			//}
			template <int64_t BASE_EXPONENT>
			static VALUE_TYPE convertFromBase(VALUE_TYPE value)
			{
				//for this type its base is itself, but with a different exponent
				if (EXPONENT == BASE_EXPONENT)
					return value;
				return value * sci::pow10<BASE_EXPONENT - EXPONENT, VALUE_TYPE>();
			}
			//template <>
			//static VALUE_TYPE convertFromBase<EXPONENT>(VALUE_TYPE value)
			//{
			//	//as above, but now the exponents are identical so no calculation is needed
			//	return value;
			//}
		};
		static constexpr bool isUnitless()
		{
			return basePowers == 0;
		}
		template<class OTHER_UNIT>
		static constexpr bool compatibleWith()
		{
			return basePowers == OTHER_UNIT::basePowers;
		}
	};

	//This is a class that represents an encoded unit to a power. It is still an EncodedUnit
	//by inheritance, so you can multiply, divide or raise to the power many units
	template<class ENCODEDUNIT, int POW>
	struct PoweredUnit : public EncodedUnit<powPowers<ENCODEDUNIT::basePowers, POW>(), ENCODEDUNIT::exponent * POW>
	{
		typedef PoweredUnit< ENCODEDUNIT, POW> unit;
		typedef EncodedUnit<powPowers<ENCODEDUNIT::basePowers, POW>(), ENCODEDUNIT::exponent * POW> encodedUnitClass;
		template<class STRING>
		static STRING getShortRepresentation(const STRING& exponentPrefix = STRING(), const STRING& exponentSuffix = STRING())
		{
			return ENCODEDUNIT::getShortRepresentation(exponentPrefix, exponentSuffix, POW);
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix = STRING(), const STRING& exponentSuffix = STRING())
		{
			return ENCODEDUNIT::getLongRepresentation(exponentPrefix, exponentSuffix, POW);
		}
		template<class VALUE_TYPE>
		struct Converter //We have to put all these functions into a struct to avoid haveing to partial specialize them
		{
			template <class T>
			static VALUE_TYPE convertTo(VALUE_TYPE value)
			{
				static_assert(encodedUnitClass::basePowers == T::unit::basePowers, "Cannot convert between units with different powers or dimensions.");
				//At this point we know both base powers are the same, but we don't know if the from
				//unit is a derived type. However, it's base type must be this, so we can call its
				//convertFromBase method to to do the actual conversion.
				if (std::is_same<T, unit>::value)
					return value;
				return T::template Converter<VALUE_TYPE>::template convertFromBase<encodedUnitClass::exponent>(value*std::pow(ENCODEDUNIT:: template Converter<VALUE_TYPE>::template convertTo<typename ENCODEDUNIT::baseClass>(VALUE_TYPE(1.0)), POW));
			}
			template <int64_t BASE_EXPONENT>
			static VALUE_TYPE convertFromBase(VALUE_TYPE value)
			{
				return value * std::pow(ENCODEDUNIT::template Converter<VALUE_TYPE>::template convertFromBase<0>(VALUE_TYPE(1.0)), POW)*sci::pow10<(BASE_EXPONENT - encodedUnitClass::exponent), VALUE_TYPE>();
			}
		};
	};

	//This is a class that represents an encoded unit rooted. It is still an EncodedUnit
	//by inheritance, so you can multiply, divide or raise to the power many units
	template<class ENCODEDUNIT, int ROOT>
	struct RootedUnit : public EncodedUnit<rootPowers<ENCODEDUNIT::basePowers, ROOT>(), ENCODEDUNIT::exponent * ROOT>
	{
		typedef RootedUnit< ENCODEDUNIT, ROOT> unit;
		typedef EncodedUnit<rootPowers<ENCODEDUNIT::basePowers, ROOT>(), ENCODEDUNIT::exponent * ROOT> encodedUnitClass;
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			return ENCODEDUNIT::getShortRepresentation(exponentPrefix, exponentSuffix, -ROOT);
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix = STRING, const STRING& exponentSuffix = STRING)
		{
			return ENCODEDUNIT::getLongRepresentation(exponentPrefix, exponentSuffix, -ROOT);
		}
		template<class VALUE_TYPE>
		struct Converter //We have to put all these functions into a struct to avoid haveing to partial specialize them
		{
			template <class T>
			static VALUE_TYPE convertTo(VALUE_TYPE value)
			{
				static_assert(encodedUnitClass::basePowers == T::unit::basePowers, "Cannot convert between units with different powers or dimensions.");
				//At this point we know both base powers are the same, but we don't know if the from
				//unit is a derived type. However, it's base type must be this, so we can call its
				//convertFromBase method to to do the actual conversion.
				if (std::is_same<T, unit>::value)
					return value;
				return T::template Converter<VALUE_TYPE>::template convertFromBase<encodedUnitClass::exponent>(value*std::pow(ENCODEDUNIT::template Converter<VALUE_TYPE>::template convertTo<typename ENCODEDUNIT::baseClass>(VALUE_TYPE(1.0)), VALUE_TYPE(1.0) / VALUE_TYPE(ROOT)));
			}
			template <int64_t BASE_EXPONENT>
			static VALUE_TYPE convertFromBase(VALUE_TYPE value)
			{
				return value * std::pow(ENCODEDUNIT::template Converter<VALUE_TYPE>::convertFromBase<0>(VALUE_TYPE(1.0)), VALUE_TYPE(1.0) / VALUE_TYPE(ROOT))*sci::pow10<(BASE_EXPONENT - encodedUnitClass::exponent), VALUE_TYPE>();
			}
		};
	};

	//This is a class that represents two encoded units multiplied together. It is still an EncodedUnit
	//by inheritance, so you can multiply, divide or raise to the power many units.
	template<class ENCODEDUNIT1, class ENCODEDUNIT2>
	struct MultipliedUnit : public EncodedUnit<multiplyPowers<ENCODEDUNIT1::basePowers, ENCODEDUNIT2::basePowers>(), ENCODEDUNIT1::exponent + ENCODEDUNIT2::exponent>
	{
		typedef MultipliedUnit< ENCODEDUNIT1, ENCODEDUNIT2> unit;
		typedef EncodedUnit<multiplyPowers<ENCODEDUNIT1::basePowers, ENCODEDUNIT2::basePowers>(), ENCODEDUNIT1::exponent + ENCODEDUNIT2::exponent> encodedUnitClass;
		typedef MultipliedUnit<typename ENCODEDUNIT1::baseClass, typename ENCODEDUNIT2::baseClass> baseClass;
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			//for all sensible encodings a space is represented by value of 32, so we don't need to do any encoding conversion
			return ENCODEDUNIT1::getShortRepresentation(exponentPrefix, exponentSuffix) + STRING{STRING::value_type(32)} + ENCODEDUNIT2::getShortRepresentation(exponentPrefix, exponentSuffix);
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix = STRING(), const STRING& exponentSuffix = STRING())
		{
			//for all sensible encodings a space is represented by value of 32, so we don't need to do any encoding conversion
			return ENCODEDUNIT1::getLongRepresentation(exponentPrefix, exponentSuffix) + STRING{ STRING::value_type(32) } + ENCODEDUNIT2::getLongRepresentation(exponentPrefix, exponentSuffix);
		}

		template<class VALUE_TYPE>
		struct Converter //We have to put all these functions into a struct to avoid haveing to partial specialize them
		{
			template <class T>
			static VALUE_TYPE convertTo(VALUE_TYPE value)
			{
				static_assert(encodedUnitClass::basePowers == T::unit::basePowers, "Cannot convert between units with different powers or dimensions.");

				if (std::is_same<T, unit>::value)
					return value;
				//Convert the value to the ENCODEDUNIT1::baseType and multiple by 1 converted to ENCODEDUNIT2::baseType, then call convertFromBase
				return T::template Converter<VALUE_TYPE>::template convertFromBase<encodedUnitClass::exponent>(ENCODEDUNIT1::template Converter<VALUE_TYPE>::template convertTo<typename ENCODEDUNIT1::baseClass>(value)*ENCODEDUNIT2::template Converter<VALUE_TYPE>::template convertTo<typename ENCODEDUNIT2::baseClass>(1.0));
			}
			template <int64_t BASE_EXPONENT>
			static VALUE_TYPE convertFromBase(VALUE_TYPE value)
			{
				//Use each encodedUnit's convert from base function. The first one uses the exponent and the value, the second uses 1
				return ENCODEDUNIT1::template Converter<VALUE_TYPE>::template convertFromBase<BASE_EXPONENT>(value)*ENCODEDUNIT2::template Converter<VALUE_TYPE>::template convertFromBase<0>(1.0);
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
		return getShortRepresentation(exponentPrefix, exponentSuffix, 1);\
	}\
	template<class STRING>\
	static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix, int raisedByPower)\
	{\
		return makeShortName<EXPONENT>(getShortName<STRING>(), POWER*raisedByPower, exponentPrefix, exponentSuffix);\
	}\
	template<class STRING>\
	static STRING getLongRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())\
	{\
		return getLongRepresentation(exponentPrefix, exponentSuffix, 1);\
	}\
	template<class STRING>\
	static STRING getLongRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix, int raisedByPower)\
	{\
		return makeLongName<EXPONENT>(getLongName<STRING>(), POWER*raisedByPower);\
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

	struct Unitless : public EncodedUnit<0, 0>
	{
		static const int8_t power = 0;
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			return getShortRepresentation(exponentPrefix, exponentSuffix, 1); 
		}
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix, int raisedByPower)
		{
			return getShortName<STRING>();
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix = STRING(), const STRING& exponentSuffix = STRING())
		{
			return getLongRepresentation(exponentPrefix, exponentSuffix, 1);
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix, const STRING& exponentSuffix, int raisedByPower)
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

	struct Percent : public EncodedUnit<0, -2>
	{
		static const int8_t power = 0;
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			return getShortRepresentation(exponentPrefix, exponentSuffix, 1);
		}
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix, int raisedByPower)
		{
			return getShortName<STRING>();
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix = STRING(), const STRING& exponentSuffix = STRING())
		{
			return getLongRepresentation(exponentPrefix, exponentSuffix, 1);
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix, const STRING& exponentSuffix, int raisedByPower)
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

	struct PerMille : public EncodedUnit<0, -3>
	{
		static const int8_t power = 0;
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			return getShortRepresentation(exponentPrefix, exponentSuffix, 1);
		}
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix, int raisedByPower)
		{
			return getShortName<STRING>();
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix = STRING(), const STRING& exponentSuffix = STRING())
		{
			return getLongRepresentation(exponentPrefix, exponentSuffix, 1);
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix, const STRING& exponentSuffix, int raisedByPower)
		{
			return getLongName<STRING>();
		}
		template<class STRING>
		static STRING getShortName()
		{
			if constexpr (std::is_same<STRING, std::string>::value)
				return "\u2030";
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

	struct BasisPoint : public EncodedUnit<0, -4>
	{
		static const int8_t power = 0;
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())
		{
			return getShortRepresentation(exponentPrefix, exponentSuffix, 1);
		}
		template<class STRING>
		static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix, int raisedByPower)
		{
			return getShortName<STRING>();
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix = STRING(), const STRING& exponentSuffix = STRING())
		{
			return getLongRepresentation(exponentPrefix, exponentSuffix, 1);
		}
		template<class STRING>
		static STRING getLongRepresentation(const STRING& exponentPrefix, const STRING& exponentSuffix, int raisedByPower)
		{
			return getLongName<STRING>();
		}
		template<class STRING>
		static STRING getShortName()
		{
			if constexpr (std::is_same<STRING, std::string>::value)
				return "\u2031";
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
	struct Amp : public EncodedUnit<encodePower<POWER, 0>(), EXPONENT*POWER>
	{
		static const int8_t power = POWER;
		NAMEDEF("A", "ampere")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Kelvin : public EncodedUnit<encodePower<POWER, 1>(), EXPONENT*POWER>
	{
		static const int8_t power = POWER;
		NAMEDEF("K", "kelvin")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Second : public EncodedUnit<encodePower<POWER, 2>(), EXPONENT*POWER>
	{
		static const int8_t power = POWER;
		NAMEDEF("s", "second")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Metre : public EncodedUnit<encodePower<POWER, 3>(), EXPONENT*POWER>
	{
		static const int8_t power = POWER;
		NAMEDEF("m", "metre")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Gram : public EncodedUnit<encodePower<POWER, 4>(), EXPONENT*POWER>
	{
		static const int8_t power = POWER;
		NAMEDEF("g", "gram")
	};

	template<int8_t POWER = 1>
	struct Kilogram : public Gram<POWER, 3>
	{
		static const int8_t power = POWER;
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Candela : public EncodedUnit<encodePower<POWER, 5>(), EXPONENT*POWER>
	{
		static const int8_t power = POWER;
		NAMEDEF("cd", "candela")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Mole : public EncodedUnit<encodePower<POWER, 6>(), EXPONENT*POWER>
	{
		static const int8_t power = POWER;
		NAMEDEF("mol", "mole")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Radian : public EncodedUnit<encodePower<POWER, 7>(), EXPONENT*POWER>
	{
		static const int8_t power = POWER;
		NAMEDEF("rad", "radian")
	};

	//When defining these derived units we simply pass the exponent to a
	//single linearly dependent base unit and the multiplication by the power will happen at
	//the end


	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Steradian : public MultipliedUnit<Radian<POWER>, Radian<POWER, EXPONENT>> //Note we don't have any linear base units so we split the rd^2 into rd and rd and just pass the exponent through one
	{
		static const int8_t power = POWER;
		NAMEDEF("sr", "steradian")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Hertz : public Second<-1 * POWER, -EXPONENT>//note the negative exponent because 1 mHz= 1ks-1 i.e 1/1ks
	{
		static const int8_t power = POWER;
		NAMEDEF("Hz", "hertz")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Newton : public MultipliedUnit<MultipliedUnit<Kilogram<POWER>, Metre<POWER, EXPONENT>>, Second<-2 * POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("N", "newton")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Pascal : public MultipliedUnit<Newton<POWER, EXPONENT>, Metre<-2 * POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("Pa", "pascal")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Joule : public MultipliedUnit<Newton<POWER, EXPONENT>, Metre<POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("J", "joule")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Watt : public MultipliedUnit<Joule<POWER, EXPONENT>, Second<-POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("W", "watt")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Coulomb : public MultipliedUnit<Amp<POWER, EXPONENT>, Second<POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("C", "coulomb")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Volt : public MultipliedUnit<Watt<POWER, EXPONENT>, Amp<-POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("V", "volt")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Farad : public MultipliedUnit<Coulomb<POWER, EXPONENT>, Volt<-POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("F", "farad")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Ohm : public MultipliedUnit<Volt<POWER, EXPONENT>, Amp<-POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("\u2126", "ohm")
	};


	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Seimens : public Ohm<-1 * POWER, -EXPONENT>//note the negative exponent because 1 mS= 1kOhm-1, i.e 1/1kOhm
	{
		static const int8_t power = POWER;
		NAMEDEF("S", "seimens")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Weber : public MultipliedUnit<Volt<POWER, EXPONENT>, Second<POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("Wb", "weber")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Tesla : public MultipliedUnit<Weber<POWER, EXPONENT>, Metre<-2 * POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("T", "tesla")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Henry : public MultipliedUnit<Weber<POWER, EXPONENT>, Amp<-POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("H", "henry")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Lumen : public MultipliedUnit<Candela<POWER, EXPONENT>, Steradian<-POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("lm", "lumen")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Lux : public MultipliedUnit<Lumen<POWER, EXPONENT>, Metre<-2 * POWER>>::unit
	{
		static const int8_t power = POWER;
		NAMEDEF("lx", "lux")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Becquerel : public Second<-POWER, EXPONENT>
	{
		static const int8_t power = POWER;
		NAMEDEF("Bq", "baquerel")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Gray : public MultipliedUnit<Joule<POWER, EXPONENT>, Kilogram<-POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("Gy", "grey")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Seivert : public MultipliedUnit<Joule<POWER, EXPONENT>, Kilogram<-POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("Sv", "seivert")
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Katal : public MultipliedUnit<Mole<POWER, EXPONENT>, Second<-POWER>>
	{
		static const int8_t power = POWER;
		NAMEDEF("kat", "katal")
	};


	//A separate class for other units that are scaled versions of EncodedUnits.
	//Note that these don't derive from EncodedUnits, but they conform to the same
	//template as EncodedUnit, so can be used by Physical as a template parameter.
/*#define MAKE_SCALED_UNIT(CLASS_NAME, BASE_NAME, BASE_TO_SCALED_MULTIPLIER, SHORT_NAME)\
	template<class BASE_NAME, int8_t POWER = 1, int64_t EXPONENT = 0>\
	class CLASS_NAME\
	{\
	public:\
		static const uint64_t basePowers = BASE_NAME::basePowers;\
		static const int64_t exponent = BASE_NAME::exponent;\
		typedef CLASS_NAME< BASE_NAME, POWER, EXPONENT > unit;\
		template <class T>\
		static double convertTo(double value)\
		{\
			static_assert(basePowers == T::basePowers, "Cannot convert between units with different powers or dimensions.");\
			return T::convertFromBase<EXPONENT>(convertTo<BASE_NAME<POWER, EXPONENT>>(value));\
		}\
		template <>\
		static double convertTo <CLASS_NAME<BASE_NAME, POWER, EXPONENT>>(double value)\
		{\
			return value;\
		}\
		template <>\
		static double convertTo <BASE_NAME<POWER, EXPONENT>>(double value)\
		{\
			return value / m_baseToScaledMultiplier;\
		}\
		template <int64_t BASE_EXPONENT>\
		static double convertFromBase(double value)\
		{\
			return value * m_baseToScaledMultiplier * sci::pow10<BASE_EXPONENT - EXPONENT>();\
		}\
		static const double m_baseToScaledMultiplier;\
	};\
	template <int8_t POWER, int64_t EXPONENT>\
	const double CLASS_NAME<BASE_NAME, POWER, EXPONENT>::m_baseToScaledMultiplier = std::pow(BASE_TO_SCALED_MULTIPLIER, POWER);*/
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define MAKE_SCALED_UNIT(CLASS_NAME, BASE_CLASS, BASE_CLASS_POWER, BASE_TO_SCALED_MULTIPLIER, SHORTNAME, LONGNAME)\
	template<int8_t POWER = 1, int64_t EXPONENT = 0>\
	class CLASS_NAME\
	{\
	public:\
		typedef BASE_CLASS<POWER*BASE_CLASS_POWER,EXPONENT> baseClass;\
		typedef CLASS_NAME< POWER, EXPONENT > unit;\
		static const uint64_t basePowers = baseClass::basePowers;\
		static const int64_t baseExponent = baseClass::exponent;\
		static const int64_t exponent = EXPONENT;\
		static const int8_t power = POWER;\
		template <class VALUE_TYPE>\
		struct Converter\
		{\
			template <class T>\
			static VALUE_TYPE convertTo(VALUE_TYPE value)\
			{\
				static_assert(basePowers == T::basePowers, "Cannot convert between units with different powers or dimensions.");\
				if (std::is_same<T, unit>::value)\
					return value;\
				if (std::is_same<T, baseClass>::value)\
					return value / VALUE_TYPE(BASE_TO_SCALED_MULTIPLIER);\
				return T::template Converter<VALUE_TYPE>::template convertFromBase<baseExponent>(convertTo<baseClass>(value));\
			}\
			template <int64_t BASE_EXPONENT>\
			static VALUE_TYPE convertFromBase(VALUE_TYPE value)\
			{\
				return value * sci::pow10<(BASE_EXPONENT - baseExponent), VALUE_TYPE>() * VALUE_TYPE(BASE_TO_SCALED_MULTIPLIER);\
			}\
		};\
		template<class STRING>\
		static STRING getShortRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())\
		{\
			return getShortRepresentation(exponentPrefix, exponentSuffix, 1);\
		}\
		template<class STRING>\
		static STRING getShortRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix, int raisedByPower)\
		{\
			return makeShortName<EXPONENT>(getShortName<STRING>(), POWER*raisedByPower, exponentPrefix, exponentSuffix);\
		}\
		template<class STRING>\
		static STRING getLongRepresentation(const STRING &exponentPrefix = STRING(), const STRING &exponentSuffix = STRING())\
		{\
			return getLongRepresentation(exponentPrefix, exponentSuffix, 1);\
		}\
		template<class STRING>\
		static STRING getLongRepresentation(const STRING &exponentPrefix, const STRING &exponentSuffix, int raisedByPower)\
		{\
			return makeLongName<EXPONENT>(getLongName<STRING>(), POWER*raisedByPower);\
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
			return basePowers == 0;\
		}\
		template<class OTHER_UNIT>\
		static constexpr bool compatibleWith()\
		{\
			return basePowers == OTHER_UNIT::basePowers;\
		}\
	};


	//angle units
	MAKE_SCALED_UNIT(Degree, Radian, 1, 180.0 / M_PI, "degree", "degree")
	MAKE_SCALED_UNIT(ArcMinute, Radian, 1, 10800.0 / M_PI, "\u8242", "arcminute")
	MAKE_SCALED_UNIT(ArcSecond, Radian, 1, 648000.0 / M_PI, "\u8243", "arcsecond")
	MAKE_SCALED_UNIT(Turn, Radian, 1, 0.5 / M_PI, "tr", "turn")
	MAKE_SCALED_UNIT(Quadrant, Radian, 1, 2.0 / M_PI, "quadrant", "quadrant")
	MAKE_SCALED_UNIT(Sextant, Radian, 1, 3.0 / M_PI, "sextant", "sextant")
	MAKE_SCALED_UNIT(Hexacontade, Radian, 1, 30.0 / M_PI, "hexacontade", "hexacontade")
	MAKE_SCALED_UNIT(BinaryDegree, Radian, 1, 128.0 / M_PI, "binary degree", "binary degree")
	MAKE_SCALED_UNIT(Gradian, Radian, 1, 200.0 / M_PI, "gon", "gradian")

	//Farenheit equivalent of kelvin unit
	MAKE_SCALED_UNIT(Rankine, Kelvin, 1, 0.55, "\u00b0Ra", "rankine")

	//alternative metric and scientific units
	MAKE_SCALED_UNIT(Angstrom, Metre, 1, 1e-10, "\u212B", "\u00e5ngstrom")
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
	MAKE_SCALED_UNIT(Minute, Second, 1, 1.0 / 60.0, "min", "minute")
	MAKE_SCALED_UNIT(Hour, Second, 1, 1.0 / 3600.0, "hr", "hour")
	MAKE_SCALED_UNIT(Day, Second, 1, 1.0 / 86400.0, "day", "day")

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
			return ENCODED_UNIT::getLongRepresentation(exponentPrefix, exponentSuffix);
		}

		//static const uint64_t basePowers = ENCODED_UNIT::basePowers;
		//static const int64_t exponent = ENCODED_UNIT::exponent;
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
			return ENCODED_UNIT::basePowers == OTHER::unit::basePowers;
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
		return Physical<T, promotedType>(std::pow(baseVal, powerVal)*std::pow(promotedType(10), T::exponent*(powerVal - promotedType(1.0))));
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
	//This version undoes a powered unit
	template <int ROOT, class T, class V>
	constexpr Physical<T, V> root(const Physical<PoweredUnit<T, ROOT>, V> &base)
	{
		return Physical<T, V>(std::pow(base.template value<PoweredUnit<T, ROOT>>(), V(1.0) / V(ROOT)));
	}
	//same but for Unitless - we can't have a Physical<PoweredUnit<Unitless, POWER>>
	template <int ROOT, class V>
	constexpr Physical<Unitless, V> root(const Physical<Unitless, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), V(1.0)/V(ROOT)));
	}
	//same but for percent
	template <int ROOT, class V>
	constexpr Physical<Unitless, V> root(const Physical<Percent, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), V(1.0) / V(ROOT)));
	}
	//same but for per mille
	template <int ROOT, class V>
	constexpr Physical<Unitless, V> root(const Physical<PerMille, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), V(1.0) / V(ROOT)));
	}
	//same but for basis point
	template <int ROOT, class V>
	constexpr Physical<Unitless, V> root(const Physical<BasisPoint, V> &base)
	{
		return Physical<Unitless, V>(std::pow(base.template value<Unitless>(), V(1.0) / V(ROOT)));
	}
	//sqrt - uses root<2> function
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
		//we need this version with both parameters identical types in order to specialize
		//<template<class T> sci::atan2(const T&y, const T&x) in svector.h. Otherwise
		//the Physical version is more specialised by the type, but less specialised because
		//the two types are different, so the compiler does not know which to choose.
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

	
	template<class T, class V>
	Physical<Unitless, V> floor(const Physical<T, V> &value)
	{
		//Only really makes sense to floor a unitless
		static_assert(T::isUnitless() && T::exponent == 0, "sci:floor only accepts sci::Physical with a sci::Unitless unit with no exponent.");
		return Physical<Unitless, V>(std::floor(value.template value<Unitless>()));
	}

	template<class T, class V>
	Physical<Unitless, V> ceil(const Physical<T, V> &value)
	{
		//Only really makes sense to ceil a unitless
		static_assert(T::isUnitless() && T::exponent == 0, "sci:ceil only accepts sci::Physical with a sci::Unitless unit with no exponent.");
		return Physical<Unitless, V>(std::ceil(value.template value<Unitless>()));
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
		return numerator / unitless((V)denominator);
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
