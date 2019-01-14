#pragma once
#include<vector>
#include<svector/sstring.h>
namespace sci
{
	template<int64_t EXPONENT>
	struct ExponentTraits
	{
		const static bool valid = false;
		static sci::string getName() { static_assert(false, "Cannot have an exponent not represented by the standard metric prefixes"); }
		static sci::string getPrefix() { static_assert(false, "Cannot have an exponent not represented by the standard metric prefixes"); }
	};
	template<>
	struct ExponentTraits<24>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("yotta"); }
		static sci::string getPrefix() { return sU("Y"); }
	};
	template<>
	struct ExponentTraits<21>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("zetta"); }
		static sci::string getPrefix() { return sU("Z"); }
	};
	template<>
	struct ExponentTraits<18>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("exa"); }
		static sci::string getPrefix() { return sU("E"); }
	};
	template<>
	struct ExponentTraits<15>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("peta"); }
		static sci::string getPrefix() { return sU("P"); }
	};
	template<>
	struct ExponentTraits<12>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("tera"); }
		static sci::string getPrefix() { return sU("T"); }
	};
	template<>
	struct ExponentTraits<9>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("giga"); }
		static sci::string getPrefix() { return sU("G"); }
	};
	template<>
	struct ExponentTraits<6>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("mega"); }
		static sci::string getPrefix() { return sU("M"); }
	};
	template<>
	struct ExponentTraits<3>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("kilo"); }
		static sci::string getPrefix() { return sU("k"); }
	};
	template<>
	struct ExponentTraits<2>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("hecto"); }
		static sci::string getPrefix() { return sU("h"); }
	};
	template<>
	struct ExponentTraits<1>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("deca"); }
		static sci::string getPrefix() { return sU("da"); }
	};
	template<>
	struct ExponentTraits<0>
	{
		const static bool valid = true;
		static sci::string getName() { return sU(""); }
		static sci::string getPrefix() { return sU(""); }
	};
	template<>
	struct ExponentTraits<-1>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("deci"); }
		static sci::string getPrefix() { return sU("d"); }
	};
	template<>
	struct ExponentTraits<-2>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("centi"); }
		static sci::string getPrefix() { return sU("c"); }
	};
	template<>
	struct ExponentTraits<-3>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("milli"); }
		static sci::string getPrefix() { return sU("m"); }
	};
	template<>
	struct ExponentTraits<-6>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("micro"); }
		static sci::string getPrefix() { return sU("\u00b5"); }
	};
	template<>
	struct ExponentTraits<-9>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("nano"); }
		static sci::string getPrefix() { return sU("n"); }
	};
	template<>
	struct ExponentTraits<-12>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("pico"); }
		static sci::string getPrefix() { return sU("p"); }
	};
	template<>
	struct ExponentTraits<-15>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("femto"); }
		static sci::string getPrefix() { return sU("f"); }
	};
	template<>
	struct ExponentTraits<-18>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("atto"); }
		static sci::string getPrefix() { return sU("a"); }
	};
	template<>
	struct ExponentTraits<-21>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("zepto"); }
		static sci::string getPrefix() { return sU("z"); }
	};
	template<>
	struct ExponentTraits<-24>
	{
		const static bool valid = true;
		static sci::string getName() { return sU("yocto"); }
		static sci::string getPrefix() { return sU("y"); }
	};

	//build a string showing the unit. Note that exponent must be before multiplying by power
	template<int64_t EXPONENT>
	sci::string makeShortName(const sci::string &unit, int8_t power, const sci::string &powerPrefix, const sci::string &powerSuffix)
	{
		if (EXPONENT == 0)
			return sU("");
		if (EXPONENT == 1)
			return unit;

		sci::ostringstream stream;
		stream << ExponentTraits<EXPONENT>::getPrefix() << unit << powerPrefix << power << powerSuffix;
		return stream.str();
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
	};

	//This is a class that represents an encoded unit to a power. It is still an EncodedUnit
	//by inheritance, so you can multiply, divide or raise to the power many units
	template<class ENCODEDUNIT, int POW>
	struct PoweredEncodedUnit : public EncodedUnit<powPowers<ENCODEDUNIT::basePowers, POW>(), ENCODEDUNIT::exponent * POW>
	{
		static sci::string getShortName(const sci::string &exponentPrefix = sU(""), const sci::string &exponentSuffix = sU(""))
		{
			return ENCODEDUNIT::getShortName(exponentPrefix, exponentSuffix, POW);
		}
	};

	//This is a class that represents two encoded units multiplied together. It is still an EncodedUnit
	//by inheritance, so you can multiply, divide or raise to the power many units.
	template<class ENCODEDUNIT1, class ENCODEDUNIT2>
	struct MultipliedEncodedUnit : public EncodedUnit<multiplyPowers<ENCODEDUNIT1::basePowers, ENCODEDUNIT2::basePowers>(), ENCODEDUNIT1::exponent + ENCODEDUNIT2::exponent>
	{
		static sci::string getShortName(const sci::string &exponentPrefix = sU(""), const sci::string &exponentSuffix = sU(""))
		{
			return ENCODEDUNIT1::getShortName(exponentPrefix, exponentSuffix) + sU(" ") + ENCODEDUNIT2::getShortName(exponentPrefix, exponentSuffix);
		}
	};

	//This is a class that represents two encoded units divided by each other. It is still an EncodedUnit
	//by inheritance, so you can multiply, divide or raise to the power many units
	template<class ENCODEDUNIT1, class ENCODEDUNIT2>
	struct DividedEncodedUnit : public MultipliedEncodedUnit<ENCODEDUNIT1, PoweredEncodedUnit<ENCODEDUNIT2, -1> >
	{
	};

	//The base SI units, plus a couple of derived ones
	//Note I have gram, which can have exponent, but kilogram
	//cannot - would be ridiculouse to have a millikilogram
	//FOR ALL THESE THE EXPONENT IS NOT PREMULTIPLIED as it is in EncodedUnit - it's as you would write it
	//so for a mm^2 you would use metre<2,-3>

	//This is a macro used to shrthand the definition of functions in each of the SI units
#define NAMEDEF(SHORTNAME)\
	static sci::string getShortRepresentation(const sci::string &exponentPrefix = sU(""), const sci::string &exponentSuffix = sU(""))\
	{\
		return getShortRepresentation(exponentPrefix, exponentSuffix, 1);\
	}\
	static sci::string getShortRepresentation(const sci::string &exponentPrefix, const sci::string &exponentSuffix, int raisedByPower)\
	{\
		return makeShortName<EXPONENT>(getShortName(), POWER*raisedByPower, exponentPrefix, exponentSuffix);\
	}\
	static sci::string getShortName()\
	{\
		return SHORTNAME;\
	}

	template<int64_t EXPONENT = 0>
	struct Unitless : public EncodedUnit<0, EXPONENT>
	{
		NAMEDEF(sU(""))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Amp : public EncodedUnit<encodePower<POWER, 0>(), EXPONENT*POWER>
	{
		NAMEDEF(sU("A"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Kelvin : public EncodedUnit<encodePower<POWER, 1>(), EXPONENT*POWER>
	{
		NAMEDEF(sU("K"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Second : public EncodedUnit<encodePower<POWER, 2>(), EXPONENT*POWER>
	{
		NAMEDEF(sU("s"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Metre : public EncodedUnit<encodePower<POWER, 3>(), EXPONENT*POWER>
	{
		NAMEDEF(sU("m"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Gram : public EncodedUnit<encodePower<POWER, 4>(), EXPONENT*POWER>
	{
		NAMEDEF(sU("g"))
	};

	template<int8_t POWER = 1>
	struct Kilogram : public Gram<POWER, 3>
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Candela : public EncodedUnit<encodePower<POWER, 5>(), EXPONENT*POWER>
	{
		NAMEDEF(sU("cd"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Mole : public EncodedUnit<encodePower<POWER, 6>(), EXPONENT*POWER>
	{
		NAMEDEF(sU("mol"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Radian : public EncodedUnit<encodePower<POWER, 7>(), EXPONENT*POWER>
	{
		NAMEDEF(sU("rad"))
	};

	//When defining these derived units we simply pass the exponent to a
	//single linearly dependent base unit and the multiplication by the power will happen at
	//the end


	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Steradian : public MultipliedEncodedUnit<Radian<POWER>, Radian<POWER, EXPONENT>> //Note we don't have any linear base units so we split the rd^2 into rd and rd and just pass the exponent through one
	{
		NAMEDEF(sU("sr"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Hertz : public Second<-1 * POWER, -EXPONENT>//note the negative exponent because 1 mHz= 1ks-1 i.e 1/1ks
	{
		NAMEDEF(sU("Hz"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Newton : public MultipliedEncodedUnit<MultipliedEncodedUnit<Kilogram<POWER>, Metre<POWER, EXPONENT>>, Second<-2 * POWER>>
	{
		NAMEDEF(sU("N"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Pascal : public MultipliedEncodedUnit<Newton<POWER, EXPONENT>, Metre<-2 * POWER>>
	{
		NAMEDEF(sU("Pa"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Joule : public MultipliedEncodedUnit<Newton<POWER, EXPONENT>, Metre<POWER>>
	{
		NAMEDEF(sU("J"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Watt : public MultipliedEncodedUnit<Joule<POWER, EXPONENT>, Second<-POWER>>
	{
		NAMEDEF(sU("W"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Coulomb : public MultipliedEncodedUnit<Amp<POWER, EXPONENT>, Second<POWER>>
	{
		NAMEDEF(sU("C"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Volt : public MultipliedEncodedUnit<Watt<POWER, EXPONENT>, Amp<-POWER>>
	{
		NAMEDEF(sU("V"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Farad : public MultipliedEncodedUnit<Coulomb<POWER, EXPONENT>, Volt<-POWER>>
	{
		NAMEDEF(sU("F"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Ohm : public MultipliedEncodedUnit<Volt<POWER, EXPONENT>, Amp<-POWER>>
	{
		NAMEDEF(sU("\u2126"))
	};


	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Seimens : public Ohm<-1 * POWER, -EXPONENT>//note the negative exponent because 1 mS= 1kOhm-1, i.e 1/1kOhm
	{
		NAMEDEF(sU("S"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Weber : public MultipliedEncodedUnit<Volt<POWER, EXPONENT>, Second<POWER>>
	{
		NAMEDEF(sU("Wb"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Tesla : public MultipliedEncodedUnit<Weber<POWER, EXPONENT>, Metre<-2 * POWER>>
	{
		NAMEDEF(sU("T"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Henry : public MultipliedEncodedUnit<Weber<POWER, EXPONENT>, Amp<-POWER>>
	{
		NAMEDEF(sU("H"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Lumen : public MultipliedEncodedUnit<Candela<POWER, EXPONENT>, Steradian<-POWER>>
	{
		NAMEDEF(sU("lm"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Lux : public MultipliedEncodedUnit<Lumen<POWER, EXPONENT>, Metre<-2 * POWER>>::unit
	{
		NAMEDEF(sU("lx"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Becquerel : public Second<-POWER, EXPONENT>
	{
		NAMEDEF(sU("Bq"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Gray : public MultipliedEncodedUnit<Joule<POWER, EXPONENT>, Kilogram<-POWER>>
	{
		NAMEDEF(sU("Gy"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Seivert : public MultipliedEncodedUnit<Joule<POWER, EXPONENT>, Kilogram<-POWER>>
	{
		NAMEDEF(sU("Sv"))
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Katal : public MultipliedEncodedUnit<Mole<POWER, EXPONENT>, Second<-POWER>>
	{
		NAMEDEF(sU("kat"))
	};


	template < class ENCODED_UNIT >
	class Physical
	{
	public:
		Physical() {}
		Physical(double v) : m_v(v) {};
		template<class U>
		Physical(const Physical<U> &other)
		{
			static_assert(basePowers == U::basePowers, "Cannot create a physical value from another physical value with different powers for the base units.");
			m_v = other.m_v*pow10<U::exponent - exponent>();
		}
		template<class U>
		Physical<ENCODED_UNIT> &operator=(const U& other)
		{
			static_assert(basePowers == U::basePowers, "Cannot create a physical value from another physical value with different powers for the base units.");
			m_v = other.m_v*pow10<U::exponent - exponent>();
			return *this;
		}
		//although trying to set a Physical with a double causes a compile error with just
		//the above, the error message is pretty cryptic. This should give something more
		//sensible
		template<>
		Physical<ENCODED_UNIT> &operator=(const double& other)
		{
			static_assert(false, "Cannot assign a physical value from a raw double. Use the Physical constructor to create a Physical first.");
		}

		static const uint64_t basePowers = ENCODED_UNIT::basePowers;
		static const int64_t exponent = ENCODED_UNIT::exponent;
		typedef ENCODED_UNIT unit;
		double m_v;
	};

	//This is used to get the value of a Physical with a different prefix, e.g. if you have
	//a physical as mm, you can use it to get the value in km
	template<class REQUIRED_PHYSICAL, class U>
	double physicalsToValues(const U &physical)
	{
		return REQUIRED_PHYSICAL(physical).m_v;
	}
	//This is used to get the value of a vector of Physical with a different prefix, e.g. if you have
	//a vector of Physical as mm, you can use it to get the values in km
	template<class REQUIRED_PHYSICAL, class U>
	auto physicalsToValues(const std::vector<U> &physicals) -> std::vector<decltype(physicalsToValues<REQUIRED_PHYSICAL>(physicals[0]))>
	{
		std::vector<decltype(physicalsToValues<REQUIRED_PHYSICAL>(physicals[0]))> result(physicals.size());
		for (size_t i = 0; i < result.size(); ++i)
			result[i] = physicalsToValues<REQUIRED_PHYSICAL>(physicals[i]);
		return result;
	}

	//* operator
	template <class T, class U>
	Physical<MultipliedEncodedUnit<T, U>> operator*(const Physical<T> &first, const Physical<U> &second)
	{
		return Physical<MultipliedEncodedUnit<T, U>>(first.m_v*second.m_v);
	}

	// / operator
	template <class T, class U>
	Physical<DividedEncodedUnit<T, U>> operator/(const Physical<T> &first, const Physical<U> &second)
	{
		return Physical <DividedEncodedUnit<T, U>>(first.m_v / second.m_v);
	}

	//Hopefully these make raising 10 to the power zero get
	//determined at compile time as it will be a common operation
	//when adding and subtracting
	template <int pow>
	constexpr double pow10()
	{
		return std::pow(10, pow);
	}

	template<>
	constexpr double pow10<0>()
	{
		return 1;
	}

	//+ operator
	template <class T, class U>
	Physical<T> operator+(const Physical<T> &first, const Physical<U> &second)
	{
		static_assert(T::basePowers == U::basePowers, "Cannot add two physical values with different powers for the base units.");
		return Physical<T>(first.m_v + Physical<T>(second).m_v);
	}

	//- operator
	template <class T, class U>
	Physical<T> operator-(const Physical<T> &first, const Physical<U> &second)
	{
		static_assert(T::basePowers == U::basePowers, "Cannot subtract two physical values with different powers for the base units.");
		return Physical<T>(first.m_v - Physical<T>(second).m_v);
	}

	//uniary- operator
	template <class T>
	Physical<T> operator-(const Physical<T> &val)
	{
		return Physical<T>(-val.m_val);
	}

	//+= operator
	template <class T, class U>
	Physical<T> &operator+=(Physical<T> &first, const Physical<U> &second)
	{
		static_assert(T::basePowers == U::basePowers, "Cannot add two physical values with different powers for the base units.");
		first.m_v += Physical<T>(second).m_v;
		return first;
	}

	//-= operator
	template <class T, class U>
	Physical<T> &operator-=(Physical<T> &first, const Physical<U> &second)
	{
		static_assert(T::basePowers == U::basePowers, "Cannot subtract two physical values with different powers for the base units.");
		first.m_v -= Physical<T>(second).m_v;
		return first;
	}

	// /= operator
	template <class T, class U>
	Physical<T> &operator/=(Physical<T> &first, const Physical<U> &second)
	{
		static_assert(U::basePowers == 0, "Can only divide assign with a dimensionless quantity.");
		first.m_v /= second.m_v*pow10<U::exponent>();
		return first;
	}

	//*= operator
	template <class T, class U>
	Physical<T> &operator*=(Physical<T> &first, const Physical<U> &second)
	{
		static_assert(U::basePowers == 0, "Can only multiply assign with a dimensionless quantity.");
		first.m_v *= second.m_v*pow10<U::exponent>();
		return first;
	}

	//> operator
	template <class T, class U>
	bool operator>(Physical<T> &first, const Physical<U> &second)
	{
		static_assert(T::basePowers == U::basePowers, "Cannot compare two physical values with different powers for the base units.");
		return first.m_v > Physical<T>(second).m_v;
	}

	//< operator
	template <class T, class U>
	bool operator<(Physical<T> &first, const Physical<U> &second)
	{
		static_assert(T::basePowers == U::basePowers, "Cannot compare two physical values with different powers for the base units.");
		return first.m_v < Physical<T>(second).m_v;
	}

	//== operator
	template <class T, class U>
	bool operator==(Physical<T> &first, const Physical<U> &second)
	{
		static_assert(T::basePowers == U::basePowers, "Cannot compare two physical values with different powers for the base units.");
		return first.m_v == Physical<T>(second).m_v;
	}

	//!= operator
	template <class T, class U>
	bool operator!=(Physical<T> &first, const Physical<U> &second)
	{
		static_assert(T::basePowers == U::basePowers, "Cannot compare two physical values with different powers for the base units.");
		return first.m_v != Physical<T>(second).m_v;
	}

	//>= operator
	template <class T, class U>
	bool operator>=(Physical<T> &first, const Physical<U> &second)
	{
		static_assert(T::basePowers == U::basePowers, "Cannot compare two physical values with different powers for the base units.");
		return first.m_v >= Physical<T>(second).m_v;
	}

	//<= operator
	template <class T, class U>
	bool operator<=(Physical<T> &first, const Physical<U> &second)
	{
		static_assert(T::basePowers == U::basePowers, "Cannot compare two physical values with different powers for the base units.");
		return first.m_v <= Physical<T>(second).m_v;
	}

	// power - this case deals with dimensionless exponents and bases
	template <class T, class U>
	Physical<T> pow(const Physical<T> &base, const Physical<U> &power)
	{
		static_assert(U::basePowers == 0, "We can only raise a physical value to the power of a dimensionless quantity.");
		static_assert(T::basePowers == 0, "We can only raise a physical value to a non-integer power if it is dimensionless.");
		//We don't need to play with the units at all and the impact of the exponent of the
		//power is put in the result value - so it has the same exponent as the base
		Physical<Unitless> powerVal(power.m_v);
		return Physical<T>(std::pow(base.m_v, powerVal.m_v)*std::pow(10, T::exponent*(powerVal.m_v - 1.0)));
	}

	// power - this case deals with raising to the power of an integer.
	//Note is has a templated argument otherwise we would not know at
	//compile time what the return type would be.
	template <int POWER, class T>
	Physical<PoweredEncodedUnit<T, POWER>> pow(const Physical<T> &base)
	{
		return Physical<PoweredEncodedUnit<T, POWER>>(std::pow(base.m_v, POWER));
	}

	template <class T>
	Physical<Unitless<>> log(const Physical<T> &value)
	{
		static_assert(T::basePowers == 0, "We can only log a dimensionless quantity.");
		return Physical<Unitless>(std::log(value.m_v*pow10<T::exponent>()));
	}

	template <class T>
	Physical<Unitless<>> log10(const Physical<T> &value)
	{
		static_assert(T::basePowers == 0, "We can only log a dimensionless quantity.");
		return Physical<Unitless>(std::log10(value.m_v*pow10<T::exponent>()));
	}

	template <class T>
	Physical<Unitless<>> log2(const Physical<T> &value)
	{
		static_assert(T::basePowers == 0, "We can only log a dimensionless quantity.");
		return Physical<Unitless>(std::log2(value.m_v*pow10<T::exponent>()));
	}


	template <class T>
	Physical<Radian<>> asin(const Physical<T> &value)
	{
		static_assert(T::basePowers == 0, "We can only asin a dimensionless quantity.");
		return Physical<Radian<>>(std::asin(value.m_v*pow10<T::exponent>()));
	}

	template <class T>
	Physical<Radian<>> acos(const Physical<T> &value)
	{
		static_assert(T::basePowers == 0, "We can only acos a dimensionless quantity.");
		return Physical<Radian<>>(std::acos(value.m_v*pow10<T::exponent>()));
	}

	template <class T>
	Physical<Radian<>> atan(const Physical<T> &value)
	{
		static_assert(T::basePowers == 0, "We can only atan a dimensionless quantity.");
		return Physical<Radian<>>(std::atan(value.m_v*pow10<T::exponent>()));
	}

	template <class T>
	Physical<Unitless<>> sin(const Physical<T> &value)
	{
		static_assert(T::basePowers == Radian<>::basePowers, "We can only sin a quantity in Radians.");
		return Physical<Unitless>(std::sin(value.m_v*pow10<T::exponent>()));
	}

	template <class T>
	Physical<Unitless<>> cos(const Physical<T> &value)
	{
		static_assert(T::basePowers == Radian<>::basePowers, "We can only cos a quantity in Radians.");
		return Physical<Unitless>(std::cos(value.m_v*pow10<T::exponent>()));
	}

	template <class T>
	Physical<Unitless<>> tan(const Physical<T> &value)
	{
		static_assert(T::basePowers == Radian<>::basePowers, "We can only tan a quantity in Radians.");
		return Physical<Unitless>(std::atan(value.m_v*pow10<T::exponent>()));
	}

	//This is used by averaging algorithms where simply casting size_t to the
	//same type as the type we are averaging doesn't work
	template <class T>
	Physical<T> PhysicalDivide(const Physical<T> &numerator, size_t denominator)
	{
		return numerator / unitless((double)denominator);
	}
}