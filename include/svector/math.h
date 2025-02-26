#ifndef svectormath_h
#define svectormath_h

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif
#include<cmath>
#ifndef M_PI
#define M_PI       3.14159265358979323846
#endif


namespace sci
{
	//these accept const T & references to match the specializations for sci::GridData

	template<int POW, class T>
	auto pow(const double &base)
	{
		return std::pow(base, POW);
	}

	template<int ROOT, class T>
	auto root(const T &base)
	{
		return std::pow(base, 1.0f/float(ROOT));
	}

	template<class T, class U>
	auto pow(const T &base, const U &power)
	{
		return std::pow(base, power);
	}

	template<class T>
	auto sqrt(const T &value)
	{
		return std::sqrt(value);
	}

	template<class T>
	auto abs(const T &value)
	{
		return std::abs(value);
	}

	template<class T>
	auto log10(const T &value)
	{
		return std::log10(value);
	}

	template<class T>
	auto erf(const T &value)
	{
		return std::erf(value);
	}
	double erfInverse(double a);
	float erfInverse(float a);
}


#ifdef MUSTUNDEFNOMINMAX
#undef NOMINMAX
#endif

#endif
