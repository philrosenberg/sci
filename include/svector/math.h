#ifndef svectormath_h
#define svectormath_h

#include<cmath>


namespace sci
{
	//standard cmath math constants
	const double m_e = 2.71828182845904523536;// e
	const double m_log2e = 1.44269504088896340736;// log2(e)
	const double m_log10e = 0.434294481903251827651;// log10(e)
	const double m_ln2 = 0.693147180559945309417;// ln(2)
	const double m_ln10 = 2.30258509299404568402;// ln(10)
	const double m_pi = 3.14159265358979323846;// pi
	const double m_pi_2 = 1.57079632679489661923;// pi/2
	const double m_pi_4 = 0.785398163397448309616;// pi/4
	const double m_1_pi = 0.318309886183790671538;// 1/pi
	const double m_2_pi = 0.636619772367581343076;// 2/pi
	const double m_2_sqrtpi = 1.12837916709551257390;// 2/sqrt(pi)
	const double m_sqrt2 = 1.41421356237309504880;// sqrt(2)
	const double m_sqrt1_2 = 0.707106781186547524401;// 1/sqrt(2)

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
	T abs(const T &value)
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

	inline float erfInverse(float a)
	{
		float p, r, t;
		t = fmaf(a, 0.0f - a, 1.0f);
		t = std::log(t);
		if (fabsf(t) > 6.125f) { // maximum ulp error = 2.35793
			p = 3.03697567e-10f; //  0x1.4deb44p-32 
			p = fmaf(p, t, 2.93243101e-8f); //  0x1.f7c9aep-26 
			p = fmaf(p, t, 1.22150334e-6f); //  0x1.47e512p-20 
			p = fmaf(p, t, 2.84108955e-5f); //  0x1.dca7dep-16 
			p = fmaf(p, t, 3.93552968e-4f); //  0x1.9cab92p-12 
			p = fmaf(p, t, 3.02698812e-3f); //  0x1.8cc0dep-9 
			p = fmaf(p, t, 4.83185798e-3f); //  0x1.3ca920p-8 
			p = fmaf(p, t, -2.64646143e-1f); // -0x1.0eff66p-2 
			p = fmaf(p, t, 8.40016484e-1f); //  0x1.ae16a4p-1 
		}
		else { // maximum ulp error = 2.35456
			p = 5.43877832e-9f;  //  0x1.75c000p-28 
			p = fmaf(p, t, 1.43286059e-7f); //  0x1.33b458p-23 
			p = fmaf(p, t, 1.22775396e-6f); //  0x1.49929cp-20 
			p = fmaf(p, t, 1.12962631e-7f); //  0x1.e52bbap-24 
			p = fmaf(p, t, -5.61531961e-5f); // -0x1.d70c12p-15 
			p = fmaf(p, t, -1.47697705e-4f); // -0x1.35be9ap-13 
			p = fmaf(p, t, 2.31468701e-3f); //  0x1.2f6402p-9 
			p = fmaf(p, t, 1.15392562e-2f); //  0x1.7a1e4cp-7 
			p = fmaf(p, t, -2.32015476e-1f); // -0x1.db2aeep-3 
			p = fmaf(p, t, 8.86226892e-1f); //  0x1.c5bf88p-1 
		}
		r = a * p;
		return float(r);
	}

	inline double erfInverse(double a)
	{
		return (double)sci::erfInverse((float)a);
	}

	template<class T>
	T floor(const T &value)
	{
		return std::floor(value);
	}

	template<class T>
	T ceil(const T& value)
	{
		return std::ceil(value);
	}

	template<class T>
	T round(T n)
	{
		if (n > T(0.0))
			return floor(n + T(0.5));
		else
			return ceil(n - T(0.5));
	}
}

#endif
