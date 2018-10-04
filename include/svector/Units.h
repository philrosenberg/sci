#pragma once
#include<vector>
namespace sci
{
	template<uint64_t BASE_POWERS, int64_t EXPONENT>
	struct EncodedUnit
	{
		static const uint64_t basePowers = BASE_POWERS;
		static const int64_t exponent = EXPONENT;
		typedef EncodedUnit< basePowers, exponent > unit;
	};

	template<int8_t POWER, int BYTE>
	constexpr uint64_t encodePower()
	{
		//take the raw bits and cast them to an unsigned 8 bit
		//value, then convert this to a 64 bit value then bitshift
		//it by the appropriate amount
		return uint64_t(uint8_t(POWER)) << (BYTE * 8);
	}

	template <uint64_t POWER, int BYTE>
	constexpr int8_t decodePower()
	{
		//this should filter out the correct byte, bitshift it to the
		return int8_t((POWER & (uint64_t(0xff) << (BYTE * 8))) >> (BYTE * 8));
	}

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


	template <class T, class U>
	struct UnitMultiply
	{
		typedef EncodedUnit< multiplyPowers< T::unit::basePowers, T::unit::basePowers >(), T::unit::exponent + U::unit::exponent> unit;
	};

	template <class T, class U>
	struct UnitDivide
	{
		typedef EncodedUnit< dividePowers<T::unit::basePowers, T::unit::basePowers>(), T::unit::exponent - U::unit::exponent> unit;
	};

	//The base SI units, plus a couple of derived ones
	//Note I have gram, which can have exponent, but kilogram
	//cannot - would be ridiculouse to have a millikilogram
	template<int64_t EXPONENT = 0>
	struct Unitless : public EncodedUnit<0, EXPONENT>
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Amp : public EncodedUnit<encodePower<POWER, 0>(), EXPONENT*POWER>
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Kelvin : public EncodedUnit<encodePower<POWER, 1>(), EXPONENT*POWER>
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Second : public EncodedUnit<encodePower<POWER, 2>(), EXPONENT*POWER>
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Metre : public EncodedUnit<encodePower<POWER, 3>(), EXPONENT*POWER>
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Gram : public EncodedUnit<encodePower<POWER, 4>(), EXPONENT*POWER>
	{
	};

	template<int8_t POWER = 1>
	struct Kilogram : public EncodedUnit<encodePower<POWER, 4>(), 3 * POWER>
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Candela : public EncodedUnit<encodePower<POWER, 5>(), EXPONENT*POWER>
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Mole : public EncodedUnit<encodePower<POWER, 6>(), EXPONENT*POWER>
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Radian : public EncodedUnit<encodePower<POWER, 7>(), EXPONENT*POWER>
	{
	};

	//When defining these derived units we simply pass the exponent to a
	//single linearly dependent base unit and the multiplication by the power will happen at
	//the end


	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Steradian : public UnitMultiply<Radian<POWER>, Radian<POWER, EXPONENT>>::unit //Note we don't have any linear base units so we split the rd^2 into rd and rd and just pass the exponent through one
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Hertz : public Second<-1 * POWER, -EXPONENT>//note the negative exponent because 1 mHz= 1ks-1 i.e 1/1ks
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Newton : public UnitMultiply<UnitMultiply<Kilogram<POWER>, Metre<POWER, EXPONENT>>, Second<-2 * POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Pascal : public UnitMultiply<Newton<POWER, EXPONENT>, Metre<-2 * POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Joule : public UnitMultiply<Newton<POWER, EXPONENT>, Metre<POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Watt : public UnitMultiply<Joule<POWER, EXPONENT>, Second<-POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Coulomb : public UnitMultiply<Amp<POWER, EXPONENT>, Second<POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Volt : public UnitMultiply<Watt<POWER, EXPONENT>, Amp<-POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Farad : public UnitMultiply<Coulomb<POWER, EXPONENT>, Volt<-POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Ohm : public UnitMultiply<Volt<POWER, EXPONENT>, Amp<-POWER>>::unit
	{
	};


	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Seimens : public Ohm<-1 * POWER, -EXPONENT>//note the negative exponent because 1 mS= 1kOhm-1, i.e 1/1kOhm
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Weber : public UnitMultiply<Volt<POWER, EXPONENT>, Second<POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Tesla : public UnitMultiply<Weber<POWER, EXPONENT>, Metre<-2 * POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Henry : public UnitMultiply<Weber<POWER, EXPONENT>, Amp<-POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Lumen : public UnitMultiply<Candela<POWER, EXPONENT>, Steradian<-POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Lux : public UnitMultiply<Lumen<POWER, EXPONENT>, Metre<-2 * POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Becquerel : public Second<-POWER, EXPONENT>
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Gray : public UnitMultiply<Joule<POWER, EXPONENT>, Kilogram<-POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Seivert : public UnitMultiply<Joule<POWER, EXPONENT>, Kilogram<-POWER>>::unit
	{
	};

	template<int8_t POWER = 1, int64_t EXPONENT = 0>
	struct Katal : public UnitMultiply<Mole<POWER, EXPONENT>, Second<-POWER>>::unit
	{
	};


	template < class T >
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
		Physical<T> &operator=(const U& other)
		{
			static_assert(basePowers == U::basePowers, "Cannot create a physical value from another physical value with different powers for the base units.");
			m_v = other.m_v*pow10<U::exponent - exponent>();
			return *this;
		}
		//although trying to set a Physical with a double causes a compile error with just
		//the above, the error message is pretty cryptic. This should give something more
		//sensible
		template<>
		Physical<T> &operator=(const double& other)
		{
			static_assert(false, "Cannot assign a physical value from a raw double. Use the Physical constructor to create a Physical first.");
		}

		static const uint64_t basePowers = T::basePowers;
		static const int64_t exponent = T::exponent;
		typedef T unit;
		double m_v;
	};

	template <class T, class U>
	class Physical2 : public Physical<typename UnitMultiply<typename T::unit, typename U::unit>::unit>
	{
	public:
		Physical2() {}
		Physical2(double v) : Physical(v) {}
		template<class V>
		Physical2(const Physical<V> &other)
		{
			static_assert(basePowers == V::basePowers, "Cannot create a physical value from another physical value with different powers for the base units.");
			m_v = other.m_v*pow10<Physical<V>::exponent - exponent>();
		}
		template<class V, class W>
		Physical2(const Physical2<V, W> &other)
		{
			static_assert(basePowers == Physical2<V, W>::basePowers, "Cannot create a physical value from another physical value with different powers for the base units.");
			m_v = other.m_v*pow10<Physical2<V, W>::exponent - exponent>();
		}
		template<class V>
		Physical2<T, U> &operator=(const V& other)
		{
			static_assert(basePowers == V::basePowers, "Cannot create a physical value from another physical value with different powers for the base units.");
			m_v = other.m_v*pow10<V::exponent - exponent>();
			return *this;
		}
		typedef typename UnitMultiply< typename T::unit, typename U::unit>::unit unit;
	};

	template<class REQUIRED_PHYSICAL, class U>
	double physicalsToValues(const U &physical)
	{
		return REQUIRED_PHYSICAL(physical).m_v;
	}
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
	Physical<EncodedUnit<multiplyPowers<T::basePowers, U::basePowers>(), T::exponent + U::exponent>> operator*(const T &first, const U &second)
	{
		return Physical<EncodedUnit<multiplyPowers<T::basePowers, U::basePowers>(), T::exponent + U::exponent>>(first.m_v*second.m_v);
	}

	// / operator
	template <class T, class U>
	Physical<EncodedUnit<dividePowers<T::basePowers, U::basePowers>(), T::exponent - U::exponent>> operator/(const T &first, const U &second)
	{
		return Physical<EncodedUnit<dividePowers<T::basePowers, U::basePowers>(), T::exponent - U::exponent>>(first.m_v / second.m_v);
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
		return Physical<EncodedUnit<T::basePowers, T::exponent>>(first.m_v + Physical<T>(second).m_v);
	}

	//- operator
	template <class T, class U>
	Physical<T> operator-(const Physical<T> &first, const Physical<U> &second)
	{
		static_assert(T::basePowers == U::basePowers, "Cannot subtract two physical values with different powers for the base units.");
		return Physical<EncodedUnit<T::basePowers, T::exponent>>(first.m_v - Physical<T>(second).m_v);
	}

	//uniary- operator
	template <class T>
	Physical<EncodedUnit<T::basePowers, T::exponent>> &operator-(Physical<T> &val)
	{
		val.m_val = -val.m_val;
		return val;
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
		Physical<Dimensionless> powerVal(power.m_v);
		return Physical<T>(std::pow(base.m_v, powerVal.m_v)*std::pow(10, T::exponent*(powerVal.m_v - 1.0)));
	}

	// power - this case deals with raising to the power of an integer.
	//Note is has a templated argument otherwise we would not know at
	//compile time what the return type would be.
	template <long POWER, class T>
	Physical<EncodedUnit<powPowers<T::basePowers, POWER>(), T::exponent*POWER>> pow(const Physical<T> &base)
	{
		return Physical<EncodedUnit<powPowers<T::basePowers, POWER>(), T::exponent*POWER>>(std::pow(base.m_v, POWER));
	}

	template <class T>
	Physical<EncodedUnit<0, 0>> log(const Physical<T> &value)
	{
		static_assert(T::basePowers == 0, "We can only log a dimensionless quantity.");
		return Physical<EncodedUnit<0, 0>>(std::log(value.m_v*pow10<T::exponent>()));
	}

	template <class T>
	Physical<EncodedUnit<0, 0>> log10(const Physical<T> &value)
	{
		static_assert(T::basePowers == 0, "We can only log a dimensionless quantity.");
		return Physical<EncodedUnit<0, 0>>(std::log10(value.m_v*pow10<T::exponent>()));
	}

	template <class T>
	Physical<EncodedUnit<0, 0>> log2(const Physical<T> &value)
	{
		static_assert(T::basePowers == 0, "We can only log a dimensionless quantity.");
		return Physical<EncodedUnit<0, 0>>(std::log2(value.m_v*pow10<T::exponent>()));
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
		return numerator / unitless(denominator);
	}
}